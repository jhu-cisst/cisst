
if (WIN32)
  
  find_path( ODE_INCLUDE_DIR ode/ode.h 
    PATHS ENV ODE_ROOT_PATH PATH_SUFFIXES include )

  find_library( ODE_LIBRARY NAMES ode ode_double 
    PATHS ENV ODE_ROOT_PATH PATH_SUFFIXES lib )

else (WIN32)

  if (APPLE)


  else (APPLE)

    find_path( ODE_INCLUDE_DIR ode/ode.h )
    find_library( ODE_LIBRARY ode )

  endif (APPLE)

endif (WIN32)

set( ODE_FOUND FALSE )

if( ODE_INCLUDE_DIR ) 
  if( ODE_LIBRARY )
    set( ODE_FOUND TRUE )
  endif( ODE_LIBRARY )
endif( ODE_INCLUDE_DIR )

mark_as_advanced( ODE_INCLUDE_DIR ODE_LIBRARY )



