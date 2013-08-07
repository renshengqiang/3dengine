include_directories(/usr/include/SDL /usr/include/ImageMagick /usr/local/include/assimp /usr/include/freetype2 ../include)
link_directories(/usr/local/lib /usr/lib/i386-linux-gnu)

set(3DENGINE_SRCS
        util/Timer.c
        math/Vector.cpp 
        math/Quaternion.cpp 
        math/Matrix.cpp 
        math/Plane.cpp 
        math/AxisAlignedBox.cpp 
        math/Frustum.cpp 
        math/Ray.cpp 
        RenderWindow.cpp 
        MoveableObject.cpp 
        Node.cpp 
        SceneManager.cpp 
        Camera.cpp 
        SceneNode.cpp 
        RaySceneQuery.cpp 
        SimpleSpline.cpp 
        RotationalSpline.cpp 
        Texture.cpp 
        Render.cpp 
        SubMesh.cpp 
        Mesh.cpp 
        AnimationState.cpp 
        Animation.cpp 
        AnimationTrack.cpp 
        KeyFrame.cpp 
        Skeleton.cpp 
        SkeletonBone.cpp 
        SkeletonNodeTrack.cpp 
        SkeletonAnimation.cpp 
        Entity.cpp 
        MeshManager.cpp 
        Octree.cpp 
        OctreeCamera.cpp 
        OctreeNode.cpp 
        OctreeSceneManager.cpp 
        OctreeRaySceneQuery.cpp 
        effect/Effect.cpp 
        effect/SimpleMeshEffect.cpp 
        effect/ParticleEffect.cpp 
        effect/EffectManager.cpp 
        particle/ParticleSystem.cpp 
        FreeType.cpp
        )

add_library(engine SHARED ${3DENGINE_SRCS})
add_library(engine_static STATIC ${3DENGINE_SRCS})
set_target_properties(engine PROPERTIES VERSION 0.0 SOVERSION 0)
set_target_properties(engine_static PROPERTIES OUTPUT_NAME "3dengine")

#install(TARGETS 3dengine DESTINATION lib)
target_link_libraries(engine GL GLU GLEW SDL stdc++ m Magick++ assimp freetype pthread X11)
