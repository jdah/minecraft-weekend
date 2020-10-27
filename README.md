# Minecraft, but I made it in 48 hours*

\* I've actually updated it since - [see this commit for the 48 hour version](https://github.com/jdah/minecraft-weekend/tree/cb19738305804b5734faa7118c1c784f26ff9463).

![screenshot](screenshots/1.png)

##### Features:
- Infinite, procedurally generated world
- Infinite height/depth
- Day/night cycle
- Biomes
- ECS-driven player and entities with full colision and movement
- Full RGB lighting
- Full transparency + translucency support
- Sprite blocks (flowers)
- Animated blocks (water + lava)
- Distaince fog
- A whole lot of different block types
- More

##### Building

###### Unix-like

`$ make`

The following static libraries under `lib/` must be built before the main project can be built:

- GLAD `lib/glad/src/glad.o`
- CGLM `lib/cglm/.libs/libcglm.a`
- GLFW `lib/glfw/src/libglfw3.a`
- libnoise `lib/noise/libnoise.a`

All of the above have their own Makefile under their respective subdirectory.

###### Windows

good luck 🤷‍♂️ probably try building under WSL and using an X environment to pass graphics through.