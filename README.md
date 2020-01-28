![NeoDoa](https://user-images.githubusercontent.com/25724155/72576385-9ca35100-38e0-11ea-9f10-5de3852e6df3.png "NeoDoa Logo")

# NeoDoa 2D Game Engine

NeoDoa is a simple, open-source and easy-to-use game engine developed with C++ for game development in C, C++ or C#. NeoDoa uses OpenGL to deliver high performance by hardware accelerating the rendering.

## Features

NeoDoa is still under development. Below are the finished features.

* **Graphics**
    * OpenGL 4.5 Core Profile based renderer
    * Point and spot and custom lights
	* High Quality Text Rendering using [Signed Distance Fields](https://steamcdn-a.akamaihd.net/apps/valve/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf)
	* __EZ Rendering__ mode for quick and easy shape rendering
* **Collision & Physics**
    * To be done.
* **Audio**
    * To be done.
* **Resource Loading**
	* Simple and easy TrueType Font(.ttf) loading and Signed Distance Field generation using [SDFont](https://www.gamedev.net/forums/topic.asp?topic_id=491938) (modified for use)
    * Simple and fast Texture loading via [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h)
* **Scripting system**
	* Scene based objects and logic.
	* Add objects to scenes and select an active scene.  
    * Extend __scene::GameObject__
>   Write object's behaviors to __update()__, and write object's appearance to __render()__
* **Input**
    * Mouse, Keyboard.

## Getting Started

Here's a super-simple example - a spinning red square using __EZ Rendering__!

### Square.h
```cpp
#pragma once
#include "doa.h"

using namespace doa;

class Square : public scene::GameObject {
private:
    float rot = 0;

public:
    void update(const scene::Scene& parent, const std::vector<GameObject*>& objects, const std::vector<scene::Light*>& lights) override;
    void render(const scene::Scene& parent, const std::vector<GameObject*>& objects, const std::vector<scene::Light*>& lights) const override;
};
```

### Square.cpp
```cpp
#include "Square.h"

void Square::update(const scene::Scene& parent, const std::vector<GameObject*>& objects, const std::vector<scene::Light*>& lights) {
    rot += .1f;
}


void Square::render(const scene::Scene& parent, const std::vector<GameObject*>& objects, const std::vector<scene::Light*>& lights) const {
    using namespace ezrender;

    //EZRenderer is a state machine
    //Configure the machine
    RotateZ(rot);
    Scale(150, 150);
    Shape(SQUARE);
    Color(1, .5, .5); //RGB[0, 1]
    Mode(FILL);

    //And press the big red button to render!
    Render(parent, objects, lights);
}
```

### main.cpp
```cpp
#include "doa.h"
#include "Square.h"

using namespace doa;

int main() {
    //Initialize Doa
    Init();

    //Create Window
    CreateWindow("Spinning Square Demo", 960, 540, false);

    //Create a scene
    scene::Scene *sampleScene{ new scene::Scene("sample") };

    //Set the scene as active scene
    scene::ACTIVE_SCENE = sampleScene;

    Square s;

    sampleScene->Add(&s);

    //Start the game loop
    Loop();

    //Let Doa Clean up everything
    Purge();
    return 0;
}
```

Want to play with the code yourself? Clone the repo and get started!

## How to build

You need Visual Studio 2017 to build NeoDoa in the most convenient and easy way possible.

 * Clone the repository
 * Open Visual Studio
 * File->Open->Project/Solution
 * Select "Doa.sln" in the root of repo
 * Hit F7
 
A single dll file with the name "Engine" will be generated in _repo-root_/x64/_build-config_ folder.


## C# Bindings

NeoDoa is compiled into a single unmanaged .dll file. You can 

```c#
[DllImport("Engine.dll", CallingConvention = CallingConvention.Cdecl)]
```

to import NeoDoa functions.

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

NeoDoa is an open source 2D game engine which you can use to develop games and it is strictly for Windows PC (at the moment).
If you want to bring multi-platform support, fell free to get your hands dirty. All help is welcome!

## License

NeoDoa is released under the [GNU GPLv3](https://choosealicense.com/licenses/gpl-3.0/) license. See LICENSE file.