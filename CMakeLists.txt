cmake_minimum_required(VERSION 3.12.0)

project(eustia)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")

find_package(Eustia REQUIRED)

add_subdirectory()