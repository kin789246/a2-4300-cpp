#include "Game.h"
#include "SFML/Window/Event.hpp"
#include "imgui-SFML.h"
#include "imgui.h"
#include<fstream>
#include<iostream>
#include <memory>
#include <string>

Game::Game(const std::string& config) {
    init(config);
}

void Game::init(const std::string& path) {
    // todo: read in config file here
    //       use the premade PlayerConfig, EnemyConfig, BulletConfig variables
    std::ifstream fin(path);

    // set up default window parameters
    m_window.create(sf::VideoMode(1280, 720), "Assignment 2");
    m_window.setFramerateLimit(60);

    ImGui::SFML::Init(m_window);
    spawnPlayer();
}

void Game::run() {
    // todo: add pause functionality in here
    //       some systems should function while paused (rendering)
    //       some systems shouldn't (movement / input)
    while (m_running) {
        // update the entity manager
        m_entities.update();

        // required update call to ImGui
        ImGui::SFML::Update(m_window, m_deltaClock.restart());

        sEnemySpawner();
        sMovement();
        sCollision();
        sUserInput();
        sGUI();
        sRender();

        // increment the current frame
        // may need to be moved when pause implemented
        m_currentFrame++;
    }
}

void Game::setPaused(bool paused) {
    // todo
}

// respawn the player in the middle of the screen
void Game::spawnPlayer() {
    // todo: Finish adding all properties of the player with the correct values
    //       from the 
    
    // We ccreate every entity by calling EntityManager.addEntity(tag)
    // This returns a std::shared_ptr<Entity>, so we use 'auto' to save typing
    auto entity = m_entities.addEntity("player");

    // Give this entity a Transform so it spawns at (200, 200) with velocity (1, 1)
    // and angle 0
    entity->cTransform = std::make_shared<CTransform>(
        Vec2(200.f, 200.f), Vec2(1.f, 1.f), 0.f
    );
    
    // The entity's shape will have radius 32, 8 sides, dark grey fill,
    // and red outline of thickness 4
    entity->cShape = std::make_shared<CShape>(
        32.f, 8, sf::Color(10, 10, 10), sf::Color::Red, 4.f
    );

    // Add an input component to the player so that we can use inputs
    entity->cInput = std::make_shared<CInput>();

    // Since we want this Entity to be our player, set our Game's player variable
    // to be this entity
    // This goes slightly against the EntityManager paradigm, but we use the player
    // so much it's worth it
    m_player = entity;
}

// spawn an enemy at a random position
void Game::spawnEnemy() {
    // todo: make sure the enemy is spawned properly with the m_enemyConfig variables
    //       the enemy must be spawned completely within the bounds of the window
    
    // record when the most recent enemy was spawned
    m_lastEnemySpawnTime = m_currentFrame;
}

// spawns the small enemies when a big one (input entity e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> e) {
    // todo: spawn small enemies at the location of the input enemy e

    // when we create the smaller enemy, we have to read the values of the original enemy
    // - spawn a number of small enemies equal to the vertices of the original enemy
    // - set each small enemy to the same color as the original, half the size
    // - small enemies are worth double points of the original enemy
}

// spawns a bullet from a given entity to a target 
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target) {
    // todo: implement the spawning of a bullet which travels toward target
    //       - bullet speed is given as a scalar speed
    //       - you must set the velocity by using formula in notes
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity) {
    // todo: implement your own special weapon
}

void Game::sMovement() {
    // todo: implement all entity movement in this function
    //       you should read the m_player->cInput component to determine if the
    //       player is moving
    
    // Sample movement speed update
    m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
    m_player->cTransform->pos.y += m_player->cTransform->velocity.y;
}

void Game::sLifespan() {
    // todo: implement all lifespan functionality
    // for all entities
    //     if entity has no lifespan component, skip it
    //     if entity has > 0 remaining lifespan, subtract 1
    //     if it has lifespan and is_alive, scale its alpha channel properly
    //     if it has lifespan and its time is up, destroy the entity
}

void Game::sCollision() {
    // todo: implement all proper collisions between enemies
    //       be sure to use the collision radius, NOT the shape radius
}

void Game::sEnemySpawner() {
    // todo: code which implements enemy spawning should go here
}

void Game::sGUI() {
    ImGui::Begin("Geometry Wars");
    ImGui::Text("Stuff Goes Here");
    ImGui::End();
}

void Game::sRender() {
    // todo: change the code below to draw ALL of the entities
    //       sample drawing of the player entity that we have created
    m_window.clear();

    // set the position of the shape based on the entity's transform->pos
    m_player->cShape->circle.setPosition(
        m_player->cTransform->pos.x, m_player->cTransform->pos.y
    );

    // set the rotation of the shape based on the entity's transform->angle
    m_player->cTransform->angle += 1.0f;
    m_player->cShape->circle.setRotation(m_player->cTransform->angle);

    // draw the entity's sf::CircleShape
    m_window.draw(m_player->cShape->circle);

    // draw the ui last
    ImGui::SFML::Render(m_window);

    m_window.display();
}

void Game::sUserInput() {
    // todo: handle user input here
    //     note that you should only be setting the player's input component
    //     variables here, you should not implement the player's movemnet
    //     logic here, the movement system will read the variables you set in this function
    
    sf::Event event;
    while (m_window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(m_window, event);

        if (event.type == sf::Event::Closed) {
            m_running = false;
        }

        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::W:
                    std::cout << "W key pressed\n";
                    // todo: set player's input component "up" to true
                    break;
                default:
                    break;
            }
        }

        if (event.type == sf::Event::KeyReleased) {
            switch (event.key.code) {
                case sf::Keyboard::W:
                    std::cout << "W key released\n";
                    // todo: set player's input component "up" to false
                    break;
                default:
                    break;
            }
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            // this line ignores mouse events if ImGui is the thing clicked
            if (ImGui::GetIO().WantCaptureMouse) { continue; }

            if (event.mouseButton.button == sf::Mouse::Left) {
                std::cout << "Left Mouse Button Clicked at (" 
                    << event.mouseButton.x << ","
                    << event.mouseButton.y << ")\n";
                // call spawnBullet here
            }

            if (event.mouseButton.button == sf::Mouse::Right) {
                std::cout << "Right Mouse Button Clicked at (" 
                    << event.mouseButton.x << ","
                    << event.mouseButton.y << ")\n";
                // call spawnSpecialWeapon here
            }
        }
    }
}
