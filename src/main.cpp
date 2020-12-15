#include "tools.h"
#include "world.h"

bool close_button_pressed;

BITMAP* buffer;

// FPS System
volatile int ticks = 0;
const int updates_per_second = 60;
volatile int game_time = 0;

int fps;
int frames_done;
int old_time;

world game_world;

void ticker() {
  ticks++;
}
END_OF_FUNCTION(ticker)

void game_time_ticker() {
  game_time++;
}
END_OF_FUNCTION(ticker)

void close_button_handler(void) {
  close_button_pressed = TRUE;
}
END_OF_FUNCTION(close_button_handler)

void update() {
  game_world.update();
}

void draw() {
  game_world.draw(buffer);
  draw_sprite(screen, buffer, 0, 0);
}

void setup() {
  game_world.setup();

  buffer = create_bitmap(1024, 768);

  srand(time(NULL));

  // Setup for FPS system
  LOCK_VARIABLE(ticks);
  LOCK_FUNCTION(ticker);
  install_int_ex(ticker, BPS_TO_TIMER(updates_per_second));

  LOCK_VARIABLE(game_time);
  LOCK_FUNCTION(game_time_ticker);
  install_int_ex(game_time_ticker, BPS_TO_TIMER(10));

  // Close button
  LOCK_FUNCTION(close_button_handler);
  set_close_button_callback(close_button_handler);
}

int main() {
  allegro_init();
  install_timer();
  install_keyboard();
  install_mouse();
  set_color_depth(32);

  set_gfx_mode(GFX_AUTODETECT_WINDOWED, 1024, 768, 0, 0);
  install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, ".");

  set_window_title("Gunner 2.0");
  setup();

  while (!key[KEY_ESC] && !close_button_pressed) {
    while (ticks == 0) {
      rest(1);
    }
    while (ticks > 0) {
      int old_ticks = ticks;

      update();

      ticks--;
      if (old_ticks <= ticks) {
        break;
      }
    }
    if (game_time - old_time >= 10) {
      fps = frames_done;
      frames_done = 0;
      old_time = game_time;
    }
    draw();
  }

  return 0;
}
END_OF_MAIN()
