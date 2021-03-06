CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0)

PROJECT(nucleus-glfw LANGUAGES C VERSION 0.0.1)

SET(CMAKE_BUILD_TYPE Debug)
#SET(CMAKE_BUILD_TYPE Release)

SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall")

ADD_DEFINITIONS(-DGLEW_STATIC)

INCLUDE_DIRECTORIES(
    ../../../extlibs/glfw/include/
    ../../../extlibs/glew/include/
)

FILE(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

ADD_LIBRARY(
    ${PROJECT_NAME} SHARED
    ../../../extlibs/glew/src/glew.c
    ${sources}
)

FIND_PACKAGE(OpenGL)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    mingw32
    nucleus
    "${OPENGL_gl_LIBRARY}"
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../extlibs/glfw/lib/libglfw3.a
)

INSTALL(
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/
    ARCHIVE DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/
    RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../../bin/engine/system/
)
