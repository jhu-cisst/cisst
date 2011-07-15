

#add_subdirectory( trajectories )

find_package( OpenSceneGraph COMPONENTS osgDB osgGA osgViewer )
if( OPENSCENEGRAPH_FOUND )

  add_subdirectory( osg )

  find_package( ODE )
  if( ODE_FOUND )
    add_subdirectory( ode )
  endif( ODE_FOUND )

endif( OPENSCENEGRAPH_FOUND )

if( CISST_DEV_HAS_GLUT )
  add_subdirectory( glut )
endif( CISST_DEV_HAS_GLUT )

if( CISST_DEV_HAS_RTSOCKETCAN )
  add_subdirectory( wam )
endif( CISST_DEV_HAS_RTSOCKETCAN )