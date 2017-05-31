cmake_minimum_required(VERSION 2.8.0)

project(curlpp)

#i want to install lib and include in this dir for some reasons
#set (CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/install)
set (CMAKE_INSTALL_PREFIX /tmp/install/)

add_subdirectory(src)