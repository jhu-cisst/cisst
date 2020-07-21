



General code
============

* Remove `#include <cissCommon.h>` ...
* Rename `cisstMesh` to  `mshMesh` as soon as possible to make sure we only include `<cisstMesh/mshMesh.h>`
* Use `std::string` instead of `char *` for strings


Specifics
=========

* Cleanup `utilities.h`, maybe move some of these to `cisstVector` or `cisstNumerical`


Major refactor
==============

* Rename files and classes to use prefix `msh2` or `msh3`
* Update license/header, preserve copyright
* Add CISST_EXPORT
* Add formating header lines to all files
* Remove tabs and trailing blanksmsh
* Remove all static variables!  Unless large and should be allocated as data members