cmake_minimum_required ( VERSION 2.8 )
project ( AFMulator )

# ========================= General Preparation


#SET( GPPFLAGS "-std=c++11 -Og -g")
#SET( GPPFLAGS "-std=c++11 -Og -g -Wall")
SET( GPPFLAGS "-std=c++11 -Og -g -w")
#SET( GPPFLAGS "-std=c++11 -O3 -ftree-vectorize -unroll-loops -w")
SET( CMAKE_CXX_FLAGS  "${AXULIARY_COMPILE_FLAGS} ${GPPFLAGS}" )

set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/cmake_utils" )

find_package( OpenGL REQUIRED )
find_package( GLU REQUIRED    )
find_package( SDL2 REQUIRED   )
find_package( SDL2_ttf REQUIRED   )

#SET( COMMON_SRCS "${CMAKE_SOURCE_DIR}/common" )
include_directories(
        ${CMAKE_SOURCE_DIR}/src
        ${OpenGL_INCLUDE_DIR}
        ${GLU_INCLUDE_DIR}
        ${SDL2_INCLUDE_DIR}
        ${SDL2_TTF_INCLUDE_DIR}
)

add_subdirectory (src)


