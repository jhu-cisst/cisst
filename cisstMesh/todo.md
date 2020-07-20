



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

* Rename files and classes to use prefix `msh`
* Update license/header, preserve copyright
* Add formating header lines to all files
* Remove tabs and trailing blanks