#include "Game.h"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/WindowStyle.hpp"
#include "imgui-SFML.h"
#include "imgui.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <cmath>
#include <numbers>
#include <iostream>
#include <random>

Game::Game(const std::string& config) {
    init(config);
}

void Game::init(const std::string& path) {
    // read in config file here
    // use the premade PlayerConfig, EnemyConfig, BulletConfig variables
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Could not load config.txt file!\n";
        exit(-1);
    }
    std::string head;
    int window_width, window_height;
    int frameLimit, fullscreen;
    while (file >> head) {
        if (head == "Window") {
            file 
                >> window_width 
                >> window_height
                >> frameLimit
                >> fullscreen;
        }
        else if (head == "Font") {
            int r, g, b;
            std::string font_file;
            int font_size;
            file >> font_file >> font_size >> r >> g >> b;
            if (!m_font.loadFromFile(font_file)) {
                std::cerr << "Could not load font!\n";
                exit(-1);
            }
            m_text.setFillColor(sf::Color(r, g, b));
            m_text = sf::Text("SCORE:", m_font, font_size);
            m_text.setPosition(10, 10);
            m_showGuiMessage = 
                sf::Text("Press G key to show setting Window", m_font, 15);
            m_showGuiMessage.setPosition(10, window_height - 50);
            m_showGuiMessage.setFillColor(sf::Color::White);
        }
        else if (head == "Player") {
            file >> m_playerConfig.SR
                >> m_playerConfig.CR
                >> m_playerConfig.S
                >> m_playerConfig.FR
                >> m_playerConfig.FG
                >> m_playerConfig.FB
                >> m_playerConfig.OR
                >> m_playerConfig.OG
                >> m_playerConfig.OB
                >> m_playerConfig.OT
                >> m_playerConfig.V;
        }
        else if (head == "Enemy") {
            file >> m_enemyConfig.SR
                >> m_enemyConfig.CR
                >> m_enemyConfig.SMIN
                >> m_enemyConfig.SMAX
                >> m_enemyConfig.OR
                >> m_enemyConfig.OG
                >> m_enemyConfig.OB
                >> m_enemyConfig.OT
                >> m_enemyConfig.VMIN
                >> m_enemyConfig.VMAX
                >> m_enemyConfig.L
                >> m_enemyConfig.SI;
        }
        else if (head == "Bullet") {
            file >> m_bulletConfig.SR
                >> m_bulletConfig.CR
                >> m_bulletConfig.S
                >> m_bulletConfig.FR
                >> m_bulletConfig.FG
                >> m_bulletConfig.FB
                >> m_bulletConfig.OR
                >> m_bulletConfig.OG
                >> m_bulletConfig.OB
                >> m_bulletConfig.OT
                >> m_bulletConfig.V
                >> m_bulletConfig.L;
        }
        else {
            std::cerr << "head to " << head << "\n";
            std::cerr << "The config file format is incorrect!\n";
            exit(-1);
        }
    }
    // set up default window parameters
    auto style = sf::Style::Default;
    if (fullscreen) {
        style = sf::Style::Fullscreen;
    }
    m_window.create(
        sf::VideoMode(window_width, window_height), 
        "Geometry Wars",
        style
    );
    m_window.setFramerateLimit(frameLimit);

    ImGui::SFML::Init(m_window);
    spawnPlayer();
}

void Game::run() {
    // add pause functionality in here
    // some systems should function while paused (rendering)
    // some systems shouldn't (movement / input)
    while (m_running) {
        // update the entity manager
        m_entities.update();

        // required update call to ImGui
        ImGui::SFML::Update(m_window, m_deltaClock.restart());

        if (!m_paused) {
            sMovement();
            if (m_lifespan) {
                sLifespan();
            }
            // increment the current frame
            // may need to be moved when pause implemented
            m_currentFrame++;
        }

        sUserInput();
        
        if (m_spawning) {
            sEnemySpawner();
        }

        if (m_collision) {
            sCollision();
        }

        if (m_gui) {
            sGUI();
        }

        sRender();
    }
}

void Game::setPaused(bool paused) {
    m_paused = paused;
}

