##############################################################
#                      LEARN C THE HARD WAY                  #
##############################################################
# prerequisites
cmake_minimum_required( VERSION 3.5.2 )
project( Triangular-Mesh-Generator )

# enable testing support
enable_testing()

# Include cmake files
include( "${CMAKE_SOURCE_DIR}/share/cmake/directories.cmake" )

#-------------------------------------------------------------
# Define modules
#-------------------------------------------------------------
add_subdirectory( ${SRC}/lcthw )
add_subdirectory( ${SRC}/trimesh )
