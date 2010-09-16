#include <cisstDevices/can/CiA301.h>

CiA301::Node::ID CiA301::Node::operator++( CiA301::Node::ID& id, int  ){
  if( id == CiA301::Node::NODE_0 ) { return id = CiA301::Node::NODE_0; }
  if( id == CiA301::Node::NODE_1 ) { return id = CiA301::Node::NODE_2; }
  if( id == CiA301::Node::NODE_2 ) { return id = CiA301::Node::NODE_3; }
  if( id == CiA301::Node::NODE_3 ) { return id = CiA301::Node::NODE_4; }
  if( id == CiA301::Node::NODE_4 ) { return id = CiA301::Node::NODE_5; }
  if( id == CiA301::Node::NODE_5 ) { return id = CiA301::Node::NODE_6; }
  if( id == CiA301::Node::NODE_6 ) { return id = CiA301::Node::NODE_6; }
  return CiA301::Node::NODE_0;
}
