# Second EGC Homework - SkyRoads

The problem statement can be found [here](https://ocw.cs.pub.ro/courses/egc/teme/2020/02).

## Table of Contents

- [Second EGC Homework - SkyRoads](#second-egc-homework---skyroads)
  - [Table of Contents](#table-of-contents)
  - [Gameplay details](#gameplay-details)
  - [Implementation details](#implementation-details)
    - [Game Manager](#game-manager)
    - [Game Engine Namespace](#game-engine-namespace)
      - [GameObject](#gameobject)
      - [Physics](#physics)
      - [Collision Manager](#collision-manager)
    - [Meshes & Shaders](#meshes--shaders)

## Gameplay details

The game is an endless runner, platforms being continuously spawned/despawned. The game will end if the player touched a **red platform**, fell out of the map or ran out of lives. The player can lose a life if he runs out of fuel. There are platforms that can give/take fuel (**green/yellow**), and some platform that can give a life (**white**). Most platforms are **blue** and have no effect. **Orange** platforms will set the speed of the player to maximum for a certain amount of time. The white and red platforms are the rarest, as they have the biggest impact on the gameplay.

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

There are two important **UI Elements** : the `fuel bar` and the `lives counter`. The fuel bar is placed on the left side of the screen and scales with the percent fuel remaining. The lives counter is placed on the right side of the screen, and it will display a red square for every life available.

As previously stated, the **game manager** handles the object spawning, in particular, platforms. The way this works is:

- there is an limit to the amount of platforms in the scene
- a platform will be spawned in the lane that last spawned a platform
- the type of platform will be chosen randomly, with a specific change for each type of platform to spawn, most of the platforms being blue. In case we can choose a special platform, **some** of the platforms will be `yellow`, **few** will be `green` or `orange`, and **very few** will be `red` or `white` (all these spawn chances were chosen based on the impact a platform can have on the gameplay).
- a gap will be chosen between the last platform of the lane and the new one
- the platform is spawned

Platforms that are out of sight are removed (after a specific delay).

### Game Engine Namespace

This namespace contains more generic classes and functions (not specifically related to this game, with a few exceptions). In this namespace we can find the implementations for the:

- `GameObject` - encapsulates different components that define an object in the game - the player, platforms, UI, etc..
- `Colliders` - implements the different colliders types attached to the game objects
- `CollisionManager` - manages the collision
- `Physics` - used to compute things like the position and velocity of a game object, to implement gravity and drag
- `Transform` - implements a few 3D Transforms (only translate and scale)
- `Camera` - the camera used by the game (a slightly modified version of the camera used in one of the laboratories)

#### GameObject

Every object in the game needs some components like a `mesh`, a `shader`, transforms (`position, scale`), `color`, and some (like the player, or the platforms) even require additional information (`collider`, `rigidbody`, `lightingInfo`).

Using this information, we can "update every object" from the game manager : the physics (position mainly), collisions, and to render it. The game object class contains it's own rendering methods (`Render` and `Render2D`), choosing between them based on the type of object: 3D or 2D(UI elements).

#### Physics

The "physics engine" used in this game is based on the one I created for the previous project, updated for 3D computations.

#### Collision Manager

As there are two types of colliders, there are 3 types of collisions that can happen (in this game, only one interest us, but I wanted to have a more generic implementation) :

- `BoxCollider` - `BoxCollider`
- `BoxCollider` - `SphereCollider`
- `SphereCollider` - `SphereCollider`

### Meshes & Shaders

The game uses two meshes,  `box` & `sphere` . However, any mesh can be used, by placing it in the **Meshes** folder, and adding it's name to the `meshNames` constant in the `GameManager`.

There are 3 shaders used by the game:

- **Base** - the default shader used by the game, a **Phong Lighting shader** (implemented in the **Fragment Shader**). This shader is used for the platforms.
- **UI** - a simple shader, used in the rendering of the UI.
- **Distorted** - a shader similar on the **Base** shader, used to render the player. It uses noise to distort the mesh (if `is_distorted` variable is set to true) and to change the light intensity in the fragment shader.

Shaders can be added to the game by placing the two files (`shader_name`.VS.glsl, `shader_name`.FS.glsl) in the **Shaders** folder, and adding `shader_name` to the `meshNames` constant in the `GameManager`.

© 2020 Grama Nicolae, 332CA
