cmake_minimum_required(VERSION 3.20)
project(ModLoader)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# find_package(glm REQUIRED)
# find_package(imgui REQUIRED)
# find_package(minhook REQUIRED)


# file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/*.cpp")
# list(FILTER SOURCES EXCLUDE REGEX "${CMAKE_SOURCE_DIR}/build/.*")

# # 将资源文件加入编译列表
# list(APPEND SOURCES 
#     "${CMAKE_SOURCE_DIR}/resources.rc"
#     # ... 其他源文件
# )

# add_library(ModLoader SHARED ${SOURCES})

# target_include_directories(ModLoader PRIVATE
#     ${CMAKE_SOURCE_DIR}/src
#     ${CMAKE_SOURCE_DIR}/core
#     ${CMAKE_SOURCE_DIR}/trainer
#     ${CMAKE_SOURCE_DIR}/games/AssaultCube
#     ${CMAKE_SOURCE_DIR}/third_party/Imgui
# )

# target_link_libraries(ModLoader PRIVATE
#     glm::glm
#     imgui::imgui
#     minhook::minhook
#     opengl32
# )

add_subdirectory(Injector)
add_subdirectory(Loader)
add_subdirectory(Mods)
# target_link_libraries(ModLoader PRIVATE
#     Injector
#     ModLoader
# )

# target_link_libraries(ModLoader PRIVATE
#     Injector
#     ModLoader
#     ${CMAKE_SOURCE_DIR}/third_party/Imgui/imgui
#     ${CMAKE_SOURCE_DIR}/third_party/Imgui/imgui_impl_dx11

# target_compile_options(ModLoader PRIVATE
#     $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
# )