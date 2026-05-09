#pragma once
#include <ctime>
#include <windows.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <map>
#include <functional>
#include <fstream>
#include <string>
#include <any>
#include <algorithm>
#include <array>

#include "./include/glm/gtc/type_ptr.hpp"
#include "./include/glm/gtc/matrix_transform.hpp"
#include "./include/glm/gtc/noise.hpp"
#include "./include/glm/gtx/rotate_vector.hpp"
#include "./include/glm/gtx/string_cast.hpp"
#include "./include/glm/gtc/type_ptr.hpp"
#include "./include/glm/gtc/matrix_transform.hpp"

#include "include/gl/glew.h"
#include <gl/gl.h>

#define _USE_MATH_DEFINES
#include <math.h>

// Macro to log error to the output window of visual studio
// All uses in final version were commented out
#define LOG_ERROR(format, ...) \
do { \
    char buffer[512]; \
    snprintf(buffer, sizeof(buffer), "[ERROR] " format "\n", ##__VA_ARGS__); \
    OutputDebugStringA(buffer); \
} while(0)

// Macro to log any debug message to the output window of visual studio
// All uses in final version were commented out
#define DEBUG_MSG(format, ...) \
do { \
    char buffer[512]; \
    snprintf(buffer, sizeof(buffer), "[DEBUG MESSAGE] " format "\n", ##__VA_ARGS__); \
    OutputDebugStringA(buffer); \
} while(0)

inline glm::vec3 ParseVec3(const std::string & value)
{
    std::stringstream ss(value);
    std::string val;
    float x = 0.f, y = 0.f, z = 0.f;

    if (std::getline(ss, val, ',')) x = std::stof(val);
    if (std::getline(ss, val, ',')) y = std::stof(val);
    if (std::getline(ss, val, ',')) z = std::stof(val);

    return glm::vec3(x, y, z);
}

constexpr const char* MAIN_SHADER = "MainShader";

using PropertyMap = std::unordered_map<std::string, std::string>;