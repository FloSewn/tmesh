##############################################################
# MODULE: LEARN C THE HARD WAY
##############################################################
# Define directories
set( TRIMESH_DIR    ${SRC}/trimesh         )
set( TRIMESH_INC    ${TRIMESH_DIR}/include )
set( TRIMESH_SRC    ${TRIMESH_DIR}/src     )

# Define source files
set( TRIMESH_MAIN
  ${TRIMESH_SRC}/tmNode.c
  ${TRIMESH_SRC}/tmEdge.c
  ${TRIMESH_SRC}/tmTri.c
  ${TRIMESH_SRC}/tmMesh.c
  ${TRIMESH_SRC}/tmQtree.c
  )

##############################################################
# LIBRARY: trimesh
##############################################################
# define name
set( TRIMESH_LIB trimesh )

# add sources to library
add_library( ${TRIMESH_LIB} STATIC ${TRIMESH_MAIN} )

# set public include
target_include_directories( ${TRIMESH_LIB} PUBLIC
  $<BUILD_INTERFACE:${TRIMESH_INC}>
  PRIVATE ${TRIMESH_SRC}
)

target_link_libraries( ${TRIMESH_LIB}
  lcthw
  m
)

install( TARGETS trimesh DESTINATION ${LIB} )


##############################################################
# TESTS: trimesh
##############################################################
set( TESTEXE_TRIMESH trimesh_test        )
set( TESTDIR_TRIMESH ${TRIMESH_DIR}/test )

add_executable( ${TESTEXE_TRIMESH}
  ${TESTDIR_TRIMESH}/trimesh_tests.c
  ${TESTDIR_TRIMESH}/geom_tests.c
  ${TESTDIR_TRIMESH}/unit_tests.c
)

target_include_directories( ${TESTEXE_TRIMESH} PUBLIC
  $<BUILD_INTERFACE:${INC_TRIMESH}>
)

target_link_libraries( ${TESTEXE_TRIMESH}
  lcthw
  trimesh
  m
)

# Install executables
install( TARGETS ${TESTEXE_TRIMESH} RUNTIME DESTINATION ${BIN} )