// respawn the player in the middle of the screen
void Game::spawnPlayer() {
    // Finish adding all properties of the player with the correct values
    // from the config
    
    // We ccreate every entity by calling EntityManager.addEntity(tag)
    // This returns a std::shared_ptr<Entity>, so we use 'auto' to save typing
    auto entity = m_entities.addEntity("player");

    Vec2 position = Vec2(
        m_window.getSize().x / 2.0, m_window.getSize().y / 2.0
    );
    Vec2 velocity = Vec2(m_playerConfig.S, m_playerConfig.S);
    entity->cTransform = std::make_shared<CTransform>(
        position, velocity, 0
    );
    
    entity->cShape = std::make_shared<CShape>(
        m_playerConfig.SR,
        m_playerConfig.V,
        sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
        sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB),
        m_playerConfig.OT
    );

    // Add an input component to the player so that we can use inputs
    entity->cInput = std::make_shared<CInput>();
    entity->cCollission = std::make_shared<CCollision>(m_playerConfig.CR);

    // Since we want this Entity to be our player, set our Game's player variable
    // to be this entity
    // This goes slightly against the EntityManager paradigm, but we use the player
    // so much it's worth it
    m_player = entity;
}

// spawn an enemy at a random position
void Game::spawnEnemy() {
    // make sure the enemy is spawned properly with the m_enemyConfig variables
    //       the enemy must be spawned completely within the bounds of the window
    //       srand(time(null))
    //       random r = min + (rand() % (1 + max - min))
    auto enemy = m_entities.addEntity("enemy");
    Vec2 position = Vec2(
        mersenne(m_enemyConfig.SR, m_window.getSize().x - m_enemyConfig.SR),
        mersenne(m_enemyConfig.SR, m_window.getSize().y - m_enemyConfig.SR)
    );
    // std::cout << "pos x=" << position.x << " y=" << position.y << '\n';
    Vec2 velocity = Vec2(
        mersenne(m_enemyConfig.SMIN, m_enemyConfig.SMAX),
        mersenne(m_enemyConfig.SMIN, m_enemyConfig.SMAX)
    );
    sf::Color color(
        mersenne(0, 255), mersenne(0, 255), mersenne(0, 255)
    );
    sf::Color outlineColor(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB);
    enemy->cTransform = std::make_shared<CTransform>(
        position, velocity, 0
    );
    int vertex = mersenne(m_enemyConfig.VMIN, m_enemyConfig.VMAX);
    enemy->cShape = std::make_shared<CShape>(
        m_enemyConfig.SR, vertex, color, outlineColor, m_enemyConfig.OT 
    );
    enemy->cCollission = std::make_shared<CCollision>(m_enemyConfig.CR);
    enemy->cScore = std::make_shared<CScore>(vertex * 100);
    // record when the most recent enemy was spawned
    m_lastEnemySpawnTime = m_currentFrame;
}

// spawns the small enemies when a big one (input entity e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> e) {
    // spawn small enemies at the location of the input enemy e
    // when we create the smaller enemy, we have to read the values of the original enemy
    // - spawn a number of small enemies equal to the vertices of the original enemy
    // - set each small enemy to the same color as the original, half the size
    // - small enemies are worth double points of the original enemy
    Vec2 speed = e->cTransform->velocity;
    int n = e->cShape->circle.getPointCount();
    sf::Vector2f center = e->cShape->circle.getOrigin();
    for (int i=0; i<n; i++) {
        sf::Vector2f vertex = e->cShape->circle.getPoint(i);
        float theta = std::atan2(vertex.y - center.y, vertex.x - center.x);
        Vec2 velocity = Vec2(
            speed.x * std::cos(theta), speed.y * std::sin(theta)
        );
        auto small = m_entities.addEntity("small");
        small->cTransform = std::make_shared<CTransform>(
            e->cTransform->pos, velocity, 0
        );
        small->cShape = std::make_shared<CShape>(
            e->cShape->circle.getRadius() / 2,
            n,
            e->cShape->circle.getFillColor(),
            e->cShape->circle.getOutlineColor(),
            e->cShape->circle.getOutlineThickness()
        );
        small->cCollission = std::make_shared<CCollision>(m_enemyConfig.CR);
        small->cScore = std::make_shared<CScore>(e->cScore->score * 2);
        small->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
    }
}

