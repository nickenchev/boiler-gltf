cmake_minimum_required(VERSION 3.5)
project(boiler-gltf CXX)

set(SOURCE_FILES
  src/gltf.cpp
  src/modelaccessors.cpp)

set(HEADER_FILES
  src/gltf.h
  src/modelaccessor.h
  src/typedaccessor.h)

add_library(boiler-gltf ${SOURCE_FILES})

target_include_directories(boiler-gltf
  PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external>
)
