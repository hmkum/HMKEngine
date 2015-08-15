
# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#

# define the C++ compiler to use
CC = clang++

# define any compile-time flags
CFLAGS = -Wall -std=c++11 -g

# define any directories containing header files other than /usr/include
#
INCLUDES = -IHMK/external/glm -IHMK/external/gl3w/include -IHMK/external -IHMK/external/glfw3.1-32/include

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS = -LHMK/external/glfw3.1-32/ -LHMK/external/assimp/lib

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
LIBS = -lGL -ldl -lGLU -lglfw3 -lassimp -lX11 -lXxf86vm -lXrandr -pthread -lXi -lXcursor -lXinerama

# define the C++ source files
SRCS = HMK/src/Main.cpp HMK/src/Camera.cpp HMK/src/Game.cpp HMK/src/KeyManager.cpp HMK/src/Mesh.cpp HMK/src/Model.cpp HMK/src/Shader.cpp HMK/src/ShaderProgram.cpp HMK/src/Skybox.cpp HMK/src/Texture.cpp HMK/external/gl3w/src/gl3w.c HMK/external/stb/stb_image.c

# define the C++ object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .cpp of all words in the macro SRCS
# with the .o suffix
#
OBJS = $(SRCS:.cpp=.o)

# define the executable file 
MAIN = HMK/HMK

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean

all:    $(MAIN)
		@echo  HMK binary olusturuldu.

$(MAIN): $(OBJS) 
		$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .cpp file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.cpp.o:
		$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
		$(RM) HMK/src/*.o HMK/external/stb/*.o HMK/external/gl3w/src/*.o *~ $(MAIN)

run:
		(cd HMK/ && exec ./HMK)

depend: $(SRCS)
		makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