// spawns a bullet from a given entity to a target 
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target) {
    // implement the spawning of a bullet which travels toward target
    // - bullet speed is given as a scalar speed
    // - you must set the velocity by using formula in notes
    // given speed S, angle θ 
    // get pos x = S * cos(θ), y = S * sin(θ)
    // angle θ = atan2f(dy, dx)
    // or normalize distence vector mulply speed
    auto bullet = m_entities.addEntity("bullet");
    Vec2 velocity = (target - entity->cTransform->pos) /
        entity->cTransform->pos.dist(target) *
        10;
    bullet->cTransform = std::make_shared<CTransform>(
        entity->cTransform->pos, velocity, 0
    );
    bullet->cShape = std::make_shared<CShape>(
        10, 10, sf::Color::White, sf::Color::White, 0
    );
    bullet->cCollission = std::make_shared<CCollision>(m_bulletConfig.CR);
    bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity) {
    // todo: implement your own special weapon
}

void Game::sMovement() {
    // implement all entity movement in this function
    // you should read the m_player->cInput component to determine if the
    // player is moving
    // caculate the speed when diagonal moving e.g UP and LEFT pressed
    // at the same time
    m_player->cTransform->velocity = Vec2{ 0, 0 };
    // 8 directions
    // up
    if (m_player->cInput->up && !m_player->cInput->down
        && !m_player->cInput->left && !m_player->cInput->right
        ) {
        m_player->cTransform->velocity.y = -m_playerConfig.S;
    }
    // down
    else if (!m_player->cInput->up && m_player->cInput->down
        && !m_player->cInput->left && !m_player->cInput->right
    ) {
        m_player->cTransform->velocity.y = m_playerConfig.S;
    }
    // left
    else if (m_player->cInput->left && !m_player->cInput->right
        && !m_player->cInput->up && !m_player->cInput->down
    ) {
        m_player->cTransform->velocity.x = -m_playerConfig.S;
    }
    // right
    else if (!m_player->cInput->left && m_player->cInput->right
        && !m_player->cInput->up && !m_player->cInput->down
    ) {
        m_player->cTransform->velocity.x = m_playerConfig.S;
    }
    // up right
    else if (!m_player->cInput->left && m_player->cInput->right
        && m_player->cInput->up && !m_player->cInput->down
    ) {
        m_player->cTransform->velocity = Vec2(
            m_playerConfig.S * std::cosf(std::numbers::pi / 4),
            -m_playerConfig.S * std::sinf(std::numbers::pi / 4)
        );
    }
    // up left
    else if (m_player->cInput->left && !m_player->cInput->right
        && m_player->cInput->up && !m_player->cInput->down
    ) {
        m_player->cTransform->velocity = Vec2(
            -m_playerConfig.S * std::cosf(std::numbers::pi / 4),
            -m_playerConfig.S * std::sinf(std::numbers::pi / 4)
        );
    }
    // down right
    else if (!m_player->cInput->left && m_player->cInput->right
        && !m_player->cInput->up && m_player->cInput->down
    ) {
        m_player->cTransform->velocity = Vec2(
            m_playerConfig.S * std::cosf(std::numbers::pi / 4),
            m_playerConfig.S * std::sinf(std::numbers::pi / 4)
        );
    }
    // down left
    else if (m_player->cInput->left && !m_player->cInput->right
        && !m_player->cInput->up && m_player->cInput->down
    ) {
        m_player->cTransform->velocity = Vec2(
            -m_playerConfig.S * std::cosf(std::numbers::pi / 4),
            m_playerConfig.S * std::sinf(std::numbers::pi / 4)
        );
    }

    for (auto e : m_entities.getEntities()) {
        // Sample movement speed update
        e->cTransform->pos.x += e->cTransform->velocity.x;
        e->cTransform->pos.y += e->cTransform->velocity.y;
    }

    // if player moves to wall stop it
    if(0 > m_player->cTransform->pos.x - m_player->cCollission->radius ||
        m_window.getSize().x < 
        m_player->cTransform->pos.x + m_player->cCollission->radius
    ) {
        m_player->cTransform->pos.x -= m_player->cTransform->velocity.x;
    }
    
    if(0 > m_player->cTransform->pos.y - m_player->cCollission->radius ||
        m_window.getSize().y < 
        m_player->cTransform->pos.y + m_player->cCollission->radius
    ) {
        m_player->cTransform->pos.y -= m_player->cTransform->velocity.y;
    }
}

