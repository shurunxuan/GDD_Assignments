For building the project from source files:

To build the project from source files, you'll need 
Visual Studio 2017 or Visual Studio 2019 (tested on 
both environment), and have Windows 10 SDK installed 
(version 10240 or higher, which is basically any 
version).

First, unzip the src.7z zip file and open DSEngine.sln
with Visual Studio, then simply build the solution with
Build - Build Solution menu. We use vcpkg for package
management and we have a pre-build script that will 
automatically get the vcpkg, install it and build the
required 3rd party libraries. The vcpkg will also handle
the linkage so no additional steps needed. 

Notice that it will take a very long time (around 1 ~ 1.5 
hours) for vcpkg to install the packages since it compiles 
everything we need (boost, assimp, ffmpeg, directxtk) from 
source. And it will also take a huge amount of disk space 
after the installation. You'll also need to have 
administrator privilege and write access to the Visual 
Studio installation folder because vcpkg will try to write 
new files for automatically linking libraries.

Also, there's an issue with vcpkg building ffmepg that will
possibly freeze the compiling process. In this case, simply 
force the vcpkg or Visual Studio to end is fine. The details 
can be found here:
https://github.com/Microsoft/vcpkg/issues/5476

You'll need to put Assets folder to proper locations to debug
the compiled binaries with Visual Studio. Unzip the Assets 
folder, and put them under:
  "../src/DSEngine/DSEngineTestProject", 
  "../src/DSEngine/RacingGame",
  "../src/DSEngine/Tower"
to debug each project.
  
-------------------------------------------------------------

For running the pre-built binaries:

The binaries are located in bin.7z zip file. Simply unzip it.

The binaries are compiled with Visual Studio 2017 so make sure
that you have vc2017 redistributables installed.

You'll need to put Assets folder to the same location with the 
pre-built binaries (exe files). Unzip the Assets.7z zip file,
and put it under bin folder. The Assets folder should not be
wrapped by itself, which means, you should not see something
like "../bin/Assets/Assets/..". 

Since we are also using the engine for IGME740 GGP course, we
have an extra bonus from that course. But the post-processing
graphics stuff are not optimized and very slow. If you find 
out that running the binaries on your computer is unbearably
slow, you can get the DSENGINESYSTEM.dll file from patch.7z
and replace the one in bin.7z. This will disable all post-
processing effects while keep other things the same.

-------------------------------------------------------------

Controls:

DSEngineTestProject:

W,A,S,D: move the camera
Mouse: rotate the camera

RacingGame:

W: accelerate
S: brake
A,D: steering

Controller RT: accelerate
Controller LT: brake
Controller Left Thumb: steering

Tower:

1,2,3: choose a brick to place when no brick is chosen, or place 
       the brick to the specified column when a brick is chosen.
space: draw a task card when possible
W,A,S,D: move the camera
Mouse movement when holding the right button: rotate the camera