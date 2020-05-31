cmake_minimum_required(VERSION 3.2)
project(boiler-gltf CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)

set(SOURCE_FILES
  src/gltf.cpp
  src/modelaccessors.cpp)

set(HEADER_FILES
  src/gltf.h
  src/modelaccessor.h
  src/typedaccessor.h)

add_library(boiler-gltf STATIC ${SOURCE_FILES})

target_include_directories(boiler-gltf PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external>
)

install(TARGETS boiler-gltf
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/boiler-gltf)