void Game::sLifespan() {
    // implement all lifespan functionality
    // for all entities
    //     if entity has no lifespan component, skip it
    //     if entity has > 0 remaining lifespan, subtract 1
    //     if it has lifespan and is_alive, scale its alpha channel properly
    //     if it has lifespan and its time is up, destroy the entity
    for (auto e : m_entities.getEntities()) {
        if (e->cLifespan) {
            if (e->cLifespan->remaining > 0) {
                if (e->cShape) {
                    // set alpha
                    int fr = e->cShape->circle.getFillColor().r;
                    int fg = e->cShape->circle.getFillColor().g;
                    int fb = e->cShape->circle.getFillColor().b;
                    int outr = e->cShape->circle.getOutlineColor().r;
                    int outg = e->cShape->circle.getOutlineColor().g;
                    int outb = e->cShape->circle.getOutlineColor().b;
                    int alpha = (float)e->cLifespan->remaining / 
                        (float)e->cLifespan->total * 255.0;
                    e->cShape->circle.setFillColor(sf::Color(fr, fg, fb, alpha));
                    e->cShape->circle.
                        setOutlineColor(sf::Color(outr, outg, outb, alpha));
                }

                if (e->cText) {
                    // set alpha
                    int fr = e->cText->text.getFillColor().r;
                    int fg = e->cText->text.getFillColor().g;
                    int fb = e->cText->text.getFillColor().b;
                    int outr = e->cText->text.getOutlineColor().r;
                    int outg = e->cText->text.getOutlineColor().g;
                    int outb = e->cText->text.getOutlineColor().b;
                    int alpha = (float)e->cLifespan->remaining / 
                        (float)e->cLifespan->total * 255.0;
                    e->cText->text.setFillColor(sf::Color(fr, fg, fb, alpha));
                    e->cText->text.
                        setOutlineColor(sf::Color(outr, outg, outb, alpha));
                }

                e->cLifespan->remaining--;
            }
            else {
                e->destroy();
            }
        }
    }
}

void Game::sCollision() {
    // implement all proper collisions between enemies
    // be sure to use the collision radius, NOT the shape radius
    // sqrt(dx*dx + dy*dy) < r1 + r2
    // collision = DistSq < (r1+r2)*(r1+r2)
    // m_collision will not be effected by wall colliding
    
    // enemies and player
    for (auto enemy : m_entities.getEntities("enemy")) {
        if (
            circleCollision(
                m_player->cTransform->pos,
                m_player->cCollission->radius,
                enemy->cTransform->pos,
                enemy->cCollission->radius
            )
        ) {
            spawnSmallEnemies(enemy);
            m_player->destroy();
            enemy->destroy();
            spawnPlayer();
        }
    }

    for (auto small : m_entities.getEntities("small")) {
        if (
            circleCollision(
                m_player->cTransform->pos,
                m_player->cCollission->radius,
                small->cTransform->pos,
                small->cCollission->radius
            )
        ) {
            m_player->destroy();
            small->destroy();
            spawnPlayer();
        }
    }
    // bullet and enemies
    for (auto bullet : m_entities.getEntities("bullet")) {
        for (auto enemy : m_entities.getEntities("enemy")) {
            if (
                circleCollision(
                    bullet->cTransform->pos,
                    bullet->cCollission->radius,
                    enemy->cTransform->pos,
                    enemy->cCollission->radius
                )
            ) {
                m_score += enemy->cScore->score;
                // std::cout << "+" << enemy->cScore->score << "!\n";
                spawnSmallEnemies(enemy);
                spawnAddScore(enemy->cScore->score, enemy->cTransform->pos);
                enemy->destroy();
                bullet->destroy();
            }
        }
        for (auto enemy : m_entities.getEntities("small")) {
            if (
                circleCollision(
                    bullet->cTransform->pos,
                    bullet->cCollission->radius,
                    enemy->cTransform->pos,
                    enemy->cCollission->radius
                )
            ) {
                m_score += enemy->cScore->score;
                // std::cout << "+" << enemy->cScore->score << "!\n";
                spawnAddScore(enemy->cScore->score, enemy->cTransform->pos);
                enemy->destroy();
                bullet->destroy();
            }
        }
    }
    // big enemies and wall
    for (auto enemy : m_entities.getEntities("enemy")) {
        if (
            0 > enemy->cTransform->pos.x - enemy->cCollission->radius ||
            enemy->cTransform->pos.x + enemy->cCollission->radius >
            m_window.getSize().x) {
            enemy->cTransform->velocity.x *= -1;
        }

        if (
            0 > enemy->cTransform->pos.y - enemy->cCollission->radius ||
            enemy->cTransform->pos.y + enemy->cCollission->radius >
            m_window.getSize().y
        ) {
            enemy->cTransform->velocity.y *= -1;
        }
    }
}

