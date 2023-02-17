# ConnectFourRobot
This repo contains all version controls for our group's LEGO EV3 controlled connect four robot.

All code in this repo is written in C with added functions for interfacing motors and sensors with the EV3 Brick.

The Main algorithm works by simulating moves within the board stored in memory and determining if the simulated move generates a win for either player.
This algorithm uses iterative for-loops to simulate a move in every column alternating between the robot and player, A score is than summed for the leading column that leads to a win.
Higher levels of the simulation have heavier weighting as they are more likely to occur.
Wins for the robot count as a positive score and wins for the player are weighted as a larger negative value (favours blocking).

All Contributors:
Hayden Kebbel
Luke Taylor
Magnus Scott
Mathew Hanson
