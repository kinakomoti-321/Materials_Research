add_subdirectory(glm)

add_library(tinygltf INTERFACE)
target_include_directories(tinygltf SYSTEM INTERFACE "./tinygltf")

add_library(spdlog INTERFACE)
target_include_directories(spdlog SYSTEM INTERFACE "./spdlog/include")

add_library(stb INTERFACE)
target_include_directories(stb SYSTEM INTERFACE stb)

add_library(tinyobj INTERFACE)
target_include_directories(tinyobj SYSTEM INTERFACE "./tinyobjloader")

add_library(json INTERFACE)
target_include_directories(json SYSTEM INTERFACE "./json/include")