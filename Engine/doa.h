// NeoDoa 2019, v0.1 ALPHA
#pragma once

//taken from https://www.youtube.com/watch?v=meARMOmTLgE&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&index=5
#ifdef DOA_WINDOWS
	#ifdef DOA_BUILD_DLL
		#define DOA_API __declspec(dllexport)
	#else
		#define DOA_API __declspec(dllimport)
	#endif
#else
#error DOA only has support for Windows x64. Feel free to fork and pr to bring support for other platforms.
#endif

#define _USE_MATH_DEFINES

#include <map>
#include <time.h>
#include <math.h>
#include <string>
#include <fstream>
#include <utility>
#include <iostream>
#include <streambuf>
#include <vector>
#include <algorithm>
#include <functional>
#include <thread>
#include <sstream>
#include <atomic>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/component_wise.hpp>

#include <vld.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "DoaEngine.h"
#include "DoaShaders.h"
#include "DoaTextures.h"
#include "DoaLight.h"
#include "DoaScene.h"
#include "DoaGameObject.h"
#include "DoaAnimations.h"
#include "DoaPrimitives.h"
#include "DoaKeyboard.h"
#include "DoaMouse.h"
#include "DoaCamera.h"
#include "DoaEzRenderer.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>
#endif