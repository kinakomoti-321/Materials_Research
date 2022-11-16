cmake_minimum_required(VERSION 3.12)
project(main LANGUAGES C CXX)

find_package(embree 3.0 REQUIRED)
add_executable(MR "src/embree_scene_test.cpp")

add_subdirectory(external)

target_include_directories(MR PUBLIC "include") 

target_link_libraries(MR PUBLIC stb) 
target_link_libraries(MR PUBLIC glm) 
target_link_libraries(MR PUBLIC spdlog) 
target_link_libraries(MR PUBLIC tinygltf) 
target_link_libraries(MR PUBLIC tinyobj) 
target_link_libraries(MR PUBLIC embree) 
target_compile_features(MR PRIVATE cxx_std_17)