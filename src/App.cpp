#include "./App.hpp"
#include "./AnmOpener/AnmManager.h"
#include "./EclFileManager.h"
#include "./GlobalData.h"
#include "./GoastManager.h"
#include "./Hardcoded.h"
#include "./ItemManager.h"
#include "./Player.h"
#include "./Spellcard.h"
#include "./StdOpener/StdFile.h"
#include "./AsciiManager.hpp"
#include "./AsciiPopupManager.hpp"
#include "./Gui.hpp"
#include "Bomb.hpp"
#include "PauseMenu.hpp"
#include "StdOpener/Stage.hpp"
#include <NSEngine.h>

int layerOrder[] = {
  0, 32, 33,
  1, 2, 3, 4, 5, 34,
  6, 7, 8, 9, 10, 11,
  12, 13, 14, 15, 16, 17, 18, 19,
  20, 21, 22, 23, 36, 24, 25, 37,
  26, 27, 38, 28, 39, 29, 40, 30,
  41, 31, 42, 35, 0, 0
};

bool pause = false;
void App::on_create() {
  Hardcoded_Load();

  setMaxFps(60);
  NSEngine::createCamera(NS_CAM_3D, WINDOW_WIDTH, WINDOW_HEIGHT);

  NSEngine::addGameLayer(false, false);
  NSEngine::addGameLayer(false, false);
  NSEngine::addGameLayer(false, false);
  for (int i = 0; i < 40; i++)
    NSEngine::addGameLayer(false, true);
  NSEngine::engineData::layerDrawOrder = layerOrder;


  AnmManager::Init();
  AnmManager::LoadFile(8, "effect.anm");

  std::cout << "Touhou " << TOUHOU_VERSION << "\n";

  auto cam = NSEngine::activeCamera3D();
  cam->setUp(0, 0, -1);
  float dist_coeff = 1.207106781186548;
  glm::mat4 perspS = glm::perspective(PI1_4,
                                      static_cast<float>(WINDOW_WIDTH) /
                                          static_cast<float>(WINDOW_HEIGHT),
                                      0.01f, 10000.f);
  glm::mat4 viewMatrixS = glm::lookAt(
      glm::vec3(0.f, -224.f,
                dist_coeff * static_cast<float>(WINDOW_HEIGHT) / 2.f),
      glm::vec3(0.f, -224.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
  cam->setMatStatic(perspS * viewMatrixS);

  GLOBALS.inner.CHARACTER = 1;
  GLOBALS.inner.SHOTTYPE = 1;
  GLOBALS.inner.CURRENT_POWER = 450;
  GLOBALS.inner.DIFFICULTY = 3;
  GLOBALS.inner.SPELL_ID = -1;

  bm = BulletManager::GetInstance();
  em = EnemyManager::GetInstance();

  if (strlen(m_argv[1]) > 3 && m_argv[1][3] >= '0' && m_argv[1][3] < '8')
      GLOBALS.inner.STAGE_NUM = m_argv[1][3] - '0';

  Bomb::initialize();
  new Player();
  new ItemManager();
  new AsciiManager();
  new AsciiPopupManager();
  new Gui();
  new Spellcard();
  // new Stage();
  em->Start(m_argv[1], m_argc > 2 ? m_argv[2] : "main");
  if (TOUHOU_VERSION == 17)
    new GoastManager();

  new PauseMenu();

  UPDATE_FUNC_REGISTRY->register_on_tick(new UpdateFunc([this](){this->anmViewer.on_tick(); return 1;}), 3);
  UPDATE_FUNC_REGISTRY->register_on_tick(new UpdateFunc(AnmManager::on_tick_ui), 10);
  UPDATE_FUNC_REGISTRY->register_on_tick(new UpdateFunc(AnmManager::on_tick_world), 35);
  UPDATE_FUNC_REGISTRY->register_on_tick(new UpdateFunc([](){return GAME_PAUSED ? 3 : 1;}), 16);
  UPDATE_FUNC_REGISTRY->register_on_tick(new UpdateFunc([](){
    if (EclFileManager::GetInstance()->stdf)
      EclFileManager::GetInstance()->stdf->Update();
    return 1;
  }), 18);
}

void App::on_update() {
  UPDATE_FUNC_REGISTRY->run_all_on_tick();

  if (Inputs::Keyboard().Pressed(NSK_y)) PAUSE_MENU_PTR->open_submenu3();
}

void App::on_render() {
  // NSEngine::draw_set_layer(NSEngine::engineData::debugLayer);

  // __DRAW
  // NSEngine::Color c;
  // c = {255,   0, 255, 255};
  // __MONITOR(BulletManager::GetInstance()->get_nb_bul()/2000.f, c);
  // c = {  0, 255,   0, 255};
  // __MONITOR(1.f-AnmManager::getFreeAnm()/8191.f, c);
  // c = {  0,   0, 255, 255};
  // __MONITOR(EnemyManager::GetInstance()->enemyCount/300.f, c);

  static bool renderStage = true;
  static bool renderUpdFunc = true;
  static bool renderAnmManager = true;
  if (Inputs::Keyboard().Pressed(NSK_a)) renderAnmManager = !renderAnmManager;
  if (Inputs::Keyboard().Pressed(NSK_s)) renderStage = !renderStage;
  if (Inputs::Keyboard().Pressed(NSK_u)) renderUpdFunc = !renderUpdFunc;
  if (renderStage)
    if (EclFileManager::GetInstance()->stdf)
      EclFileManager::GetInstance()->stdf->Draw();
  if (renderUpdFunc)
    UPDATE_FUNC_REGISTRY->run_all_on_draw();
  if (renderAnmManager)
    AnmManager::draw();
  anmViewer.on_draw();

}

void App::on_destroy() {
  EnemyManager::Cleanup();
  delete BulletManager::GetInstance();
  delete Spellcard::GetInstance();
  AnmManager::Cleanup();
  delete PLAYER_PTR;
  delete ITEM_MANAGER_PTR;
  delete ASCII_MANAGER_PTR;
  if (GOAST_MANAGER_PTR) delete GOAST_MANAGER_PTR;
  delete POPUP_MANAGER_PTR;
}
