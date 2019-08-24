/*
 * fake86.c
 *
 *  Created on: 2019. 8. 21.
 *      Author: Baram
 */


#include <math.h>

#include "../common/common.h"
#include "../cpu/cpu.h"


#if defined (__GNUC__)
asm(".section .fake86_pcxtbios_bin,\"a\";"
                         ".incbin \"../src/ap/fake86/data/pcxtbios.bin\";"
);
asm(".section .fake86_rombasic_bin,\"a\";"
                             ".incbin \"../src/ap/fake86/data/rombasic.bin\";"
);
asm(".section .fake86_videorom_bin,\"a\";"
                             ".incbin \"../src/ap/fake86/data/videorom.bin\";"
);
#endif


extern uint32_t fake86_pcxtbios_bin;
extern uint32_t fake86_pcxtbios_bin_end;
extern uint32_t fake86_rombasic_bin;
extern uint32_t fake86_rombasic_bin_end;
extern uint32_t fake86_videorom_bin;
extern uint32_t fake86_videorom_bin_end;


uint32_t frame_buf[640*480] __attribute__((section(".sdram_buf")));

extern void port_init(void);


const char *biosfile = "pcxtbios.bin";


static void cpu_setup(void) {
  struct cpu_io_t io;
  io.ram = RAM;
  io.mem_read_8    = read86;
  io.mem_read_16   = readw86;
  io.mem_write_8   = write86;
  io.mem_write_16  = writew86;
  io.port_read_8   = portin;
  io.port_read_16  = portin16;
  io.port_write_8  = portout;
  io.port_write_16 = portout16;
  io.int_call      = intcall86;
  cpu_set_io(&io);
}

static bool emulate_init(void)
{
  // initialize memory
  mem_init();
  port_init();
  // insert our option rom
  rom_insert();
  // initalize the cpu
  cpu_setup();
  cpu_reset();
  // initalize hardware
  i8253_init();
  i8259_init();
  i8237_init();
  i8255_init();
  cmos_init();
  mouse_init(0x3F8, 4);
  // initalize vga refresh timing
  vga_timing_init();

  if (!neo_init())
  {
    return false;
  }

  return true;
}

static bool load_roms(void)
{
  // load bios
  const uint32_t biossize = mem_loadbios_flash((uint8_t *)&fake86_pcxtbios_bin, (&fake86_pcxtbios_bin_end - &fake86_pcxtbios_bin)*4);
  if (!biossize)
  {
    return false;
  }
  // load other roms
  if (biossize <= (1024 * 8))
  {
    const char *rom_basic = "rombasic.bin";
    if (!mem_loadrom_flash(0xF6000UL, (uint8_t *)&fake86_rombasic_bin, (&fake86_rombasic_bin_end - &fake86_rombasic_bin)*4))
    {
      log_printf(LOG_CHAN_MEM, "unable to load '%s'", rom_basic);
    }
  }
  // load the video bios
  const char *rom_video = "videorom.bin";

  if (!mem_loadrom_flash(0xC0000UL, (uint8_t *)&fake86_videorom_bin, (&fake86_videorom_bin_end - &fake86_videorom_bin)*4))
  {
    log_printf(LOG_CHAN_MEM, "unable to load '%s'", rom_video);
  }
  return true;
}

static void tick_hardware(uint64_t cycles)
{
  // dma controller
  i8237_tick(cycles);
  // PIC controller
  i8259_tick(cycles);
  // PIA
  i8255_tick(cycles);
  //
  vga_timing_advance(cycles);
  // PIT timer
  i8253_tick(cycles);
  // tick audio event stream
  audio_tick(cycles);
  //
  neo_tick(cycles);
}

static int64_t tick_cpu(int64_t num_cycles)
{
  return cpu_exec86((int32_t)num_cycles);
}

#if 0
static void emulate_loop(void)
{
  // enter main emulation loop
  while (cpu_running)
  {
    // set ourselves some cycle targets
    const int64_t target = min(CYCLES_PER_SLICE, i8253_cycles_before_irq());
    // run for some cycles
    const int64_t executed = tick_cpu(target);
    // tick the hardware
    tick_hardware(executed);

    // exit if we are locked up
    if (cpu_in_hlt_state())
    {
      if (cpu_flags.ifl == 0)
      {
        cpu_running = false;
      }
    }

  }
  log_printf(LOG_CHAN_CPU, "cpu reached halt state");
  cpu_dump_state(stdout);
}
#else
static uint64_t get_ticks() {
  return millis();
}

struct render_target_t {
  uint32_t *dst;
  uint32_t w, h, pitch;
};


extern void neo_render_tick(const struct render_target_t *target) ;

static uint8_t *p_data = (uint8_t *)frame_buf;

void setPixel(int x, int y, uint32_t color)
{

  p_data[y*480*3 + x*3 + 2] = color >> 16;
  p_data[y*480*3 + x*3 + 1] = color >> 8;
  p_data[y*480*3 + x*3 + 0] = color >> 0;

}

void tick_render(void) {

  struct render_target_t target = {
    (uint32_t*)frame_buf,
    640,
    480,
    640
  };
  static uint8_t cnt = 0;

  if (cnt%5 == 0)
  {
    neo_render_tick(&target);
  }
  cnt++;
}

static void emulate_loop(void) {

#define CYCLES_PER_REFRESH (CYCLES_PER_SECOND / 30)
#define MSTOCYCLES(X) ((X) * (CYCLES_PER_SECOND / 1000))

  int64_t cpu_acc = 0;
  uint64_t old_ms = get_ticks();

  // enter main emulation loop
  while (cpu_running) {

    // diff cycle time
    const uint64_t new_ms = get_ticks();
    const uint64_t new_cycles = MSTOCYCLES(new_ms - old_ms);
    // avoid fastforwaring on lag
    if (new_ms - old_ms > 1000) {
      old_ms = new_ms;
      if (!cpu_halt)
      {
        continue;
      }
    }
    bool video_redraw = false;
    uint64_t executed = 0;
    // update cpu
    while (cpu_acc <= 0 || cpu_halt) {

      // set ourselves some cycle targets
      int64_t target;
      target = cpu_halt ? 0 : CYCLES_PER_SLICE;
      target = cpu_step ? 1 : target;
      target = min(target, i8253_cycles_before_irq());
      target = max(target, cpu_halt ? 0 : 1);

      // run for some cycles
      executed = tick_cpu(target);
      cpu_acc += executed;


      // disable the stepping flag
      cpu_step = executed ? false : cpu_step;

      // keep track of if the video needs refreshed
      if (vga_timing_should_flip()) {
        video_redraw = true;
        vga_timing_did_flip();
      }
      // tick peripherals
      tick_hardware(executed);

      if (video_redraw) {
        break;
      }

      if (cpu_halt)
      {
        break;
      }
    }

    // refresh the screen buffer
    if (video_redraw || cpu_halt)
    {
      tick_render();
    }
    // parse events from host
    //tick_events();
    // advance cpu or sleep
    if ((int64_t)new_cycles >= cpu_acc)
    {
      cpu_acc -= cpu_halt ? executed : new_cycles;
      old_ms = new_ms;
    }
  }
}
#endif
void fake86Main(void)
{
  emulate_init();
  load_roms();

  cpu_running = true;

  emulate_loop();
}
