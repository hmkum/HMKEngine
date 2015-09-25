TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -Wall -std=c++11

INCLUDEPATH += HMK/src
INCLUDEPATH += HMK/external/assimp/
INCLUDEPATH += HMK/external/gl3w/include/
INCLUDEPATH += HMK/external/glfw3.1-32/include/
INCLUDEPATH += HMK/external/glm/
INCLUDEPATH += HMK/external/imgui-1.45/
INCLUDEPATH += HMK/external/stb/
INCLUDEPATH += HMK/external/

LIBS += -lGL -ldl -lGLU
LIBS += -L$$PWD/HMK/external/glfw3.1-32/ -lglfw3
LIBS += -L$$PWD/HMK/external/assimp/lib/ -lassimp
LIBS += -lX11 -lXxf86vm -lXrandr -pthread -lXi -lXcursor -lXinerama

HEADERS += HMK/src/*.h
SOURCES += HMK/src/Main.cpp \
           HMK/src/Camera.cpp \
           HMK/src/DrawableTexture.cpp \
           HMK/src/Game.cpp \
           HMK/src/KeyManager.cpp \
           HMK/src/Mesh.cpp \
           HMK/src/Model.cpp \
           HMK/src/PostProcess.cpp \
           HMK/src/Ray.cpp \
           HMK/src/Shader.cpp \
           HMK/src/ShaderProgram.cpp \
           HMK/src/ShadowMap.cpp \
           HMK/src/Skybox.cpp \
           HMK/src/Texture.cpp \
           HMK/external/gl3w/src/gl3w.c \
           HMK/external/stb/stb_image.c \
           HMK/external/imgui-1.45/imgui/imgui.cpp \
           HMK/external/imgui-1.45/imgui/imgui_draw.cpp \
           HMK/external/imgui-1.45/imgui/imgui_impl_gl3.cpp

