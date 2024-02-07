![NeoDoa](https://user-images.githubusercontent.com/25724155/72576385-9ca35100-38e0-11ea-9f10-5de3852e6df3.png "NeoDoa Logo")

# NeoDoa Game Engine
**NeoDoa** is a simple, open-source and easy-to-use game engine developed with C++ for high fun game development! NeoDoa uses OpenGL to deliver high performance by hardware accelerating the rendering. 
<div align="center"> 

### Leave a ⭐ if you like the project!
</div> 

## Features

**NeoDoa** is still under development and is not ready for producing games, but it will be 😎 Below are the finished and upcoming features.

* **Graphics**
	* OpenGL 4.5 Core Profile based renderer
	* Agressive Instancing and Batched Rendering!!
	* WIP: Point and spot and custom lights
	* WIP: High Quality Text Rendering using [Signed Distance Fields](https://steamcdn-a.akamaihd.net/apps/valve/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf)
	* WIP: Shadows
	* WIP: Waters
	* WIP: Skybox
	* WIP: Deferred Shading
	* WIP: Skeletal Animations
* **Collision & Physics**
	* WIP: With PhysX
* **Audio**
 	* WIP: With OpenAL
* **Asset Loading**
	* 3D models loading via [Assimp](https://www.assimp.org/)
	* Texture loading via [stb_image](https://github.com/nothings/stb)
* **Scripting system**
	* Scripting via [AngelScript](https://www.angelcode.com/angelscript/)
	* With true ECS!
* **Input**
	* Mouse, Keyboard. 
	* WIP: Controller
	* WIP: Joystick
* **Editor**
	* Project system with Scene serialization with [TinyXML2](https://github.com/leethomason/tinyxml2)
	* Asset Manager
	* Code Editor
	* Custom Shaders
	* WIP: Mouse Picking to select Entity
	* WIP: Outlining of selected Entity
	* Editor Camera
	* WIP: Terrain Editor
	* WIP: Tree Maker
	* WIP: Skeletal Animation/Ragdoll Editor
	* ... and many more
	* ... help :(

## How to build

### Prerequisites:
 * git 2.40.1 or higher (GIT Bash on Windows)
 * CMake 3.26.4 or higher
 * A C++ compiler with C++23 support (gcc 13.1, clang 17.0.1, MSVC 19.30) or higher

### Building 
#### 1. Start by cloning the repository by SSH
``` sh
git clone git@github.com:aeris170/NeoDoa.git
```
or by HTTPS
``` sh
git clone https://github.com/aeris170/NeoDoa.git
```

When cloning is done, go into the cloned folder and initialize submodules
``` sh
cd NeoDoa
git submodule update --init
```

After this, `git pull` should print `Already up to date.` Proceed to the next step.

#### 2. Fetching Required Installations (Linux ONLY)

**If you are using Windows, please skip this step.**

**NeoDoa** requires various installations from `apt-get` to compile. These are listed below:
> build-essential
> curl zip unzip tar
> pkg-config
> libxinerama-dev
> libxcursor-dev
> xorg xorg-dev
> libglu1-mesa libglu1-mesa-dev
> autoconf autoconf-archive
>

These can either be install manually or by launching the helper script by executing:
```sh
sh FetchAptGet.sh
```

#### 3. Fetching Dependencies

**NeoDoa** uses `vcpkg` to manage packages and depends on packages listed below: 
> angelscript[addons]
> argparse
> assimp
> imgui[core,docking-experimental,glfw-binding,opengl3-binding]
> imguizmo
> entt
> eventpp
> glew 
> glfw3
> glm
> icu
> lunasvg
> stb
> tinyxml2

The packages above must be installed by launching the helper script [`FetchDependencies.sh`](https://github.com/aeris170/NeoDoa/blob/master/FetchDependencies.sh). This script takes two arguments
`platform` and `clonemode`. `platform` defaults to `x64-windows` and can be overriden to `x64-linux` or your OS of choosing
by passing it as the first argument. `clonemode` governs how the script should try to clone `vcpkg` repository. Passing nothing
defaults to `https`, only `https` and `ssh` are accepted.

Examples:
```sh
sh FetchDependencies.sh                  # Fetch packages for x64-windows using https
sh FetchDependencies.sh x64-windows ssh  # Fetch packages for x64-windows using ssh (must have an ssh key set-up)
sh FetchDependencies.sh x64-linux        # Fetch packages for x64-linux   using https (doesn't work on our test systems)
sh FetchDependencies.sh x64-linux ssh    # Fetch packages for x64-linux   using ssh (must have an ssh key set-up)
```

You should see no errors when this scripts completes. If you do, please create an issue [here](https://github.com/aeris170/NeoDoa/issues).

#### 4. Building with CMake

Building the makefile (or Visual Studio Project) CMake should be done with the commands below:

For Windows:
```sh
mkdir build
cd build
cmake ..
```
For Linux:
```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release .. # Debug or Release
```

#### OR

There are also the helper scripts [`cmake.sh`](https://github.com/aeris170/NeoDoa/blob/master/cmake.sh) to execute the commands above.

For Windows:
```sh
sh cmake.sh
```
For Linux:
```sh
sh cmakeLinuxDebug.sh # OR sh cmakeLinuxRelease.sh
```

#### 5. Building!

If you built to a makefile, what's left is calling `make`

```sh
make
```

---

If you build to Visual Studio Project, boot up a Visual Studio of your choosing and press F5.

## Contributing

Want to help us make this project better? Great!
Check out [CONTRIBUTING.md](https://github.com/aeris170/NeoDoa/blob/master/CONTRIBUTING.md) to get started.

### Github Issues

Please use [Github Issues](https://github.com/aeris170/NeoDoa/issues) to report bugs or request features.

### Reporting bugs

Please follow these steps to report a bug

1. **Search for related issues** - search the existing issues so that you don't create duplicates

2. **Create a testcase** - please create the smallest isolated testcase that you can that reproduces your bug

3. **Share as much information as possible** - everything little helps, OS, IDE, GPU, all that stuff.

## NeoDoa Platform

**NeoDoa** is an open source cross-platform game engine which you can use to develop games on Windows, Linux and MacOS (although untested on).
