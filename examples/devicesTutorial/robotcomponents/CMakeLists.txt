

add_subdirectory( trajectories )

if( CISST_DEV_HAS_ODE )

  add_subdirectory( wamode )
  
endif( CISST_DEV_HAS_ODE )

if( CISST_DEV_HAS_GLUT )

  add_subdirectory( glut )
  
endif( CISST_DEV_HAS_GLUT )
