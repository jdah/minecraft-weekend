# Minecraft, but I made it in 48 hours\*

\* I've actually updated it since - [see this commit for the 48 hour version](https://github.com/jdah/minecraft-weekend/tree/cb19738305804b5734faa7118c1c784f26ff9463).

![screenshot](screenshots/1.png)

## Features:

-   Infinite, procedurally generated world
-   Infinite height/depth
-   Day/night cycle
-   Biomes
-   ECS-driven player and entities with full collision and movement
-   Full RGB lighting
-   Full transparency + translucency support
-   Sprite blocks (flowers)
-   Animated blocks (water + lava)
-   Distance fog
-   A whole lot of different block types
-   More

## Building & Running Locally

###### Unix-like OS

Clone Repository + Sub Modules
```
git clone --recurse-submodules https://github.com/andrew-pynch/minecraft-weekend.git && cd minecraft-weekend
```

Make libraries this repo relies on
```
make libs
```
this builds the following static libraries under `lib/` must be built before the main project can be built:

-   GLAD `lib/glad/src/glad.o`
-   CGLM `lib/cglm/.libs/libcglm.a`
-   GLFW `lib/glfw/src/libglfw3.a`
-   libnoise `lib/noise/libnoise.a`

Make this project
```
make
```

Run Game
```
./bin/game
```
NOTE: do not try to run game from the /bin folder. Doing so will return 
```error loading shader at res/shaders/basic_texture.vs```



All of the above have their own Makefile under their respective subdirectory and can be built with `$make libs`.
If libraries are not found, ensure that submodules have been cloned.

## Windows
```
good luck 🤷‍♂️ probably try building under WSL and using an X environment to pass graphics through.
```
