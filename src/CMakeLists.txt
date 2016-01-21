
set( NAME "AFMulator" )

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11" )

add_executable( ${NAME}.x 
    main.cpp
)

target_link_libraries( ${NAME}.x ${OpenGL_LIBRARIES} ${GLU_LIBRARY} ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY} )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR} )

#file( COPY ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.x DESTINATION ${CMAKE_SOURCE_DIR} )
