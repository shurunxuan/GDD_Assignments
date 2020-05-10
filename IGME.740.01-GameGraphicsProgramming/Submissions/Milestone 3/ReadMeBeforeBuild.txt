This project uses the code base from IGME750 Game Engine. 
It's a modified DX11Starter, and with other engine stuff 
that helps porting the game.

It uses vcpkg as its package manager, and before the first 
compilation it will try to download and compile all 3rd 
party libraries that is used in this project. This procedure 
will take around 30 minutes. It will complete in a flash after 
the first compilation. You can also copy the whole vcpkg 
folder from the last compilation to skip this step. We deleted
some unnecessary parts of the engine to get a faster build.

The src.zip includes the source code.

The bin.zip includes the x64 release binaries.

The Assets.zip includes the assets that we used in the project. 
Put the unzipped Assets/ folder under Tower/Tower where the 
Tower.vcxproj file is located, and the unzipped bin/ folder 
where the Tower.exe file is located.

Please run the application in Release mode.

Control:

Game:
Press Space to draw a task
Press 1/2/3 to choose a brick
Press 1/2/3 again to place a brick
Press ESC after a brick is chosen to cancel

Graphics:
Hold mouse right button and move mouse to rotate camera
Hold W/A/S/D to move camera
Click mouse left button to change skybox
Press Z to toggle normal mapping
Press X to turn off bloom & SSAO/SSIL
Press C to turn on only bloom
Press V to turn on bloom & SSAO/SSIL
Press B to toggle between SSAO w/o IL
Press N to toggle screen dark corner
Press M to toggle refraction on yellow bricks
Hold Up/Down to adjust metalness
Hold Left/Right to adjust roughness