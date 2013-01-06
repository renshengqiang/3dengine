PROJNAME:=engine
LIBDIR:=../lib
LINK_FLAGS:=-lSDL -lGL -lGLU -lGLEW -lstdc++ -lMagick++ -lassimp
COMPILE_FLAGS:=-I/usr/include/SDL -I/usr/include/ImageMagick -I/usr/local/include/assimp -I../include -D_GNU_SOURCE=1 -D_REENTRANT 
COMPILE_FILES:= math_3d.cpp \
				SceneManager.cpp \
				RenderWindow.cpp \
				Camera.cpp \
				MovableObject.cpp \
				SceneNode.cpp \
				Shader.cpp \
				Texture.cpp \
				Render.cpp \
				Mesh.cpp
include ../Makefile-so
