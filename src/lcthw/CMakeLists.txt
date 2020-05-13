##############################################################
# MODULE: LEARN C THE HARD WAY
##############################################################
# Define directories
set( LCTHW_DIR    ${SRC}/lcthw         )
set( LCTHW_INC    ${LCTHW_DIR}/include )
set( LCTHW_SRC    ${LCTHW_DIR}/src     )

# Define relative module path
#string( LENGTH    ${LCTHW_INC} LCTHW_INC_LEN )
#string( SUBSTRING ${LCTHW_INC} ${CMAKE_SOURCE_DIR_LEN} ${LCTHW_INC_LEN} LCTHW_INC_REL )
#set( LCTHW_INC_REL   ${LCTHW_INC_REL} PARENT_SCOPE )

# Define source files
set( LCTHW_MAIN
  ${LCTHW_SRC}/bstrlib.c
  ${LCTHW_SRC}/list.c
  ${LCTHW_SRC}/darray.c
  ${LCTHW_SRC}/list_algos.c
  ${LCTHW_SRC}/darray_algos.c
  ${LCTHW_SRC}/txtio.c
  )

##############################################################
# LIBRARY: lcthw
##############################################################
# define name
set( LCTHW_LIB lcthw )

# add sources to library
add_library( ${LCTHW_LIB} STATIC ${LCTHW_MAIN} )

# set public include
target_include_directories( ${LCTHW_LIB} PUBLIC
  $<BUILD_INTERFACE:${LCTHW_INC}>
  PRIVATE ${LCTHW_SRC}
)
install( TARGETS lcthw DESTINATION ${LIB} )


##############################################################
# TESTS: lcthw
##############################################################
set( TESTEXE_LCTHW lcthw_test        )
set( TESTDIR_LCTHW ${LCTHW_DIR}/test )

add_executable( ${TESTEXE_LCTHW}
  ${TESTDIR_LCTHW}/bstrlib_tests.c
  ${TESTDIR_LCTHW}/list_tests.c
  ${TESTDIR_LCTHW}/darray_tests.c
  ${TESTDIR_LCTHW}/list_algos_tests.c
  ${TESTDIR_LCTHW}/darray_algos_tests.c
  ${TESTDIR_LCTHW}/txtio_tests.c
  ${TESTDIR_LCTHW}/unit_tests.c
)

target_include_directories( ${TESTEXE_LCTHW} PUBLIC
  $<BUILD_INTERFACE:${INC_LCTHW}>
)

target_link_libraries( ${TESTEXE_LCTHW}
  lcthw
)

# Install executables
install( TARGETS ${TESTEXE_LCTHW} RUNTIME DESTINATION ${BIN} )



