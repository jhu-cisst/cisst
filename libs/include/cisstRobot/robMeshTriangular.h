#ifndef _robMeshTriangular_h
#define _robMeshTriangular_h

#include <cisstRobot/robMeshBase.h>

#include <cisstVector/vctFixedSizeVector.h>

class robMeshTriangular : public robMeshBase {
private:

  //! the coordinates of vertices
  //vctFixedSizeVector<double,3>* vertices;
  double* vx;           // vertices x coordinates
  double* vy;           // vertices y coordinates
  double* vz;           // vertices z coordinates     
  size_t nvertices;
  
  //! the index of the vertices
  //vctFixedSizeVector<int,3>* triangles;
  size_t* p1;           // index of the 1st vertex
  size_t* p2;           // index of the 2nd vertex
  size_t* p3;           // index of the 3rd vertex
  size_t ntriangles;

  //! the triangles normals
  //vctFixedSizeVector<double,3>* normals;
  double* nx;           // normal x
  double* ny;           // normal y
  double* nz;           // normal z

  //! The RGB color
  //vctFixedSizeVector<double,3>  RGB;

  void AllocateMemory( size_t nv, size_t nt );
  void DeallocateMemory();

public:
  
  //! Default constructor
  robMeshTriangular();

  //! Copy constructor (deep copy)
  robMeshTriangular( const robMeshTriangular& mesh );

  //! Destructor
  ~robMeshTriangular();

  //! Assignment operator (deep copy)
  robMeshTriangular& operator=(const robMeshTriangular& mesh);

  //! Assignment operator 
  robMeshTriangular& operator=(const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt);

  robError Load( const std::string& filename );
  void Draw() const;

};

#endif
