#include "EclFileManager.h"
#include "EnemyManager.h"
#include "BulletManager.h"
#include <chrono>
#include <thread>
#include <NSEngine.h>
#include <AnmOpener/AnmManagerN.h>
#include <StdOpener/StdFile.h>
#include "GlobalData.h"
#include "Spellcard.h"

#include "profiler.h"

int main (int argc, char** argv)
{
    if (argc == 1) return 1;

    int windowFlags = 0;
    int windowWidth = 384*2;
    int windowHeight = 448*2;
    //windowWidth = 1920;
    //windowHeight = 1080;

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
    NSEngine::AnmManagerN::Init();
    NSEngine::AnmManagerN::LoadFile(1, "effect.anm");

    auto cam = NSEngine::activeCamera3D();
    cam->setUp(0, 0, -1);
    BulletManager::GetInstance();
    //cam->setMatStatic(glm::ortho(-windowWidth/2.f, windowWidth/2.f, -448.f-windowHeight/2+224, 0.f+windowHeight/2-224));
    float dist_coeff = 1.207106781186548;
    glm::mat4 perspS = glm::perspective(PI1_4, (float)windowWidth / (float)windowHeight, 0.01f, 10000.f);
    glm::mat4 viewMatrixS = glm::lookAt(glm::vec3(0.f, -224.f, dist_coeff*(float)windowHeight/2.f), glm::vec3(0.f, -224.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    cam->setMatStatic(perspS * viewMatrixS);
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
          NSEngine::AnmManagerN::update();


         NSEngine::EndUpdate();

         window.InitDrawing();

         //NSEngine::draw_set_layer(4);
         //NSEngine::draw_rectangle_color(-192.f, -448.f, 192.f, 0.f, {25,25,25,255}, {25,25,25,255}, {25,25,25,255}, {25,25,25,255}, true);
         NSEngine::draw_set_layer(28);
         NSEngine::draw_circle_color(Globals::get()->playerX, -Globals::get()->playerY, 5, {255,255,255,255}, {255,255,255,255});
         NSEngine::draw_set_layer(NSEngine::engineData::debugLayer);
         //__DRAW
         //NSEngine::Color c;
         //c = {255,   0, 255, 255}; __MONITOR(BulletManager::GetInstance()->get_nb_bul()/2000.f, c);
         //c = {  0, 255,   0, 255}; __MONITOR(NSEngine::AnmManagerN::getFreeAnm()/8191.f, c);
         //c = {  0,   0, 255, 255}; __MONITOR(EnemyManager::GetInstance()->enemyCount/300.f, c);

         if (EclFileManager::GetInstance()->stdf) EclFileManager::GetInstance()->stdf->Draw();
         UPDATE_FUNC_REGISTRY->run_all_on_draw();
         NSEngine::AnmManagerN::draw();
         window.EndDrawing();

        NSEngine::EndFrame();
    }
    EnemyManager::Cleanup();
    delete BulletManager::GetInstance();
    delete Spellcard::GetInstance();

    window.destroy();
    NSEngine::AnmManagerN::Cleanup();
    NSEngine::Quit();
    return 0;
}
