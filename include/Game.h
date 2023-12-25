#pragma once

#include "Entity.h"
#include "EntityManager.h"

#include<SFML/Graphics.hpp>
#include "imgui-SFML.h"

struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

class Game
{
    sf::RenderWindow m_window; // the window be drawn to
    EntityManager m_entities; // vector of entities to maintain
    sf::Font m_font;
    sf::Text m_text; // the score text
    sf::Text m_showGuiMessage;
    PlayerConfig m_playerConfig;
    EnemyConfig m_enemyConfig;
    BulletConfig m_bulletConfig;
    sf::Clock m_deltaClock;
    sf::Clock m_cdClock;
    int m_score = 0;
    int m_currentFrame = 0;
    int m_lastEnemySpawnTime = 0; 
    int m_spawnInterval = 60;
    bool m_paused = false;
    bool m_running = true;
    bool m_lifespan = true;
    bool m_collision = true;
    bool m_spawning = true;
    bool m_gui = true;
    bool m_rendering = true;
    bool m_specialWeapon = true;

    std::shared_ptr<Entity> m_player;

    void init(const std::string& config);       
    void setPaused(bool paused);

    void sMovement();
    void sUserInput();
    void sLifespan();
    void sRender();
    void sEnemySpawner();
    void sCollision();
    void sGUI();
    void sCoolDownTimer();

    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(std::shared_ptr<Entity> entity);
    void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos);
    void spawnSpecialWeapon(std::shared_ptr<Entity> entity);
    void spawnAddScore(int score, Vec2 position);
    
    // generate random int from mersenne twister engine
    // uniform int distribution
    int mersenne(int min, int max);
    // detect two circle collision
    // parameters: positon and collision radius 
    bool circleCollision(Vec2 c1, float r1, Vec2 c2, float r2);
    public:
        Game(const std::string& config);
        void run();
};
