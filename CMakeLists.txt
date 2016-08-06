cmake_minimum_required(VERSION 2.6)
project("lily-dis")

include_directories("${CMAKE_INSTALL_PREFIX}/lily/")

add_library(dis SHARED lily_dis.c)
set_target_properties(
    dis
    PROPERTIES
        PREFIX ""
)
