#pragma once

#ifdef _DEBUG
#include "imgui.h"
#define IMGUI_CALL(...) {__VA_ARGS__;}
#else
#define IMGUI_CALL(x) {}
#endif