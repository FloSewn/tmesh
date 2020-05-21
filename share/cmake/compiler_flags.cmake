
################################################################################
# Define Build Type                                                            #
################################################################################

# default build type
if( NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES )

  # set the default build type
  message( STATUS "Setting build type to 'Debug' as none was specified." )
  set( CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE )

  # set the possible values of build type
  set_property( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" )

endif()


################################################################################
# Compiler FLags                                                               #
################################################################################

# set compiler definition
if(     CMAKE_C_COMPILER_ID STREQUAL "GNU"   )
  string( APPEND MY_CMAKE_C_FLAGS   "-DGNU"   )
elseif( CMAKE_C_COMPILER_ID STREQUAL "Intel" )
  string( APPEND MY_CMAKE_C_FLAGS   "-DINTEL" )
endif()

# define coding standards
if(     CMAKE_C_COMPILER_ID STREQUAL "GNU"   )
  string( APPEND MY_CMAKE_C_FLAGS   " -std=c99"   )
elseif( CMAKE_C_COMPILER_ID STREQUAL "Intel" )
  string( APPEND MY_CMAKE_C_FLAGS   " -std=c99"   )
endif()


#################################
# Compiler Flags (Debug Mode)   #
#################################

# set debug compiler flags
if( CMAKE_BUILD_TYPE STREQUAL "Debug" )

  # produce debugging information
  string( APPEND MY_CMAKE_C_FLAGS " -g" )

  # disable optimization
  string( APPEND MY_CMAKE_C_FLAGS " -O0" )

  # disable optimization
  string( APPEND MY_CMAKE_C_FLAGS " -pg" )

  if( CMAKE_C_COMPILER_ID STREQUAL "GNU" )
   
    # detect unused variables
    #string( APPEND MY_CMAKE_C_FLAGS " -Wunused-variable" )
   
  elseif( CMAKE_C_COMPILER_ID STREQUAL "Intel" )
   
    # detect unused variables
    # string( APPEND MY_CMAKE_C_FLAGS " -Wunused-variable" )
   
  endif()

  # overwrite compiler flags
  set(
    CMAKE_C_FLAGS_DEBUG ${MY_CMAKE_C_FLAGS}
    CACHE STRING "Flags used by the compiler during debug builds." FORCE
  )

endif()


#################################
# Compiler Flags (Release Mode) #
#################################

# set release compiler flags
if( CMAKE_BUILD_TYPE STREQUAL "Release" )

  # enable optimization
  string( APPEND MY_CMAKE_C_FLAGS " -O3" )

  if( CMAKE_C_COMPILER_ID STREQUAL "GNU" )
   
    # detect unused variables
    # string( APPEND MY_CMAKE_C_FLAGS " -Wunused-variable" )
   
  elseif( CMAKE_C_COMPILER_ID STREQUAL "Intel" )
   
    # detect unused variables
    # string( APPEND MY_CMAKE_C_FLAGS " -Wunused-variable" )
   
  endif()

  # overwrite compiler flags
  set(
    CMAKE_C_FLAGS_RELEASE ${MY_CMAKE_C_FLAGS}
    CACHE STRING "Flags used by the compiler during debug builds." FORCE
  )

endif()

