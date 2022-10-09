cmake_minimum_required(VERSION 3.20)
project(FBX_Wrapper)

set(CMAKE_CXX_STANDARD 17)
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/vs2019/x64/release")

add_library(FBX_Wrapper SHARED
        library.cpp
        SMStructs.h
        FbxDataConverter.h
        UEStructs.h
        GenerateSmoothingGroups.cpp
        GenerateSmoothingGroups.h SKStructs.h)
add_library(libfbxsdk SHARED IMPORTED)
set_target_properties(libfbxsdk PROPERTIES IMPORTED_LOCATION "${LIB_DIR}/libfbxsdk.lib")
set_target_properties(libfbxsdk PROPERTIES IMPORTED_IMPLIB "${LIB_DIR}/libfbxsdk.dll")

include_directories(include)

link_directories(${LIB_DIR}/libfbxsdk.lib)

target_link_libraries(FBX_Wrapper PRIVATE "${LIB_DIR}/libfbxsdk.lib")

add_compile_definitions(FBXLIBRARY_EXPORTS)