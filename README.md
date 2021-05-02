# COMPUTER GRAPHICS - ASSIGNMENT 1

## By - Gokul Vamsi Thota
## Roll number - 2019111009


### Brief Description

This is the first assignment, which involves creation of a maze game. I have titled it "Pacmen among us" as it is among us characters in a pacman kind of setup.



### Execution

It is assumed that all the required libraries (glfw, glad, glm, freetype) are pre-installed and present in a 'libraries' folder. The 'source' folder included here can be executed in the following way (with the help of CMakeLists.txt included).

* After extracting the folder into some location on your system, enter the directory 2019111009, with the command: `cd 2019111009`

* Move the 'libraries' folder (with installations of glfw, glad, glm, freetype) to this directory.

* In this directory create an empty folder 'build' with the command: `mkdir build`

* Navigate to this folder with the command: `cd build`

* Run the command: `cmake ..` , and after that run the command: `make` for finishing compiling and generating executable.

* Run the generated executable with the command: `./Pacmen_among_us`. You will be able to see the display now.



### Instructions

The following instructions are to be kept in mind while playing the game



#### Rules to remember

* The player is the green among us character (you) and the impostor is the red among us character.

* Impostor uses shortest path to track the position of the player at every instant and moves accordingly.

* The impostor is vaporized when player reaches the blue button (circle) which is in some random cell in the maze. Health of player won't change on reaching this button.

* 5 coins (powerups) and 5 bombs (obstacles) are spawned when the player collects the orange button (circle), which is in some random cell in the maze. Health of player won't change on reaching this button.

* Initial health/score of player is 10 units. He gains 10 units of health on collecting a coin and loses 10 units on enciuntering a bomb.

* If health of a player drops below 0 at some point, it is considered that he lost the game.

* Player also loses the game if impostor reaches him before he reaches the 'vaporize impostor' button, or if he takes more than 250 seconds to finish the game successfully.

* Player is allowed to exit the game only when he finishes the 2 tasks (collecting all coins, vaporizing impostor).

* Rest of the instructions are followed as mentioned in the assignment pdf.



#### Moving the player

* Press 'UP' arrow key to move the player upwards.

* Press 'DOWN' arrow key to move the player downwards.

* Press 'LEFT' arrow key to move the player to the left.

* Press 'RIGHT' arrow key to move the player to the right.

* It is handled such that player can move only if a wall doesn't obstruct him. Hence, the player must be cautious in movement such that he doesn't get stuck due to walls.



#### Points to note

* All the features mentioned in the assignment pdf have been implemented (except the lighting feature, which is why HUD always shows Light is on).

* All the 3 bonus features have been implemented (shortest path algorithm - Floyd Warshall, is used so that impostor moves towards player via shortest path at every instant; powerups look like coins and obstacles look like bombs; There is a 'Game Over' Screen, which shows 'You won' if player won and 'You lost' if player loses).