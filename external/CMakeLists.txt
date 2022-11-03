add_subdirectory(glm)

add_subdirectory(tinygltf)

add_library(stb INTERFACE)
target_include_directories(stb SYSTEM INTERFACE stb)