void Game::sEnemySpawner() {
    // code which implements enemy spawning should go here
    //
    // use m_currentFrame - m_lastEnemySpawnTime to determine hown long it 
    // has been since the last enemy spawned
    if (m_spawnInterval < m_currentFrame - m_lastEnemySpawnTime) {
        spawnEnemy();
    }
}

void Game::sGUI() {
    ImGui::Begin("Geometry Wars");
    // ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("configTabBar"))
    {
        if (ImGui::BeginTabItem("Systems"))
        {
            ImGui::Checkbox("Paused", &m_paused);
            ImGui::Checkbox("Lifespan", &m_lifespan);
            ImGui::Checkbox("Collision", &m_collision);
            ImGui::Checkbox("Spawning", &m_spawning);
            ImGui::SliderInt("Spawn Interval", &m_spawnInterval, 1, 120);
            if (ImGui::Button("Manual Spawn")) {
                spawnEnemy();
            }
            ImGui::Checkbox("GUI", &m_gui);
            ImGui::Checkbox("Rendering", &m_rendering);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Entity Manager"))
        {
            static ImGuiTreeNodeFlags tflags = ImGuiTreeNodeFlags_Framed;
            if (ImGui::TreeNodeEx("Entities by Tag", tflags))
            {
                for (auto& [tag, entityVec] : m_entities.getEntityMap()) {
                    if (ImGui::TreeNodeEx(tag.c_str(), tflags)) {
                        for (auto e: entityVec) {
                            std::string btnLabel = "D##" + std::to_string(e->id());
                            if (ImGui::SmallButton(btnLabel.c_str())) {
                                e->destroy();
                                if (e->tag() == "player") {
                                    spawnPlayer();
                                }
                            }
                            ImGui::SameLine();
                            ImGui::Text("%s", std::to_string(e->id()).c_str());
                            ImGui::SameLine();
                            ImGui::Text("%s", e->tag().c_str());
                            ImGui::SameLine();
                            ImGui::Text("(%.f, %.f)", 
                                    e->cTransform->pos.x, e->cTransform->pos.y);
                        }

                        ImGui::TreePop();
                    }
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNodeEx("All Entities", tflags)) {
                for (auto e : m_entities.getEntities()) {
                    std::string btnLabel = "D##" + std::to_string(e->id());
                    if (ImGui::SmallButton(btnLabel.c_str())) {
                        e->destroy();
                        if (e->tag() == "player") {
                            spawnPlayer();
                        }
                    }
                    ImGui::SameLine();
                    ImGui::Text("%s", std::to_string(e->id()).c_str());
                    ImGui::SameLine();
                    ImGui::Text("%s", e->tag().c_str());
                    ImGui::SameLine();
                    ImGui::Text("(%.f, %.f)", 
                        e->cTransform->pos.x, e->cTransform->pos.y);
                }

                ImGui::TreePop();
            }

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

void Game::sRender() {
    // change the code below to draw ALL of the entities
    // sample drawing of the player entity that we have created
    m_window.clear();
    for (auto e : m_entities.getEntities()) {
        if (e->cShape) {
            // set the position of the shape based on the entity's transform->pos
            e->cShape->circle.setPosition(
                e->cTransform->pos.x, e->cTransform->pos.y
            );

            // set the rotation of the shape based on the entity's transform->angle
            e->cTransform->angle += 1.0f;
            e->cShape->circle.setRotation(e->cTransform->angle);

            // draw the entity's sf::CircleShape
            if (m_rendering) {
                m_window.draw(e->cShape->circle);
            }
        }

        if (e->cText) {
            e->cText->text.setPosition(
                e->cTransform->pos.x, e->cTransform->pos.y
            );

            if (m_rendering) {
                m_window.draw(e->cText->text);
            }
        }
    }

    // draw score board
    m_text.setString("SCORE: " + std::to_string(m_score));
    m_window.draw(m_text);

    // draw show gui hint
    if (!m_gui) {
        m_window.draw(m_showGuiMessage);
    }

    // draw the ui last
    ImGui::SFML::Render(m_window);

    m_window.display();
}

void Game::sUserInput() {
    // handle user input here
    // note that you should only be setting the player's input component
    // variables here, you should not implement the player's movemnet
    // logic here, the movement system will read the variables you set in 
    // this function
    
    sf::Event event;
    while (m_window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(m_window, event);

        if (event.type == sf::Event::Closed) {
            m_running = false;
        }
        
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::W:
                    if (!m_paused) {
                        // std::cout << "W key pressed\n";
                        // set player's input component "up" to true
                        m_player->cInput->up = true;
                    }
                    break;
                case sf::Keyboard::S:
                    if (!m_paused) {
                        // std::cout << "S key pressed\n";
                        // set player's input component "down" to true
                        m_player->cInput->down = true;
                    }
                    break;
                case sf::Keyboard::A:
                    if (!m_paused) {
                        // std::cout << "A key pressed\n";
                        // set player's input component "left" to true
                        m_player->cInput->left = true;
                    }
                    break;
                case sf::Keyboard::D:
                    if (!m_paused) {
                        // std::cout << "D key pressed\n";
                        // set player's input component "right" to true
                        m_player->cInput->right = true;
                    }
                    break;
                default:
                    break;
            }
        }

        if (event.type == sf::Event::KeyReleased) {
            switch (event.key.code) {
                case sf::Keyboard::W:
                    if (!m_paused) {
                        // std::cout << "W key released\n";
                        // set player's input component "up" to false
                        m_player->cInput->up = false;
                    }
                    break;
                case sf::Keyboard::S:
                    if (!m_paused) {
                        // std::cout << "S key released\n";
                        // set player's input component "down" to false
                        m_player->cInput->down = false;
                    }
                    break;
                case sf::Keyboard::A:
                    if (!m_paused) {
                        // std::cout << "A key released\n";
                        // set player's input component "left" to false
                        m_player->cInput->left = false;
                    }
                    break;
                case sf::Keyboard::D:
                    if (!m_paused) {
                        // std::cout << "D key released\n";
                        // set player's input component "right" to false
                        m_player->cInput->right = false;
                    }
                    break;
                case sf::Keyboard::P:
                    if (m_paused) {
                        setPaused(false);
                    }
                    else {
                        setPaused(true);
                    }
                    break;
                case sf::Keyboard::Escape:
                    m_running = false;
                    m_window.close();
                    break;
                case sf::Keyboard::G:
                    m_gui = !m_gui;
                    break;
                default:
                    break;
            }
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            // this line ignores mouse events if ImGui is the thing clicked
            if (ImGui::GetIO().WantCaptureMouse) { continue; }

            if (event.mouseButton.button == sf::Mouse::Left) {
                // std::cout << "Left Mouse Button Clicked at (" 
                //     << event.mouseButton.x << ","
                //     << event.mouseButton.y << ")\n";
                // call spawnBullet here
                spawnBullet(
                    m_player, 
                    Vec2(event.mouseButton.x, 
                    event.mouseButton.y)
                );
            }

            if (event.mouseButton.button == sf::Mouse::Right) {
                // std::cout << "Right Mouse Button Clicked at (" 
                //     << event.mouseButton.x << ","
                //     << event.mouseButton.y << ")\n";
                // call spawnSpecialWeapon here
            }
        }
    }
}

int Game::mersenne(int min, int max) {
    std::random_device rd;  // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(min, max);

    return distrib(gen);
}


bool Game::circleCollision(Vec2 c1, float r1, Vec2 c2, float r2) {
    float dx = c1.x - c2.x;
    float dy = c1.y - c2.y;
    return (dx * dx + dy * dy < (r1 + r2) * (r1 + r2));
}

void Game::spawnAddScore(int score, Vec2 position) {
    auto s = m_entities.addEntity("addScore");
    Vec2 velocity = Vec2(0, -1);
    s->cTransform = std::make_shared<CTransform>(position, velocity, 0);
    s->cText = std::make_shared<CText>("+" + std::to_string(score), m_font, 20);
    s->cText->text.setFillColor(sf::Color::White);
    s->cLifespan = std::make_shared<CLifespan>(30);
}
