#include <asw/asw.h>

#include "./globals.h"
#include "./states/game.h"
#include "./states/init.h"
#include "./states/menu.h"
#include "./states/state.h"

int main() {
  asw::core::init(SCREEN_W, SCREEN_H);

  auto app = asw::scene::SceneManager<ProgramState>();
  app.register_scene<Init>(ProgramState::Init, app);
  app.register_scene<Menu>(ProgramState::Menu, app);
  app.register_scene<Game>(ProgramState::Game, app);
  app.set_next_scene(ProgramState::Init);

  app.start();

  return 0;
}
