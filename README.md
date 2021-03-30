# Graphics Assignment One

## Game Mechanics

This maze game is based on the universe of Halo (the game). You are Master Chief in the
maze and an Elite (enemy) chases you. You have to acquire the Red Powerup to release
bombs/coins in the maze. Reaching the shielded cross with the skull vaporizes the elite.

Once you reach both the Red Powerup as well as the shielded skull, both tasks are
complete and the Pelican lands on the exit node. Reaching the pelican wins the game.

At all times, you can see your own room coordinates as well as the enemies coordinates
on the top right of the screen. Once the exit is open, you can see it's coordinates 
there.

You die if you contact an Elite or your health goes to zero.
If you run out of time or you die, the game is lost.

## Running

`make && ./main`

Requires freetype, glad, glfw3 and glm to be in the include path. Best way to do that is install them into your `/usr/local/include`.

## Maze generation

Maze is first generated following the standard iterative procedure given on [Wikipedia](https://en.wikipedia.org/wiki/Maze_generation_algorithm#Iterative_implementation).

However, after that, I delete some walls in order to create circularity in the maze.
