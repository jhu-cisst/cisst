

#add_subdirectory( trajectories )

if( CISST_DEV_HAS_ODE )
  add_subdirectory( ode )
endif( CISST_DEV_HAS_ODE )

include( FindOpenSceneGraph )
if( OPENSCENEGRAPH_FOUND )
  add_subdirectory( osg )
endif( OPENSCENEGRAPH_FOUND )

if( CISST_DEV_HAS_GLUT )
  add_subdirectory( glut )
endif( CISST_DEV_HAS_GLUT )

if( CISST_DEV_HAS_RTSOCKETCAN )
  add_subdirectory( wam )
endif( CISST_DEV_HAS_RTSOCKETCAN )