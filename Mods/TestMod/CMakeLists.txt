add_library(TestMod SHARED main.cpp)

target_include_directories(TestMod PRIVATE
    ../../Shared/mod_abi.h
)

target_compile_options(TestMod PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)