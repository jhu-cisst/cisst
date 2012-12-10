Patch CppUnit to compile using CMake with ExternalProject_Add.

This patch is based a CMake patch available for cppunit, which can be
obtained via the Tracker/Patches link in the SourceForge cppunit page
(http://sourceforge.net/projects/cppunit/).

Main modifications include dos2unix and compile CppUnit as a static
library.
