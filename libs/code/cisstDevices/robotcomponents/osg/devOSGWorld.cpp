#include <osg/Geometry>
#include <osg/Geode>
#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>
devOSGWorld::devOSGWorld(){

  osg::Geometry* linesGeom = new osg::Geometry;

  osg::DrawArrays* drawArrayLines;
  drawArrayLines = new osg::DrawArrays(osg::PrimitiveSet::LINES); 

  linesGeom->addPrimitiveSet(drawArrayLines);
  osg::Vec3Array* vertexData = new osg::Vec3Array;
  linesGeom->setVertexArray(vertexData);

  for( int i=-10; i<=10; i++ ){
    vertexData->push_back(osg::Vec3(-10, i, 0));
    vertexData->push_back(osg::Vec3( 10, i, 0));
    vertexData->push_back(osg::Vec3( i, -10, 0));
    vertexData->push_back(osg::Vec3( i,  10, 0));
  }
  
  drawArrayLines->setFirst(0);
  drawArrayLines->setCount(vertexData->size());

  osg::Geode* geode = new osg::Geode;
  geode->addDrawable( linesGeom );
  
  addChild( geode );

}

