cmake_minimum_required(VERSION 3.8.2 FATAL_ERROR)

project(eustia)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")

find_package(Eustia REQUIRED)

add_subdirectory("${PROJECT_SOURCE_DIR}/src/Injector" "Injector")
#add_subdirectory("${PROJECT_SOURCE_DIR}/src/Loader" "Loader")