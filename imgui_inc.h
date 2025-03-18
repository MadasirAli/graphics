#pragma once

#ifdef _DEBUG
#include "imgui.h"
#define IMGUI_CALL(x) x
#else
#define IMGUI_CALL(x) 
#endif