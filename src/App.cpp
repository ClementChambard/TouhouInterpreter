#include "./App.hpp"
#include "./AnmOpener/AnmManager.h"
#include "./AsciiManager.hpp"
#include "./AsciiPopupManager.hpp"
#include "./Bomb.hpp"
#include "./GlobalData.h"
#include "./GoastManager.h"
#include "./Gui.hpp"
#include "./Hardcoded.h"
#include "./HardcodedFuncs.hpp"
#include "./ItemManager.h"
#include "./PauseMenu.hpp"
#include "./Player.h"
#include "./Spellcard.h"
#include "./StdOpener/Stage.hpp"
#include "./UpdateFuncRegistry.h"
#include "./Supervisor.h"
#include "Laser/LaserManager.h"
#include <string>

void App::on_create() {
  Hardcoded_Load();

  setMaxFps(60);

  anm::init();
  anm::RESOLUTION_MULT = WINDOW_HEIGHT / 480.f;
  anm::BACK_BUFFER_SIZE.x = WINDOW_WIDTH;
  anm::BACK_BUFFER_SIZE.y = WINDOW_HEIGHT;

  SUPERVISOR.init();
  SUPERVISOR.setup_special_anms();

  anm::loadFile(8, "effect.anm");

  ns::info("Touhou", TOUHOU_VERSION);

  HardcodedFuncsInit(TOUHOU_VERSION);

  GLOBALS.inner.CHARACTER = 1;
  GLOBALS.inner.SHOTTYPE = 0;
  GLOBALS.inner.CURRENT_POWER = 450;
  GLOBALS.inner.DIFFICULTY = 3;
  GLOBALS.inner.SPELL_ID = -1;

  if (m_argc > 3)
    GLOBALS.inner.CHARACTER = std::stoi(std::string(m_argv[3]));
  if (m_argc > 4)
    GLOBALS.inner.SHOTTYPE = std::stoi(std::string(m_argv[4]));
  if (strlen(m_argv[1]) > 3 && m_argv[1][3] >= '0' && m_argv[1][3] < '8')
    GLOBALS.inner.STAGE_NUM = m_argv[1][3] - '0';

  bm = BulletManager::GetInstance();
  em = EnemyManager::GetInstance();

  Bomb::initialize();
  new LaserManager();
  new Player();
  new ItemManager();
  new AsciiManager();
  new AsciiPopupManager();
  new Gui();
  new Spellcard();
  new Stage();
  em->Start(m_argv[1], m_argc > 2 ? m_argv[2] : "main");
  if (TOUHOU_VERSION == 17)
    new GoastManager();

  new PauseMenu();

  UPDATE_FUNC_REGISTRY->register_on_tick(new UpdateFunc([this]() {
                                           this->anmViewer.on_tick();
                                           return 1;
                                         }),
                                         3);
  UPDATE_FUNC_REGISTRY->register_on_draw(new UpdateFunc([this]() {
                                           this->anmViewer.on_draw();
                                           return 1;
                                         }),
                                         100);
  UPDATE_FUNC_REGISTRY->register_on_tick(
      new UpdateFunc([]() {
        if (GAME_PAUSED)
          return 3;
        SUPERVISOR.arcade_vm_0->update();
        SUPERVISOR.arcade_vm_1->update();
        SUPERVISOR.arcade_vm_2__handles_upscaling->update();
        SUPERVISOR.arcade_vm_3__handles_seija->update();
        return 1;
      }),
      16);
}

void App::on_update() { UPDATE_FUNC_REGISTRY->run_all_on_tick(); }

void App::on_render() { UPDATE_FUNC_REGISTRY->run_all_on_draw(); }

void App::on_destroy() {
  delete PAUSE_MENU_PTR;
  if (GOAST_MANAGER_PTR)
    delete GOAST_MANAGER_PTR;
  delete STAGE_PTR;
  delete SPELLCARD_PTR;
  delete GUI_PTR;
  delete POPUP_MANAGER_PTR;
  delete ASCII_MANAGER_PTR;
  delete ITEM_MANAGER_PTR;
  delete PLAYER_PTR;
  delete BOMB_PTR;
  EnemyManager::Cleanup();
  delete BulletManager::GetInstance();
  // SUPERVISOR.cleanup();
  anm::cleanup();
}
