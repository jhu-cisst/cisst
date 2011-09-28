#include <cisstOSG/cisstOSGPick.h>

#include <cisstCommon/cmnLogger.h>

cisstOSGPick::cisstOSGPick() : osgGA::GUIEventHandler(), mousex(0), mousey(0){}


bool cisstOSGPick::handle( const osgGA::GUIEventAdapter& ea, 
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
bool cisstOSGPick::Pick( double x, double y, osgViewer::Viewer* viewer ){

  // Nothing to pick.
  if( !viewer->getSceneData() )
    { return false; }

  double w( .05 ), h( .05 ); // tolerance

  // create a polytope picker
  osg::ref_ptr< osgUtil::PolytopeIntersector> picker;
  picker = new osgUtil::PolytopeIntersector( osgUtil::Intersector::PROJECTION,
					     x-w, y-h, x+w, y+h );
  osgUtil::IntersectionVisitor iv( picker );
  viewer->getCamera()->accept( iv );

  // something was picked
  if( picker->containsIntersections() ){

    // get the first intersection
    osgUtil::PolytopeIntersector::Intersections& 
      intersections = picker->getIntersections();
    osgUtil::PolytopeIntersector::Intersections::const_iterator 
      intersection = intersections.begin();

    // get the node path
    const osg::NodePath& nodePath = intersection->nodePath;
    unsigned int idx = nodePath.size();
    
    // go up the path searching for a body
    while(idx--){
      osg::ref_ptr<cisstOSGBody> body;
      body = dynamic_cast< cisstOSGBody* >( nodePath[ idx ] );
      
      if( body.get() != NULL ){
	PickHandler( body.get() );	
	break;
      }
    }
    
    // useful code to extract all bodies
    /*
    for( intersection =intersections.begin(); 
	 intersection!=intersections.end();
	 intersection++ ){
      const osg::NodePath& nodePath = intersection->nodePath;

      unsigned int idx = nodePath.size();

      while (idx--){
	cisstOSGBody* body = NULL;
	body = dynamic_cast< cisstOSGBody* >( nodePath[ idx ] );

	if( body == NULL )
	  { continue; }
	bodies.push_back( body );
      }
    }
    
    bodies.unique();

    std::list<cisstOSGBody*>::iterator body;
    for( body=bodies.begin(); body!=bodies.end(); body++ ){
      std::cout << (*body)->getName() << std::endl;
    }
    */

  }
  return true;
}
