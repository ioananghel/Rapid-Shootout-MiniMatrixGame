# Rapid-Shootout-MiniMatrixGame
<div align="center"><img src="rapid_shootout_logo_dalle.png" width="400" height="400"></div>

<details>
<summary>
Task Requirements
</summary>
This game was developed as part of the homeworks for the course "Introduction to Robotics" at the University of Bucharest.
The task for this homework was to develop a game that uses an 8x8 LED matrix and an LCD display. The game should be controlled by a joystick and buttons and should provide the user with a menu and a way of progressing in the game.
</details>

<details>
<summary>
General Presentation
</summary>
The game is a one-player shooter, where the player is spawned randomly in a room and has to shoot walls in order to be able to enter other ones. Walls are generated automatically and each one gives points based on the difficulty chosen by the player. The game ends when the player runs out of lives, destroys all the walls or runs out of time. Bullets come back after not hitting a wall and going out of the room, endangering the lives of the user. The player also has the chance to get streaks of destroyed walls, that double the points. During the last five seconds of the round, the current room gets shut and the points double, giving the chance to the player to shoot uninterruptedly, not fearing losing lives. This also means that entering a full room before the end, but not too soon, gives them an edge.
</details>

<details>
<summary>
How to Play
</summary>
  <details>
    <summarry>
    Menu Navigation
    </summary>
    - Scrolling through the menu is done with the joystick, by moving it up and down.
    - Selecting an option is done by pressing the button or by moving the joystick to the right.
    - Going back to the previous menu is done by moving the joystick to the left.
    - !The play option may not be selected via the joystick, in order to not start the game by mistake.
    </details>

  <details>
    <summarry>
    Game Controls
    </summary>
    - Moving the player is done by moving the joystick in the desired direction.
    - Shooting is done by pressing the button, and the bullet will take the last direction of the player.
    </details>


</details>

<details>
<summary>
Physical Requirements
</summary>
  - Arduino Board
  - Breadboard
  - 8x8 LED Matrix
  - Joystick
  - Button
  - Buzzer
  - 2x 10 kOhm Resistor for pull-down
  - 20 kOhm Resistor for LED Matrix
  - 100 uF Electroytic Capacitor
  - 0.1 uF Ceramic Capacitor
  - Wires
</details>