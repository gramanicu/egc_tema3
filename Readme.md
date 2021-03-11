# Third EGC Homework - SkyRoads

The problem statement can be found [here](https://ocw.cs.pub.ro/courses/egc/teme/2020/03). This is a continuation of the 2nd homework.

## Table of Contents

- [Third EGC Homework - SkyRoads](#third-egc-homework---skyroads)
  - [Table of Contents](#table-of-contents)
  - [Gameplay details](#gameplay-details)
  - [Implementation details](#implementation-details)
    - [Game Manager](#game-manager)
    - [Game Engine Namespace](#game-engine-namespace)
      - [GameObject](#gameobject)
      - [Physics](#physics)
      - [Collision Manager](#collision-manager)
    - [Rendering/Graphics](#renderinggraphics)
      - [Meshes, Textures & Shaders](#meshes-textures--shaders)

## Gameplay details

The game is an endless runner, platforms being continuously spawned/despawned. The game will end if the player touched a **red platform**, fell out of the map or ran out of lives. The player can lose a life if he runs out of fuel. There are platforms that can give/take fuel (**green/yellow**), and some platform that can give a life (**white**). Most platforms are **blue** and have no effect. **Orange** platforms will set the speed of the player to maximum for a certain amount of time. The white and red platforms are the rarest, as they have the biggest impact on the gameplay. There are two other interactable objects in the game, a green box that gives bonus points, and a red obstacle that takes a life away if it is hit.

By pressing `W` or `S` keys, the player can alter it's speed (this will also affect the **fuel consumption rate**). By pressing `C`, you can change between 3rd Person camera and 1st Person camera.

To move from side to side, you press the `A` and `D` keys. However, this movement "doesn't snap" to platforms (by pressing A, you won't instantly move over the platform to your left).

To jump, you can press `Space`. There are no double jumps, but you can jump if you fell of the platform (jump while airborne). Also, you can climb on a platform by touching it (touching the platform, even sideways, will put the player on it).

## Implementation details

In this section, I will present a few details about the implementation/development. The rest of the information can be found in the code comments.

### Game Manager

This class contains the logic for the game. This class stores the game objects,  meshes and shaders, camera, etc.. It also manages input, UI, loads the meshes and shaders, stores the game constants and the game state.

All objects in the game are stored in an `unordered_map`, that uses the `object ID` as it's key (the ID is unique, **ID=0** is the **player**).

For every frame, in the `Update` method, the **Game Manager**:

1. **Updates the game state** (fuel, score, lives, spawn/despawn platforms, checks if the game is over, camera, input)
2. **Renders the UI**
3. **Updates every object**
   1. Update its physics state
   2. Check for collisions
   3. Render the object

**The camera** used by the game is linked to the player's position (like the light, which is placed over the player). The camera can be rotated by using `Left Click` + `Mouse Drag` (in both camera modes). The FOV of the camera is linked to the speed of the player (effect used create the impression that the player is moving even faster).

There are two important **UI Elements** : the `fuel bar` and the `lives counter`. The fuel bar is placed on the left side of the screen and scales with the percent fuel remaining. The lives counter is placed on the right side of the screen.

As previously stated, the **game manager** handles the object spawning, in particular, platforms. The way this works is:

- there is an limit to the amount of platforms in the scene
- a platform will be spawned in the lane that last spawned a platform
- the type of platform will be chosen randomly, with a specific change for each type of platform to spawn, most of the platforms being blue. In case we can choose a special platform, **some** of the platforms will be `yellow`, **few** will be `green` or `orange`, and **very few** will be `red` or `white` (all these spawn chances were chosen based on the impact a platform can have on the gameplay).
- a gap will be chosen between the last platform of the lane and the new one
- the platform is spawned

Platforms that are out of sight are removed (after a specific delay). So are the boxes (green and red ones), and the decorative planets.

### Game Engine Namespace

This namespace contains more generic classes and functions (not specifically related to this game, with a few exceptions). In this namespace we can find the implementations for the:

- `GameObject` - encapsulates different components that define an object in the game - the player, platforms, UI, etc..
- `Colliders` - implements the different colliders types attached to the game objects
- `CollisionManager` - manages the collision
- `Physics` - used to compute things like the position and velocity of a game object, to implement gravity and drag
- `Transform` - implements a few 3D Transforms (only translate and scale)
- `Camera` - the camera used by the game (a slightly modified version of the camera used in one of the laboratories)
- `Lighting` - data structures used to store data used in the shaders (material and light properties)
- `Objects` - hardcoded meshes (quad, cube and sphere).

#### GameObject

Every object in the game needs some components like a `mesh`, a `shader`, transforms (`position, scale`), `color`, and some (like the player, or the platforms) even require additional information (`collider`, `rigidbody`, `light`, 'texture', 'material').

Using this information, we can "update every object" from the game manager : the physics (position mainly), collisions, and to render it. The game object class contains it's own rendering methods (`Render` and `Render2D`), choosing between them based on the type of object: 3D or 2D(UI elements).

#### Physics

The "physics engine" used in this game is based on the one I created for the previous project, updated for 3D computations.

#### Collision Manager

As there are two types of colliders, there are 3 types of collisions that can happen (in this game, only one interest us, but I wanted to have a more generic implementation) :

- `BoxCollider` - `BoxCollider`
- `BoxCollider` - `SphereCollider`
- `SphereCollider` - `SphereCollider`

### Rendering/Graphics

In this section I will present a few details about graphics/rendering

#### Meshes, Textures & Shaders

Beside the 3 hardcoded meshes, the game uses a another sphere mesh (for the `skybox`) and a more detailed mesh for the player (the `spaceship`).
There are many different textures used, most of them just for the coloring, but others are used as maps (emission maps). Note - The `space textures` were taken from https://www.solarsystemscope.com/textures/

There are multiple shaders used by the game:

- **Base** - the default shader used by the game, implements a lot of different features (Blinn-Phong illumination, HDR, multiple-source illumination)
- **UI** - a simple shader, used in the rendering of the UI.
- **Distorted** - not used anymore. Was used in the 1st iteration of the game, for the player
- **EmissiveTransparency** - similar to the `Base` shader, instead of discarding _transparent fragments_, it will replace them with an emission (for the bloom effect)
- **Planet** - a simplified shader, specifically made to render the planets/stars (as i don't want them to be illuminated, only emit color)
- **Skybox** - a very simple shader, just renders the texture
- **Spaceship** - a custom shader, used to render the spaceship and use 2 different emission maps
- **Blur** - a shader used during the _ping pong_ rendering phase, used by the 2-pass Gaussian Blur. (to create the blur effect in the second color buffer)

This iteration of the game uses a more advanced rendering method, to be able to use HDR and anti-aliasing at the same time.

© 2021 Grama Nicolae, 332CA
