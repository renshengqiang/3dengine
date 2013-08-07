include_directories(/usr/include/SDL /usr/local/include/assimp ../include)
link_directories(/usr/local/lib)

add_executable(demo demo.cpp)

target_link_libraries(demo 3dengine SDL)
