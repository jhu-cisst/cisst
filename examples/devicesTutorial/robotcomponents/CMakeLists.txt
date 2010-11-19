

add_subdirectory( trajectories )

if( CISST_DEV_HAS_ODE )

  add_subdirectory( ode )
  
endif( CISST_DEV_HAS_ODE )

if( CISST_DEV_HAS_GLUT )
  add_subdirectory( glut )
endif( CISST_DEV_HAS_GLUT )

if( CISST_DEV_HAS_RTSOCKETCAN )
  add_subdirectory( wam )
endif( CISST_DEV_HAS_RTSOCKETCAN )