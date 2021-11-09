#include <stdio.h>
#include <iostream>
#include <vector>

#include <cisstMesh/msh3Mesh.h>
#include <cisstMesh/msh3PDTreeMesh.h>
#include <cisstMesh/msh3AlgPDTreeCPMesh.h>

int main(void)
{	
  std::string workingDir = "/home/max/galen_ws/src/pdtree/examples/";
  std::string loadMeshPath = workingDir + "mesh/cube_ascii.stl";

  msh3Mesh mesh;
  mesh.LoadMeshFromSTLFile(loadMeshPath);

  std::cout << mesh.NumTriangles() << std::endl;
  std::cout << "face vertex index" << std::endl;
  for (auto e : mesh.faces){
      std::cout << e << std::endl;
  }
  std::cout << "face normal" << std::endl;
  std::cout << mesh.faceNormals.size() << std::endl;
  for (auto e : mesh.faceNormals){
      std::cout << e << std::endl;
  }
  std::cout << "face neighbours" << std::endl;
  std::cout << mesh.faceNeighbors.size() << std::endl;
  for (auto e : mesh.faceNeighbors){
      std::cout << e << std::endl;
  }

  // build PD tree on the mesh directly
  std::cout << "Building mesh PD tree .... " << std::endl;
  int nThresh = 5; double diagThresh = 5.0;
  msh3PDTreeMesh* pTreeMesh = new msh3PDTreeMesh(mesh, nThresh, diagThresh);
  msh3AlgPDTreeCPMesh* pAlg = new msh3AlgPDTreeCPMesh(pTreeMesh);
  pTreeMesh->SetSearchAlgorithm(pAlg);

  std::vector<int> faceIdx;
  double boundingDistance = 2.0;
  vct3 tip(5.0,5.0,11.0);
  int numIntersected = pTreeMesh->FindIntersectedPoints(tip,boundingDistance,faceIdx);

  std::cout << "Number of intersected points " << numIntersected << std::endl;
  for (auto it : faceIdx){
      std::cout << it << std::endl;
      std::cout << "Face normal \n";
      std::cout << mesh.faceNormals[it] << std::endl;
      std::cout << "Closest point found \n";
      std::cout << mesh.closestPoint[it] << std::endl;
      std::cout << "Closest point location \n";
      std::cout << mesh.cpLocation[it] << std::endl;
      std::cout << "\n";
  }

  mesh.MergeEdgePoint(faceIdx, tip);

  std::cout << "Merged result " << faceIdx.size() << std::endl;
  for (auto &it: faceIdx){
    std::cout << it << std::endl;
    std::cout << "active normal" << std::endl;
    std::cout << mesh.activeNormal.at(it) << std::endl;
  }

//  std::cout << "result" << std::endl;
//  for (int i =0; i < closestPoint.size(); i ++){
//      if (std::find(removeIndex.begin(),removeIndex.end(),i) == removeIndex.end()){
//          std::cout << i << std::endl;
//          std::cout << closestPoint.at(i) << std::endl;
//          std::cout << faceNormal.at(i)<< std::endl;
//        }
//    }


//#if test
//  std::vector<vct3> testvec; std::vector<bool> testbool;
//  testvec.push_back(vct3(1,1,1));
//  testvec.push_back(vct3(1,2,1));
//  testvec.push_back(vct3(1,1,1));
//  testvec.push_back(vct3(1,2,1));
//  testvec.push_back(vct3(1,3,1));
//  testvec.push_back(vct3(1,1,4));

//  testbool.push_back(true);
//  testbool.push_back(true);
//  testbool.push_back(true);
//  testbool.push_back(true);
//  testbool.push_back(true);
//  testbool.push_back(true);

//  // remove edge points if there is only one
//  // merge edge points if there are duplicates and average the face normals
//  std::vector<std::vector<int> > edgePointMap;
//  std::vector<int> edgePointIndex; std::vector<bool> trueflag;
//  for (int count = 0 ; count < testvec.size(); count ++){
//      // find an edge point
//      if(testbool.at(count)){
//        bool repetitionFound = false;
//        // look for repetitions
//        for (int subcount = 0; subcount < edgePointMap.size(); subcount++){
//            if (testvec.at(edgePointMap.at(subcount).at(0)) == testvec.at(count)){
//              std::cout << "repetition found to be \n" <<testvec.at(count) << std::endl;
//              edgePointMap.at(subcount).push_back(count);
//              repetitionFound = true;
//              break;
//            }
//        }
//        if (!repetitionFound){
//            std::vector<int> tmp; tmp.push_back(count);
//            edgePointMap.push_back(tmp);
//        }
//      }
//  }

//  for (int count = 0; count < edgePointMap.size(); count ++){
//      for (int subcount = 0; subcount < edgePointMap.at(count).size(); subcount ++){
//          std::cout << edgePointMap.at(count).at(subcount) << " ";
//        }
//      std::cout << std::endl;
//    }
//#endif

  return 0;
}
