#include "EclFileManager.h"
#include "EnemyManager.h"
#include "BulletManager.h"
#include <chrono>
#include <thread>
#include <NSEngine.h>
#include "AnmOpener/AnmManagerN.h"
#include "StdOpener/StdFile.h"
#include "GlobalData.h"
#include "Spellcard.h"
#include "Laser/LaserManager.h"
#include "Hardcoded.h"

#include "profiler.h"

int main (int argc, char** argv)
{
    if (argc == 1) return 1;

    int windowFlags = 0;
    int windowWidth = 384*2;
    int windowHeight = 448*2;
    //windowWidth = 1920;
    //windowHeight = 1080;

    Hardcoded_Load();

    NSEngine::Init();

    NSEngine::Window window;
    window.Init("NS Anm viewer", windowWidth, windowHeight, windowFlags);

    NSEngine::Subsystems();
    NSEngine::setMaxFps(60);
    NSEngine::createCamera(NS_CAM_3D, windowWidth, windowHeight);

    NSEngine::addGameLayer(NSEngine::GLT_SPRITES, false, false);
    NSEngine::addGameLayer(NSEngine::GLT_SPRITES, false, false);
    NSEngine::addGameLayer(NSEngine::GLT_SPRITES, false, false);
    for (int i = 0; i< 40; i++) NSEngine::addGameLayer(NSEngine::GLT_SPRITES, false, true);

    NSEngine::EndInit();

    AnmManagerN::Init();
    AnmManagerN::LoadFile(1, "effect.anm");

    std::cout << "Touhou " << TOUHOU_VERSION << "\n";

    auto cam = NSEngine::activeCamera3D();
    cam->setUp(0, 0, -1);
    float dist_coeff = 1.207106781186548;
    glm::mat4 perspS = glm::perspective(PI1_4, (float)windowWidth / (float)windowHeight, 0.01f, 10000.f);
    glm::mat4 viewMatrixS = glm::lookAt(glm::vec3(0.f, -224.f, dist_coeff*(float)windowHeight/2.f), glm::vec3(0.f, -224.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    cam->setMatStatic(perspS * viewMatrixS);

    auto lasermgr = LaserManager();
    auto bm = BulletManager::GetInstance();

    auto em = EnemyManager::GetInstance();
    em->Start(argv[1], argc > 2 ? argv[2] : "main");

    while(NSEngine::IsRunning())
    {
        NSEngine::StartFrame();

         NSEngine::StartUpdate();
          if (Inputs::Keyboard().Pressed(NSK_f10)) window.nextDisplaymode();
          if (NSEngine::IsFBF() && !Inputs::Keyboard().Pressed(NSK_backspace) && !Inputs::Keyboard().Down(NSK_equals)) continue;
          Globals::get()->playerX = Inputs::Mouse().pos.x/2;
          Globals::get()->playerY = -Inputs::Mouse().pos.y/2+224;

          if (EclFileManager::GetInstance()->stdf) EclFileManager::GetInstance()->stdf->Update();
          UPDATE_FUNC_REGISTRY->run_all_on_tick();

          AnmManagerN::update();

          if (Inputs::Keyboard().Pressed(NSK_n)) {
              AnmManagerN::killAll();
              AnmManagerN::LoadFile(1, "effect.anm");
              AnmManagerN::LoadFile(7, "bullet.anm");
              lasermgr.destroy_all();
              bm->ClearScreen(0);
              em->EnmKillAll();
              Globals::get()->stage_id++;
              em->Start("", argc > 2 ? argv[2] : "main");
          }

         NSEngine::EndUpdate();

         window.InitDrawing();

         NSEngine::draw_set_layer(28);
         NSEngine::draw_circle_color(Globals::get()->playerX, -Globals::get()->playerY, 5, {255,255,255,255}, {255,255,255,255});
         NSEngine::draw_set_layer(NSEngine::engineData::debugLayer);

         //__DRAW
         //NSEngine::Color c;
         //c = {255,   0, 255, 255}; __MONITOR(BulletManager::GetInstance()->get_nb_bul()/2000.f, c);
         //c = {  0, 255,   0, 255}; __MONITOR(1.f-AnmManagerN::getFreeAnm()/8191.f, c);
         //c = {  0,   0, 255, 255}; __MONITOR(EnemyManager::GetInstance()->enemyCount/300.f, c);

         if (EclFileManager::GetInstance()->stdf) EclFileManager::GetInstance()->stdf->Draw();
         UPDATE_FUNC_REGISTRY->run_all_on_draw();
         AnmManagerN::draw();
         window.EndDrawing();


        NSEngine::EndFrame();
    }
    EnemyManager::Cleanup();
    delete BulletManager::GetInstance();
    delete Spellcard::GetInstance();

    window.destroy();
    AnmManagerN::Cleanup();
    NSEngine::Quit();
    return 0;
}
