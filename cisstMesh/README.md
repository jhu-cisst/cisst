# Introduction

Mesh and point cloud classes for 2D and 3D datasets.  This code was mostly
developped by Seth Billings during his PhD (see
https://jscholarship.library.jhu.edu/handle/1774.2/40219.  His PhD
dissertation contains a chapter related to software architecture.

Important note: the library `cisstMesh` only contains the mesh code
(i.e. data structures and search algorithms), it doesn't contain any
registration method (e.g. Iterative Closest Points `ICP`).

# Acronyms and definitions

* `msh2`: prefix for all classes and global functions related to 2D meshes
* `msh3`: prefix for all classes and global functions related to 3D meshes
* `PDTree`: principal direction tree
* `Alg`: algorithm (search)
* `Edges`: in 2D, list of edges or contour (i.e. equivalent of mesh in 3D)
* `Mesh`: in 3D, surface mesh
* `PointClound`: point cloud, either 2D or 3D
* `CP`: closest point
* `MLP`: most likely point
* `BA`: bounded angle
* `Dir`: directed
* `DirPDTree`: directed PD tree
* `Proj`: projection

# Origin of the code

The code in cisstMesh comes from different public git repositories:
* https://github.com/sbillin/IMLP
* https://github.com/AyushiSinha/cisstICP

The main differences are:
* This repository only contains the mesh related code, no registration yet (one should create the `cisstRegistration` library for this but we need some volunteers).
* Classes and files have been renamed to match `cisst` naming convention.  One can use the `cisst/utils/crtk-port/replace-symbols.sh` script to port existing code, along with the dictionary `refactor.dict`

# Tests

To run the unit tests, make sure:
* The `share` folder in `cisst` is populated.   We use a git submodule to include the shared data so you need to do `git submodule init; git submodule update` in the cisst repository.  Alternatively, you can use the `--recursive` option when you initially `git clone` the `cisst` repository.
* You set your `cisst` environment variables properly.  The test program uses `CISST_ROOT` to find the meshes.  The script `cisstvars.sh` can be "sourced" to set `CISST_ROOT` automatically.   When using ROS/catkin build tools, you can do `source ~/catkin_ws/devel/cisstvars.sh`

Then you can run the tests with:
```sh
cisstMeshTests -r
```
