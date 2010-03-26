
set( ODE_SEARCH_PATH /usr /usr/local )

find_path( ODE_DIR include/ode/ode.h ${ODE_SEARCH_PATH} )

if( ODE_DIR ) 

  set( ODE_INCLUDE_DIR ${ODE_DIR}/include )
  set( ODE_FOUND true )

  find_library( ODE_LIBRARIES ode ${ODE_DIR}/lib)

endif( ODE_DIR )


