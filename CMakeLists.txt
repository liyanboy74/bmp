cmake_minimum_required(VERSION 3.6)

project(bmp)

add_library(${PROJECT_NAME} STATIC bmp.c)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES bmp.h DESTINATION include)
