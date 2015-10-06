#pragma once
#include <GL/gl3w.h>
#include <functional>
#include <string>
#include "Logger.h"
#include "Debug.h"

#define SAFE_RELEASE(v) {if(v != nullptr) {delete v; v = nullptr;}}

#define HMK_LOG_WRITE(log, ...)   {hmk::Logger::get_instance().write("<DEBUG>   ", log, " ", ##__VA_ARGS__, "\n");}
#define HMK_LOG_WARNING(log, ...) {hmk::Logger::get_instance().write("<WARNING> ", log, " ", ##__VA_ARGS__, "\n");}
#define HMK_LOG_ERROR(log, ...)   {hmk::Logger::get_instance().write("<ERROR>   ", log, " ", ##__VA_ARGS__, "\n");}

#define HMK_PRINT(...) {hmk::Debug::print("", __VA_ARGS__, "\n");}

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

#define CHECK_GL \
{ \
	std::string errStr = "OpenGL Error( " + std::string(__FILE__) + ", " + std::to_string(__LINE__) + "): "; \
	GLenum err = glGetError(); \
	switch (err) \
	{ \
		case GL_INVALID_ENUM: \
		errStr += "GL_INVALID_ENUM"; \
		break; \
		case GL_INVALID_VALUE: \
		errStr += "GL_INVALID_VALUE"; \
		break; \
		case GL_INVALID_OPERATION: \
		errStr += "GL_INVALID_OPERATION"; \
		break; \
		case GL_INVALID_FRAMEBUFFER_OPERATION: \
		errStr += "GL_INVALID_FRAMEBUFFER_OPERATION"; \
		break; \
		case GL_OUT_OF_MEMORY: \
		errStr += "GL_OUT_OF_MEMORY"; \
		break; \
		case GL_STACK_UNDERFLOW: \
		errStr += "GL_STACK_UNDERFLOW"; \
		break; \
		case GL_STACK_OVERFLOW: \
		errStr += "GL_STACK_OVERFLOW"; \
		break; \
		default:\
		errStr = ""; \
		break; \
	} \
	if (errStr != "") HMK_LOG_ERROR(errStr) HMK_PRINT(errStr) \
}
