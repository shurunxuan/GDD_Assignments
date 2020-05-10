This project uses the code base from IGME750 Game Engine. It's a modified DX11Starter, and with other engine stuff that helps porting the game.

It uses vcpkg as its package manager, and before the first compilation it will try to download and compile all 3rd party libraries that is used in this project. This procedure will take 1.5 to 2 hours. It will complete in a flash after the first compilation. You can also copy the whole vcpkg folder from the last compilation to skip this step.

The assets are in DSEngine/Tower folder. 

Control:

Press Space to draw a task
Press 1/2/3 to choose a brick
Press 1/2/3 again to place a brick
Press ESC after a brick is chosen to cancel
Hold mouse right button and move mouse to rotate camera
W/A/S/D to move camera