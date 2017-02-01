cmake_minimum_required(VERSION 3.0.0)
project("lily-dis")

include_directories("${CMAKE_INSTALL_PREFIX}/include/lily/")

add_library(dis SHARED lily_dis.c)
set_target_properties(dis PROPERTIES PREFIX "")
