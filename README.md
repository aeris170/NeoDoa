![NeoDoa](https://user-images.githubusercontent.com/25724155/72576385-9ca35100-38e0-11ea-9f10-5de3852e6df3.png "NeoDoa Logo")

# NeoDoa Game Engine
**NeoDoa** is a simple, open-source, easy-to-use and performant game engine developed with C++ for high fun game development! 
<div align="center"> 

### Leave a ⭐ if you like the project!
</div> 

## Features

**NeoDoa** is still under development and is not ready for producing games, but it will be 😎 Below are the finished and upcoming features.

* **Graphics**
	* **Hot-swappable graphics backends**
	* OpenGL 4.6 Core Profile
	* WIP: OpenGL 3.3 Core Profile
	* WIP: Direct3D12
	* WIP: Direct3D11
	* WIP: Vulkan
* **Collision & Physics**
	* WIP: With PhysX
* **Audio**
 	* WIP: With OpenAL
* **Asset Loading**
	* **Don't bother** - automatic importing and categorizing of assets
	* **Have control** - import/load/unload assets at will
	* **Don't pollute** - use ubiquitous file formats, without needing special formats only NeoDoa can interpret
	* **Never crash** - get nice error lists on why loads have failed and placeholders for unloaded assets
	* 3D models loading via [Assimp](https://www.assimp.org/)
	* Texture loading via [stb_image](https://github.com/nothings/stb)
* **Scripting system**
	* Pure ECS
	* Define components and implement systems in C++
* **Input**
	* Mouse
	* Keyboard
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

| OS            | Build scripts provided |
|---------------|:-:|
| Windows 10/11 |✅|
| Debian/Ubuntu |✅|
| openSUSE      |❌ (planned)|
| Arch          |❌ (planned)|

### Prerequisites:
 * git 2.40.1 or higher (GIT Bash on Windows)
 * CMake 3.26.4 or higher
 * A C++ compiler with C++23 support (clang 18.1.3, MSVC 19.30) or higher

### Building 
#### 1. Start by cloning the repository by SSH (recommended)
``` sh
git clone --recurse-submodules -j8 git@github.com:aeris170/NeoDoa.git
```
or by HTTPS
``` sh
git clone --recurse-submodules -j8 https://github.com/aeris170/NeoDoa.git
```

#### 2. Fetching Dependencies

**NeoDoa** uses `vcpkg` to manage packages and depends on packages listed below: 
- angelscript[addons]
- argparse
- assimp
- cppzmq
- entt
- eventpp
- glew
- glfw3
- glm
- icu
- imgui[core,docking-experimental,glfw-binding,sdl2-binding,opengl3-binding,vulkan-binding,*dx11-binding,dx12-binding*] **(dx11-binding and dx12-binding only installed for Windows 10/11)**
- imguizmo
- lunasvg
- stb
- tinyxml2

The packages above must be installed. Refer to the table below.

| OS            | How to install dependencies? |
|---------------|:-:|
| Windows 10/11 |[`vcpkg.ps1`](https://github.com/aeris170/NeoDoa/blob/master/scripts/vcpkg.ps1)|
| Debian/Ubuntu |[`deb_vcpkg.sh`](https://github.com/aeris170/NeoDoa/blob/master/scripts/deb_vcpkg.sh)|
| openSUSE      |Manual via terminal|
| Arch          |Manual via terminal|

The scripts mentioned take two arguments `clonemode` and `vcpkg-dir`.

* `clonemode` controls how to clone the `vcpkg` repository. Passing nothing defaults to `ssh`, only `https` and `ssh` are accepted. **`ssh` (default) is recommended.**

* `vcpkg-dir` controls where to clone the `vcpkg` repository. Passing nothing defaults to `./vcpkg`. **`./vcpkg` (default) is recommended.**

Example:
```sh
./deb_vcpkg.sh ssh ./vcpkg # Arguments can be omitted, passed defaults
```

You should see no errors when this scripts completes. If you do, please create an issue [here](https://github.com/aeris170/NeoDoa/issues).

#### 3. Generating with CMake

Building the makefile (or the Visual Studio Project) could be done by running cmake or by running helper scripts:
[`cmake.sh`](https://github.com/aeris170/NeoDoa/blob/master/cmake.sh)
[`cmakeLinuxDebug.sh`](https://github.com/aeris170/NeoDoa/blob/master/cmakeLinuxDebug.sh)
[`cmakeLinuxRelease.sh`](https://github.com/aeris170/NeoDoa/blob/master/cmakeLinuxRelease.sh)

For Windows (Visual Studio Project):
```sh
cmake.sh
```
For Linux:
```sh
chmod +x ./cmakeLinuxDebug.sh # cmakeLinuxRelease.sh
sh cmakeLinuxDebug.sh # OR sh cmakeLinuxRelease.sh
```

#### 4. Building!

If you built to a makefile, what's left is calling `make`

```sh
cd build
make # make -j16
```

---

If you built to a Visual Studio Project, boot up a Visual Studio instance and press F5.

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