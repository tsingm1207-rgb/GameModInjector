
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
list(FILTER SOURCES EXCLUDE REGEX "${CMAKE_SOURCE_DIR}/build/.*")

# # 将资源文件加入编译列表
list(APPEND SOURCES 
    "${CMAKE_SOURCE_DIR}/Shared/resources.rc"
    # ... 其他源文件
)

list(APPEND SOURCES
    "${CMAKE_SOURCE_DIR}/Shared/third_party/Imgui/imgui_impl_opengl3.cpp"
    "${CMAKE_SOURCE_DIR}/Shared/third_party/Imgui/imgui_impl_win32.cpp"
)

find_package(glm REQUIRED)
find_package(imgui REQUIRED)
find_package(minhook REQUIRED)

add_library(Loader SHARED ${SOURCES})

target_link_libraries(Loader PRIVATE
    glm::glm
    imgui::imgui
    minhook::minhook
    opengl32
)
target_include_directories(Loader PRIVATE
    ${CMAKE_SOURCE_DIR}/Shared/third_party/Imgui
)

target_compile_options(Loader PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)