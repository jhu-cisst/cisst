

#add_subdirectory( trajectories )


add_subdirectory( osg )
add_subdirectory( ode )

if( CISST_DEV_HAS_GLUT )
  add_subdirectory( glut )
endif( CISST_DEV_HAS_GLUT )

if( CISST_DEV_HAS_RTSOCKETCAN )
  add_subdirectory( wam )
endif( CISST_DEV_HAS_RTSOCKETCAN )