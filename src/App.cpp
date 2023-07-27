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
#include <NSEngine.h>

void App::on_create() {
  Hardcoded_Load();

  setMaxFps(60);
  NSEngine::createCamera(NS_CAM_3D, WINDOW_WIDTH, WINDOW_HEIGHT);

  NSEngine::addGameLayer(false, false);
  NSEngine::addGameLayer(false, false);
  NSEngine::addGameLayer(false, false);
  for (int i = 0; i < 40; i++)
    NSEngine::addGameLayer(false, true);

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

  GLOBALS.inner.CHARACTER = 0;
  GLOBALS.inner.SHOTTYPE = 0;
  GLOBALS.inner.CURRENT_POWER = 450;
  GLOBALS.inner.DIFFICULTY = 3;
  GLOBALS.inner.SPELL_ID = -1;

  bm = BulletManager::GetInstance();
  em = EnemyManager::GetInstance();

  if (strlen(m_argv[1]) > 3 && m_argv[1][3] >= '0' && m_argv[1][3] < '8')
      GLOBALS.inner.STAGE_NUM = m_argv[1][3] - '0';

  new Player();
  new ItemManager();
  new AsciiManager();
  new AsciiPopupManager();
  new Gui();
  new Spellcard();
  em->Start(m_argv[1], m_argc > 2 ? m_argv[2] : "main");
  if (TOUHOU_VERSION == 17)
    new GoastManager();
}

void App::on_update() {
  if (EclFileManager::GetInstance()->stdf)
    EclFileManager::GetInstance()->stdf->Update();
  UPDATE_FUNC_REGISTRY->run_all_on_tick();

  AnmManager::update();

  anmViewer.on_tick();

  // if (Inputs::Keyboard().Pressed(NSK_n)) {
  //   AnmManager::killAll();
  //   AnmManager::LoadFile(1, "effect.anm");
  //   AnmManager::LoadFile(7, "bullet.anm");
  //   lm.destroy_all();
  //   bm->ClearScreen(0);
  //   em->EnmKillAll();
  //   GLOBALS.inner.STAGE_NUM++;
  //   em->Start("", m_argc > 2 ? m_argv[2] : "main");
  // }
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
