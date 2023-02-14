#include "App.hpp"
#include "EclFileManager.h"
#include <chrono>
#include <thread>
#include <NSEngine.h>
#include "AnmOpener/AnmManager.h"
#include "StdOpener/StdFile.h"
#include "GlobalData.h"
#include "Spellcard.h"
#include "Hardcoded.h"
#include "Player.h"

void App::on_create() {
    Hardcoded_Load();

    setMaxFps(60);
    NSEngine::createCamera(NS_CAM_3D, WINDOW_WIDTH, WINDOW_HEIGHT);

    NSEngine::addGameLayer(false, false);
    NSEngine::addGameLayer(false, false);
    NSEngine::addGameLayer(false, false);
    for (int i = 0; i< 40; i++) NSEngine::addGameLayer(false, true);

    AnmManager::Init();
    AnmManager::LoadFile(1, "effect.anm");

    std::cout << "Touhou " << TOUHOU_VERSION << "\n";

    auto cam = NSEngine::activeCamera3D();
    cam->setUp(0, 0, -1);
    float dist_coeff = 1.207106781186548;
    std::cout << WINDOW_WIDTH << " " << WINDOW_HEIGHT << "\n";
    glm::mat4 perspS = glm::perspective(PI1_4, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 10000.f);
    glm::mat4 viewMatrixS = glm::lookAt(glm::vec3(0.f, -224.f, dist_coeff*(float)WINDOW_HEIGHT/2.f), glm::vec3(0.f, -224.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    cam->setMatStatic(perspS * viewMatrixS);

    bm = BulletManager::GetInstance();
    em = EnemyManager::GetInstance();

    em->Start(m_argv[1], m_argc > 2 ? m_argv[2] : "main");
    new Player();
}

void App::on_update() {
    Globals::get()->playerX = PLAYER_PTR->inner.pos.x; //Inputs::Mouse().pos.x/2;
    Globals::get()->playerY = PLAYER_PTR->inner.pos.y;//-Inputs::Mouse().pos.y/2+224;

    if (EclFileManager::GetInstance()->stdf) EclFileManager::GetInstance()->stdf->Update();
    UPDATE_FUNC_REGISTRY->run_all_on_tick();

    AnmManager::update();

    if (Inputs::Keyboard().Pressed(NSK_n)) {
        AnmManager::killAll();
        AnmManager::LoadFile(1, "effect.anm");
        AnmManager::LoadFile(7, "bullet.anm");
        lm.destroy_all();
        bm->ClearScreen(0);
        em->EnmKillAll();
        Globals::get()->stage_id++;
        em->Start("", m_argc > 2 ? m_argv[2] : "main");
    }
}

void App::on_render() {
    //NSEngine::draw_set_layer(28);
    //NSEngine::draw_circle_color(Globals::get()->playerX, -Globals::get()->playerY, 5, {255,255,255,255}, {255,255,255,255});
    //NSEngine::draw_set_layer(NSEngine::engineData::debugLayer);

    //__DRAW
    //NSEngine::Color c;
    //c = {255,   0, 255, 255}; __MONITOR(BulletManager::GetInstance()->get_nb_bul()/2000.f, c);
    //c = {  0, 255,   0, 255}; __MONITOR(1.f-AnmManager::getFreeAnm()/8191.f, c);
    //c = {  0,   0, 255, 255}; __MONITOR(EnemyManager::GetInstance()->enemyCount/300.f, c);

    if (EclFileManager::GetInstance()->stdf) EclFileManager::GetInstance()->stdf->Draw();
    UPDATE_FUNC_REGISTRY->run_all_on_draw();
    AnmManager::draw();
}

void App::on_destroy() {
    EnemyManager::Cleanup();
    delete BulletManager::GetInstance();
    delete Spellcard::GetInstance();
    AnmManager::Cleanup();
    delete PLAYER_PTR;
}
