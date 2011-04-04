#include <cisstCommon/cmnLogger.h>
#include <cisstDevices/robotcomponents/osg/devOSGPick.h>
#include <cisstDevices/robotcomponents/osg/devOSGBody.h>

devOSGPick::devOSGPick() : osgGA::GUIEventHandler(), mousex(0), mousey(0){}


bool devOSGPick::handle( const osgGA::GUIEventAdapter& ea, 
			 osgGA::GUIActionAdapter& aa ){

  osg::ref_ptr< osgViewer::Viewer > viewer;
  viewer = dynamic_cast<osgViewer::Viewer*>( &aa );
  if( !viewer ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Could not cast GUIActionAdapter as Viewer." 
		      << std::endl;
    return false;
  }

  switch( ea.getEventType() ){

  case osgGA::GUIEventAdapter::PUSH:

  case osgGA::GUIEventAdapter::MOVE:
    {
      // Record mouse location for the button press
      // and move events.
      mousex = ea.getX();
      mousey = ea.getY();
      return false;
    }

  case osgGA::GUIEventAdapter::RELEASE:
    {
      // If the mouse hasn't moved since the last
      // button press or move event, perform a
      // pick. (Otherwise, the trackball
      // manipulator will handle it.)
      if( mousex == ea.getX() && mousey == ea.getY()){
	if( Pick( ea.getXnormalized(), ea.getYnormalized(), viewer ) )
	  { return true; }
      }
      return false;
    }
    
  default:
    return false;
  }

}

// Perform a pick operation.
bool devOSGPick::Pick( double x, double y, osgViewer::Viewer* viewer ){

  // Nothing to pick.
  if( !viewer->getSceneData() )
    { return false; }

  double w( .05 ), h( .05 ); // tolerance

  osg::ref_ptr< osgUtil::PolytopeIntersector> picker;
  picker = new osgUtil::PolytopeIntersector( osgUtil::Intersector::PROJECTION,
					     x-w, y-h, x+w, y+h );
  osgUtil::IntersectionVisitor iv( picker );
  viewer->getCamera()->accept( iv );

  if( picker->containsIntersections() ){

    std::list<devOSGBody*> bodies;

    osgUtil::PolytopeIntersector::Intersections& intersections = picker->getIntersections();
    osgUtil::PolytopeIntersector::Intersections::const_iterator intersection;
    
    for( intersection =intersections.begin(); 
	 intersection!=intersections.end();
	 intersection++ ){
      const osg::NodePath& nodePath = intersection->nodePath;

      unsigned int idx = nodePath.size();

      while (idx--){
	devOSGBody* body = NULL;
	body = dynamic_cast< devOSGBody* >( nodePath[ idx ] );

	if( body == NULL )
	  { continue; }
	bodies.push_back( body );
      }
    }
    
    bodies.unique();
    std::list<devOSGBody*>::iterator body;
    for( body=bodies.begin(); body!=bodies.end(); body++ ){
      std::cout << (*body)->GetName() << std::endl;
    }

  }
  return true;
}
