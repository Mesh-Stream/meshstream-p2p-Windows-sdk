cmake_minimum_required(VERSION 3.21)
project(MeshStreamConsoleDemo LANGUAGES CXX)

set(MESHSTREAM_SDK_ROOT "" CACHE PATH "Root of the MeshStream Windows SDK package")
if(NOT MESHSTREAM_SDK_ROOT)
    message(FATAL_ERROR "Set -DMESHSTREAM_SDK_ROOT=<MeshStream SDK package directory>")
endif()

set(MESHSTREAM_INCLUDE_DIR "${MESHSTREAM_SDK_ROOT}/include")
set(MESHSTREAM_IMPORT_LIBRARY "${MESHSTREAM_SDK_ROOT}/lib/p2pcdn_core.dll.lib")
set(MESHSTREAM_RUNTIME_DLL "${MESHSTREAM_SDK_ROOT}/lib/p2pcdn_core.dll")
foreach(required_path MESHSTREAM_INCLUDE_DIR MESHSTREAM_IMPORT_LIBRARY MESHSTREAM_RUNTIME_DLL)
    if(NOT EXISTS "${${required_path}}")
        message(FATAL_ERROR "Missing ${required_path}: ${${required_path}}")
    endif()
endforeach()

add_executable(MeshStreamConsoleDemo src/main.cpp)
target_compile_features(MeshStreamConsoleDemo PRIVATE cxx_std_17)
target_include_directories(MeshStreamConsoleDemo PRIVATE "${MESHSTREAM_INCLUDE_DIR}")
target_link_libraries(MeshStreamConsoleDemo PRIVATE "${MESHSTREAM_IMPORT_LIBRARY}")
add_custom_command(TARGET MeshStreamConsoleDemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MESHSTREAM_RUNTIME_DLL}"
        "$<TARGET_FILE_DIR:MeshStreamConsoleDemo>")
