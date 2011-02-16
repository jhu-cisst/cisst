#include <osg/Geometry>
#include <osg/Geode>

#include <cisstCommon/cmnLogger.h>
#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>

#include <iostream>

devOSGWorld::devOSGWorld(){

  /*
  osg::ref_ptr<osg::Geometry> linesGeom;
  try{ linesGeom = new osg::Geometry; }
  catch( std::bad_alloc& ){ 
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to create a geometry." 
		      << std::endl;
  }
  
  osg::ref_ptr<osg::DrawArrays> drawArrayLines;
  try{ drawArrayLines = new osg::DrawArrays( osg::PrimitiveSet::LINES ); }
  catch( std::bad_alloc& ){ 
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to create array of lines." 
		      << std::endl;
  }
  linesGeom->addPrimitiveSet(drawArrayLines);

  osg::ref_ptr<osg::Vec3Array> vertexData;
  try{ vertexData = new osg::Vec3Array; }
  catch( std::bad_alloc& ){ 
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to create vertices." 
		      << std::endl;
  }
  linesGeom->setVertexArray(vertexData);


  for( int i=-10; i<=10; i++ ){
    vertexData->push_back(osg::Vec3(-10, i, 0));
    vertexData->push_back(osg::Vec3( 10, i, 0));
    vertexData->push_back(osg::Vec3( i, -10, 0));
    vertexData->push_back(osg::Vec3( i,  10, 0));
  }

  drawArrayLines->setFirst(0);
  drawArrayLines->setCount(vertexData->size());

  osg::Geometry*   Xgeom     = new osg::Geometry();
  osg::DrawArrays* Xdraw     = new osg::DrawArrays( osg::PrimitiveSet::LINES );
  osg::Vec3Array*  Xvertices = new osg::Vec3Array(2);
  osg::Vec4Array*  Xred      = new osg::Vec4Array;

  (*Xvertices)[0].set( 0.0, 0.0, 0.0 );
  (*Xvertices)[1].set( 1.0, 0.0, 0.0 );
  Xred->push_back( osg::Vec4( 1.0f, 0.0f, 0.0f, 1.0f ) );

  Xgeom->addPrimitiveSet( Xdraw );
  Xgeom->setVertexArray( Xvertices );
  Xgeom->setColorArray( Xred );
  Xgeom->setColorBinding( osg::Geometry::BIND_OVERALL );

  Xdraw->setFirst( 0 );
  Xdraw->setCount( Xvertices->size() );

  osg::Geometry*   Ygeom     = new osg::Geometry();
  osg::DrawArrays* Ydraw     = new osg::DrawArrays( osg::PrimitiveSet::LINES );
  osg::Vec3Array*  Yvertices = new osg::Vec3Array(2);
  osg::Vec4Array*  Ygreen    = new osg::Vec4Array;

  (*Yvertices)[0].set( 0.0, 0.0, 0.0 );
  (*Yvertices)[1].set( 0.0, 1.0, 0.0 );
  Ygreen->push_back( osg::Vec4( 0.0f, 1.0f, 0.0f, 1.0f ) );

  Ygeom->addPrimitiveSet( Ydraw );
  Ygeom->setVertexArray( Yvertices );
  Ygeom->setColorArray( Ygreen );
  Ygeom->setColorBinding( osg::Geometry::BIND_OVERALL );

  Ydraw->setFirst( 0 );
  Ydraw->setCount( Yvertices->size() );


  osg::Geometry*   Zgeom     = new osg::Geometry();
  osg::DrawArrays* Zdraw     = new osg::DrawArrays( osg::PrimitiveSet::LINES );
  osg::Vec3Array*  Zvertices = new osg::Vec3Array(2);
  osg::Vec4Array*  Zblue     = new osg::Vec4Array;

  (*Zvertices)[0].set( 0.0, 0.0, 0.0 );
  (*Zvertices)[1].set( 0.0, 0.0, 1.0 );
  Zblue->push_back( osg::Vec4( 0.0f, 0.0f, 1.0f, 1.0f ) );

  Zgeom->addPrimitiveSet( Zdraw );
  Zgeom->setVertexArray( Zvertices );
  Zgeom->setColorArray( Zblue );
  Zgeom->setColorBinding( osg::Geometry::BIND_OVERALL );

  Zdraw->setFirst( 0 );
  Zdraw->setCount( Zvertices->size() );



  osg::Geode* geode = new osg::Geode;
  geode->addDrawable( linesGeom );
  geode->addDrawable( Xgeom );
  geode->addDrawable( Ygeom );
  geode->addDrawable( Zgeom );
  
  addChild( geode );
  */

}

