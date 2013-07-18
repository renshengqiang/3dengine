PROJNAME:=engine
LIBDIR:=../lib
LINK_FLAGS:=-L/usr/local/lib -L/usr/lib/i386-linux-gnu/ -lSDL -lGL -lGLU -lGLEW -lstdc++ -lm -lMagick++ -lassimp -lfreetype -lpthread -lX11
COMPILE_FLAGS:=-std=c++0x -I/usr/include/SDL -I/usr/include/freetype2 -I/usr/include/ImageMagick -I/usr/local/include/assimp -I../include -D_GNU_SOURCE=1 -D_REENTRANT -DARCH_X86=1 -Wno-int-to-pointer-cast
COMPILE_FILES:=	util/Timer.c\
				math/Vector.cpp \
				math/Quaternion.cpp \
				math/Matrix.cpp \
				math/Plane.cpp \
				math/AxisAlignedBox.cpp \
				math/Frustum.cpp \
				math/Ray.cpp \
				RenderWindow.cpp \
				MoveableObject.cpp \
				Node.cpp \
				SceneManager.cpp \
				Camera.cpp \
				SceneNode.cpp \
				RaySceneQuery.cpp \
				SimpleSpline.cpp \
				RotationalSpline.cpp \
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
				Octree.cpp \
				OctreeCamera.cpp \
				OctreeNode.cpp \
				OctreeSceneManager.cpp \
				OctreeRaySceneQuery.cpp \
				effect/Effect.cpp \
				effect/SimpleMeshEffect.cpp \
				effect/ParticleEffect.cpp \
				particle/ParticleSystem.cpp \
				FreeType.cpp
include ../Makefile-so
