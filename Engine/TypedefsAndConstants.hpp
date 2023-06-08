#pragma once

#include <functional>

#include <entt/entt.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

constexpr const char* GLSL_VERSION = "#version 430";
constexpr int OPGL_VERSION_MAJ = 3;
constexpr int OPGL_VERSION_MIN = 3;

typedef GLuint VAO;
typedef GLuint VAOBindSlot;
typedef GLuint VBO;
typedef GLuint EBO;
typedef GLuint FBO;
typedef GLuint TEX;
typedef GLuint RBO;
typedef GLuint ProgramID;
typedef GLint UniformLocation;

typedef std::function<void(float)> ImGuiFunction;

/* This is taken from glfw3.h, if that wasn't obvious. They only exist to be registered to Angel Script. */
static int KEY_UNKNOWN = GLFW_KEY_UNKNOWN;
static int KEY_SPACE = GLFW_KEY_SPACE;
static int KEY_APOSTROPHE = GLFW_KEY_APOSTROPHE; /* ' */
static int KEY_COMMA = GLFW_KEY_COMMA; /* , */
static int KEY_MINUS = GLFW_KEY_MINUS; /* - */
static int KEY_PERIOD = GLFW_KEY_PERIOD; /* . */
static int KEY_SLASH = GLFW_KEY_SLASH; /* / */
static int KEY_0 = GLFW_KEY_0;
static int KEY_1 = GLFW_KEY_1;
static int KEY_2 = GLFW_KEY_2;
static int KEY_3 = GLFW_KEY_3;
static int KEY_4 = GLFW_KEY_4;
static int KEY_5 = GLFW_KEY_5;
static int KEY_6 = GLFW_KEY_6;
static int KEY_7 = GLFW_KEY_7;
static int KEY_8 = GLFW_KEY_8;
static int KEY_9 = GLFW_KEY_9;
static int KEY_SEMICOLON = GLFW_KEY_SEMICOLON; /* ; */
static int KEY_EQUAL = GLFW_KEY_EQUAL; /* = */
static int KEY_A = GLFW_KEY_A;
static int KEY_B = GLFW_KEY_B;
static int KEY_C = GLFW_KEY_C;
static int KEY_D = GLFW_KEY_D;
static int KEY_E = GLFW_KEY_E;
static int KEY_F = GLFW_KEY_F;
static int KEY_G = GLFW_KEY_G;
static int KEY_H = GLFW_KEY_H;
static int KEY_I = GLFW_KEY_I;
static int KEY_J = GLFW_KEY_J;
static int KEY_K = GLFW_KEY_K;
static int KEY_L = GLFW_KEY_L;
static int KEY_M = GLFW_KEY_M;
static int KEY_N = GLFW_KEY_N;
static int KEY_O = GLFW_KEY_O;
static int KEY_P = GLFW_KEY_P;
static int KEY_Q = GLFW_KEY_Q;
static int KEY_R = GLFW_KEY_R;
static int KEY_S = GLFW_KEY_S;
static int KEY_T = GLFW_KEY_T;
static int KEY_U = GLFW_KEY_U;
static int KEY_V = GLFW_KEY_V;
static int KEY_W = GLFW_KEY_W;
static int KEY_X = GLFW_KEY_X;
static int KEY_Y = GLFW_KEY_Y;
static int KEY_Z = GLFW_KEY_Z;
static int KEY_LEFT_BRACKET = GLFW_KEY_LEFT_BRACKET; /* [ */
static int KEY_BACKSLASH = GLFW_KEY_BACKSLASH; /* \ */
static int KEY_RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET; /* ] */
static int KEY_GRAVE_ACCENT = GLFW_KEY_GRAVE_ACCENT; /* ` */
static int KEY_WORLD_1 = GLFW_KEY_WORLD_1; /* non-US #1 */
static int KEY_WORLD_2 = GLFW_KEY_WORLD_2; /* non-US #2 */
static int KEY_ESCAPE = GLFW_KEY_ESCAPE;
static int KEY_ENTER = GLFW_KEY_ENTER;
static int KEY_TAB = GLFW_KEY_TAB;
static int KEY_BACKSPACE = GLFW_KEY_BACKSPACE;
static int KEY_INSERT = GLFW_KEY_INSERT;
static int KEY_DELETE = GLFW_KEY_DELETE;
static int KEY_RIGHT = GLFW_KEY_RIGHT;
static int KEY_LEFT = GLFW_KEY_LEFT;
static int KEY_DOWN = GLFW_KEY_DOWN;
static int KEY_UP = GLFW_KEY_UP;
static int KEY_PAGE_UP = GLFW_KEY_PAGE_UP;
static int KEY_PAGE_DOWN = GLFW_KEY_PAGE_DOWN;
static int KEY_HOME = GLFW_KEY_HOME;
static int KEY_END = GLFW_KEY_END;
static int KEY_CAPS_LOCK = GLFW_KEY_CAPS_LOCK;
static int KEY_SCROLL_LOCK = GLFW_KEY_SCROLL_LOCK;
static int KEY_NUM_LOCK = GLFW_KEY_NUM_LOCK;
static int KEY_PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN;
static int KEY_PAUSE = GLFW_KEY_PAUSE;
static int KEY_F1 = GLFW_KEY_F1;
static int KEY_F2 = GLFW_KEY_F2;
static int KEY_F3 = GLFW_KEY_F3;
static int KEY_F4 = GLFW_KEY_F4;
static int KEY_F5 = GLFW_KEY_F5;
static int KEY_F6 = GLFW_KEY_F6;
static int KEY_F7 = GLFW_KEY_F7;
static int KEY_F8 = GLFW_KEY_F8;
static int KEY_F9 = GLFW_KEY_F9;
static int KEY_F10 = GLFW_KEY_F10;
static int KEY_F11 = GLFW_KEY_F11;
static int KEY_F12 = GLFW_KEY_F12;
static int KEY_F13 = GLFW_KEY_F13;
static int KEY_F14 = GLFW_KEY_F14;
static int KEY_F15 = GLFW_KEY_F15;
static int KEY_F16 = GLFW_KEY_F16;
static int KEY_F17 = GLFW_KEY_F17;
static int KEY_F18 = GLFW_KEY_F18;
static int KEY_F19 = GLFW_KEY_F19;
static int KEY_F20 = GLFW_KEY_F20;
static int KEY_F21 = GLFW_KEY_F21;
static int KEY_F22 = GLFW_KEY_F22;
static int KEY_F23 = GLFW_KEY_F23;
static int KEY_F24 = GLFW_KEY_F24;
static int KEY_F25 = GLFW_KEY_F25;
static int KEY_KP_0 = GLFW_KEY_KP_0;
static int KEY_KP_1 = GLFW_KEY_KP_1;
static int KEY_KP_2 = GLFW_KEY_KP_2;
static int KEY_KP_3 = GLFW_KEY_KP_3;
static int KEY_KP_4 = GLFW_KEY_KP_4;
static int KEY_KP_5 = GLFW_KEY_KP_5;
static int KEY_KP_6 = GLFW_KEY_KP_6;
static int KEY_KP_7 = GLFW_KEY_KP_7;
static int KEY_KP_8 = GLFW_KEY_KP_8;
static int KEY_KP_9 = GLFW_KEY_KP_9;
static int KEY_KP_DECIMAL = GLFW_KEY_KP_DECIMAL;
static int KEY_KP_DIVIDE = GLFW_KEY_KP_DIVIDE;
static int KEY_KP_MULTIPLY = GLFW_KEY_KP_MULTIPLY;
static int KEY_KP_SUBTRACT = GLFW_KEY_KP_SUBTRACT;
static int KEY_KP_ADD = GLFW_KEY_KP_ADD;
static int KEY_KP_ENTER = GLFW_KEY_KP_ENTER;
static int KEY_KP_EQUAL = GLFW_KEY_KP_EQUAL;
static int KEY_LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT;
static int KEY_LEFT_CONTROL = GLFW_KEY_LEFT_CONTROL;
static int KEY_LEFT_ALT = GLFW_KEY_LEFT_ALT;
static int KEY_LEFT_SUPER = GLFW_KEY_LEFT_SUPER;
static int KEY_RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT;
static int KEY_RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL;
static int KEY_RIGHT_ALT = GLFW_KEY_RIGHT_ALT;
static int KEY_RIGHT_SUPER = GLFW_KEY_RIGHT_SUPER;
static int KEY_MENU = GLFW_KEY_MENU;
constexpr int KEY_LAST = GLFW_KEY_LAST;

static int MOUSE_BUTTON_1 = GLFW_MOUSE_BUTTON_1;
static int MOUSE_BUTTON_2 = GLFW_MOUSE_BUTTON_2;
static int MOUSE_BUTTON_3 = GLFW_MOUSE_BUTTON_3;
static int MOUSE_BUTTON_4 = GLFW_MOUSE_BUTTON_4;
static int MOUSE_BUTTON_5 = GLFW_MOUSE_BUTTON_5;
static int MOUSE_BUTTON_6 = GLFW_MOUSE_BUTTON_6;
static int MOUSE_BUTTON_7 = GLFW_MOUSE_BUTTON_7;
static int MOUSE_BUTTON_8 = GLFW_MOUSE_BUTTON_8;
constexpr int MOUSE_BUTTON_LAST = GLFW_MOUSE_BUTTON_LAST;
static int MOUSE_BUTTON_LEFT = GLFW_MOUSE_BUTTON_LEFT;
static int MOUSE_BUTTON_RIGHT = GLFW_MOUSE_BUTTON_RIGHT;
static int MOUSE_BUTTON_MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE;
