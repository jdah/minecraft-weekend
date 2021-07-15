# Minecraft, but I made it in 48 hours*

\* I've actually updated it since - [see this commit for the 48 hour version](https://github.com/jdah/minecraft-weekend/tree/cb19738305804b5734faa7118c1c784f26ff9463).

![screenshot](screenshots/1.png)

#### Features:
- Infinite, procedurally generated world
- Infinite height/depth
- Day/night cycle
- Biomes
- ECS-driven player and entities with full collision and movement
- Full RGB lighting
- Full transparency + translucency support
- Sprite blocks (flowers)
- Animated blocks (water + lava)
- Distance fog
- A whole lot of different block types
- More

#### Building

##### Unix-like
`$ git clone --recurse-submodules https://github.com/jdah/minecraft-weekend.git`\
`$ make`

The following static libraries under `lib/` must be built before the main project can be built:

- GLAD `lib/glad/src/glad.o`
- CGLM `lib/cglm/.libs/libcglm.a`
- GLFW `lib/glfw/src/libglfw3.a`
- libnoise `lib/noise/libnoise.a`

All of the above have their own Makefile under their respective subdirectory and can be built with `$ make libs`.
If libraries are not found, ensure that submodules have been cloned.

The game binary, once built with `$ make`, can be found in `./bin/`.

*Be sure* to run with `$ ./bin/game` out of the root directory of the repository.
If you are getting "cannot open file" errors (such as "cannot find ./res/shaders/*.vs"), this is the issue. 

##### Windows

With the annoucement of Linux GUI apps for WSL, you can run this game on Windows.

First install the Insider preview of [Windows 10](https://insider.windows.com/en-us/register)

Then open Powershell as Administrator and type:
`wsl --install`
This will install Ubuntu for WSL. Make sure to reboot your machine.

Next you want to follow the setup instructions for Ubuntu.

Update and upgrade apt:
`sudo apt update`
`sudo apt upgrade`

Now use apt to install the following packages:
`sudo apt install clang cmake make xorg-dev libglu1-mesa-dev doxygen libx11-dev`

If you encounter any errors saying `E: Unable to locate package` update and upgrade apt again.

Now follow the steps found in the Unix-like section and you should be able to run the game.
