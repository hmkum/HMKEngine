#pragma once

#include <functional>
#include "Logger.h"
#include "Debug.h"

#define SAFE_RELEASE(v) {if(v != nullptr) {delete v; v = nullptr;}}

#define HMK_LOG_WRITE(log, ...)   {hmk::Logger::Inst().Write("<DEBUG>   ", log, " ", ##__VA_ARGS__, "\n");}
#define HMK_LOG_WARNING(log, ...) {hmk::Logger::Inst().Write("<WARNING> ", log, " ", ##__VA_ARGS__, "\n");}
#define HMK_LOG_ERROR(log, ...)   {hmk::Logger::Inst().Write("<ERROR>   ", log, " ", ##__VA_ARGS__, "\n");}

#define HMK_PRINT(d, ...) {hmk::Debug::Print(d, __VA_ARGS__, "\n");}

// C++11 based callback macros
#define HMK_CALLBACK_1(SELECTOR, TARGET) std::bind(&SELECTOR, TARGET, std::placeholders::_1);
#define HMK_CALLBACK_2(SELECTOR, TARGET) std::bind(&SELECTOR, TARGET, std::placeholders::_1, std::placeholders::_2);
#define HMK_CALLBACK_3(SELECTOR, TARGET) std::bind(&SELECTOR, TARGET, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
#define HMK_CALLBACK_4(SELECTOR, TARGET) std::bind(&SELECTOR, TARGET, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

// Data paths
const std::string SKYBOX_TEXTURE_PATH = "data/textures/Cubemaps/";
const std::string SHADER_PATH = "data/shaders/";
const std::string MODEL_PATH = "data/models/";
const std::string TEXTURE_PATH = "data/textures/";