# a2-4300

**dependencies:**
- SFML 2.6.1
- DearImGui 1.90
- ImGui-SFML 2.6

DEMO


https://github.com/kin789246/a2-4300-cpp/assets/30062348/058c1a8b-4b6a-483d-b1bb-4482929a3d77

[![Watch the video](https://img.youtube.com/vi/19MgmLUtSik/hqdefault.jpg)](https://www.youtube.com/embed/19MgmLUtSik)

This game must have the following features:<br>
**Player:**
- The player is represented by a shape which is defined in the config file
- The player must spawn in the center of the screen at the begining of the game,
  and after it dies (collides with an enemy)
- The player moves by a speed read from the config file in these directions:
  Up: W key, Left: A, Down S key, Right: D key
- The player is confined to move only within the bounds of the window
- The player will shoot a bullet toward the mouse pointer when the left mouse
  button is clicked. The speed, size and lifespan of the bullets are read from
  the config file.

**Special Ability**
- You are free to come up with your own 'special move' which is fired by the
  player when the right mouse button is clicked. This special ability must:
    - Multiple entities (bullets etc) spawned by special weapon
    - Entities have some unique graphic associate with them
    - A unique game mechanic is introduced via a new component
    - A 'cooldown timer' must be implemented for the special weapon
  The properties of the special move are not in the config file

**Enemy(s):**
- Enemies will spawn in a random location on the screen every X frames,
  where X is defined in the configuration file.
- Enemies must not overlap the sides of the screen at the time of spawn.
- Enemies shapes have random number of vertices, between a givin minimum and
  maximum number, which is specified in the config file.
- Enemy shape radius will be specified int the configuration file.
- Enemies will be givin a random color upon spawning.
- Enemies will be givin a random speed upon spawning, between a minimum and
  maximum value specified in the config file.
- When an enemy reches the edge of the window, it should bounce off in the opposite
  direction at the same speed.
- When (large) enemies collide with a bullet or player, they are destroyed,
  and N small enemies spawn in its place, where N is the number of vertices of
  the original enemy. Each small enemy must have the same number of vertices and
  color of the origin enemy. These small entities travel outward at angles at a
  fixed intervals equal to (360 / verticies).
  For example, if the original enemy has 6 sides, the 6 smaller enemies will 
  travel outward in intervals of (360 / 6) = 60 degrees. The smaller enemies 
  must have a radius equal to half of the original entity.

**Score:**
- Each time an enemy spawns, it is given a score component of N * 100, where N 
  is the number of vertices it has. Small enemies get double this value.
- If a player bullet kills an enemy, the game score is increased by the score
  component of the enemy killed.
- The score should be displayed with the font specified by the config file in 
  the top-left corner of the screen.

**Drawing:**
- In the render system, all entities should be given a slow rotation, which
  makes the game look a little nicer.
- Any special effects which do not alter game play can be added for up to 5%
  bounds marks on the assigment. Note that assigments cannot go above 100% total
  marks, but the 5% bonus can overwrite any marks lost in other areas of the
  assignment.
- Any Entity with a lifespan is currently alive, it should have its Color alpha
  channel set to a ratio depending on how long it has left to live.
  For example, if an Entity has a 80 frame life span, and 25 frames remaining,
  its alpha value should be set to (float)25/80 * 255, the alpha should go from
  255 when it is first spawned, to 0 on the last frame it is alive.

**GUI:**
- You must construct a GUI using ImGui which has the following functionality:
  - The GUI must display options to turn off each system independently
  - For each entity in the game, the GUI must list the ID, tag, and positon of
    that entity. You must display a list of all entities, as well as lists
    of entities by their tag. You must also have some way of destroying a given
    entity by interacting with the UI element associated with it.
  - You must be able to change the enemy spawn interval through the GUI
  - You must be able to manually spawn enemies in some way through the GUI
  - You may develop the GUI in any way that contains this functionality, but
    it must be esily usable and clearly presented to get full marks.

**Misc:**
- The 'P' key should pause the game
- The 'ESC' key should close the game

**Configuration File:**
- Window W H FL FS
  - W window width int
  - H window height int
  - FL frame limit
  - FS full screen mode = 1 | window mode = 0
- Font F S R G B
  - F font file std::string (no spaces)
  - S size int
  - Colr (R, G, B) int, int, int
- Player SR CR S FR FG FB OR OG OB OT V
  - SR shape radius int
  - CR collision radius int
  - S speed float
  - FR, FG, FB fill color int, int, int
  - OR, OG, OB outline color int, int, int
  - OT outline thickness int
  - V shape vertices int
- Enemy SR CR SMIN SMAX OR OG OB OT VMIN VMAX L SI
  - SR shape radius int
  - CR collision radius int
  - SMIN, SMAX min / max speed float, float
  - OR, OG, OB outline color int, int, int
  - OT outline thickness int
  - VMIN, VMAX min /max vertices int, int
  - L small lifespan int
  - SP spawn interval int
- Bullet SR CR S FR FG FB OR OG OB OT V L
  - SR shape radius int
  - CR collision radius int
  - S speed float
  - FR, FG, FB fill color int, int, int
  - OR, OG, OB outline color int, int, int
  - OT outline thickness int
  - V shape vertices int
  - L lifespan int
