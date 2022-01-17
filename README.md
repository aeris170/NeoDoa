![NeoDoa](https://user-images.githubusercontent.com/25724155/72576385-9ca35100-38e0-11ea-9f10-5de3852e6df3.png "NeoDoa Logo")

# NeoDoa Game Engine

NeoDoa is a simple, open-source and easy-to-use game engine developed with C++ for high fun game development! NeoDoa uses OpenGL to deliver high performance by hardware accelerating the rendering. 

<div align="center"> Leave a ⭐ if you like the project! </div> 

## Features

NeoDoa is still under development and is not ready for producing games, but it will be 😎 Below are the finished and upcoming features.

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
	* WIP: Code Editor
	* WIP: Custom Shaders
	* WIP: Mouse Picking to select Entity
	* Outlining of selected Entity
	* Editor Camera
	* WIP: Camera Component
	* WIP: Terrain Editor
	* WIP: Tree Maker
	* WIP: Skeletal Animation/Ragdoll Editor
	* ... and many more
	* ... help :(

## How to build

You need Visual Studio 2019 to build NeoDoa in the most convenient and easy way possible. Older versions may work but wasn't tested against.

 * Clone the repository
 * Open Visual Studio
 * File->Open->Project/Solution
 * Select "NeoDoa.sln" in the root of repo
 * Set "Editor" as the start-up project
 * Hit F5
 
A single window named "NeoDoa Editor" will appear.

## Contributing

Want to help me make this project better? Great!
Check out [CONTRIBUTING.md](https://github.com/aeris170/NeoDoa/blob/master/CONTRIBUTING.md) to get started.

### Github Issues

Please use [Github Issues](https://github.com/aeris170/NeoDoa/issues) to report bugs or request features.

### Reporting bugs

Please follow these steps to report a bug

1. **Search for related issues** - search the existing issues so that you don't create duplicates

2. **Create a testcase** - please create the smallest isolated testcase that you can that reproduces your bug

3. **Share as much information as possible** - everything little helps, OS, IDE, GPU, all that stuff.

## NeoDoa Platform

NeoDoa is an open source game engine which you can use to develop games and it is strictly for Windows PC (at the moment).
If you want to bring multi-platform support, fell free to get your hands dirty. All help is welcome!

## License

NeoDoa is released under its own license. See [licence](https://github.com/aeris170/NeoDoa/LICENCE).
