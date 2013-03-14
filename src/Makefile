PROJNAME:=engine
LIBDIR:=../lib
LINK_FLAGS:=-L/usr/local/lib -L/usr/lib/i386-linux-gnu/ -lSDL -lGL -lGLU -lGLEW -lstdc++ -lMagick++ -lassimp -lfreetype
COMPILE_FLAGS:=-std=c++0x -I/usr/include/SDL -I/usr/include/freetype2 -I/usr/include/ImageMagick -I/usr/local/include/assimp -I../include -D_GNU_SOURCE=1 -D_REENTRANT -DARCH_X86=1 -Wno-int-to-pointer-cast
COMPILE_FILES:= util/Array.c\
				util/Timer.c\
				math/Vector.cpp \
				math/Quaternion.cpp \
				math/Matrix.cpp \
				SceneManager.cpp \
				RenderWindow.cpp \
				Camera.cpp \
				Node.cpp \
				SceneNode.cpp \
				Shader.cpp \
				Texture.cpp \
				Render.cpp \
				SubMesh.cpp \
				Mesh.cpp \
				AnimationState.cpp \
				Animation.cpp \
				AnimationTrack.cpp \
				KeyFrame.cpp \
				Skeleton.cpp \
				SkeletonBone.cpp \
				SkeletonNodeTrack.cpp \
				SkeletonAnimation.cpp \
				Entity.cpp \
				MeshManager.cpp \
				FreeType.cpp
include ../Makefile-so
