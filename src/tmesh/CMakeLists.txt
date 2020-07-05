##############################################################
# MODULE: LEARN C THE HARD WAY
##############################################################
# Define directories
set( TMESH_DIR    ${SRC}/tmesh         )
set( TMESH_INC    ${TMESH_DIR}/include )
set( TMESH_SRC    ${TMESH_DIR}/src     )

# Define source files
set( TMESH_MAIN
  ${TMESH_SRC}/tmNode.c
  ${TMESH_SRC}/tmEdge.c
  ${TMESH_SRC}/tmTri.c
  ${TMESH_SRC}/tmBdry.c
  ${TMESH_SRC}/tmFront.c
  ${TMESH_SRC}/tmMesh.c
  ${TMESH_SRC}/tmQtree.c
  )

##############################################################
# LIBRARY: tmesh
##############################################################
# define name
set( TMESH_LIB tmesh )

# add sources to library
add_library( ${TMESH_LIB} STATIC ${TMESH_MAIN} )

# set public include
target_include_directories( ${TMESH_LIB} PUBLIC
  $<BUILD_INTERFACE:${TMESH_INC}>
  PRIVATE ${TMESH_SRC}
)

target_link_libraries( ${TMESH_LIB}
  lcthw
  m
)

install( TARGETS tmesh DESTINATION ${LIB} )


##############################################################
# TESTS: tmesh
##############################################################
set( TESTEXE_TMESH tmesh_test        )
set( TESTDIR_TMESH ${TMESH_DIR}/test )

add_executable( ${TESTEXE_TMESH}
  ${TESTDIR_TMESH}/tmesh_tests.c
  ${TESTDIR_TMESH}/geom_tests.c
  ${TESTDIR_TMESH}/unit_tests.c
)

target_include_directories( ${TESTEXE_TMESH} PUBLIC
  $<BUILD_INTERFACE:${INC_TMESH}>
)

target_link_libraries( ${TESTEXE_TMESH}
  lcthw
  tmesh
  m
)

# Install executables
install( TARGETS ${TESTEXE_TMESH} RUNTIME DESTINATION ${BIN} )



