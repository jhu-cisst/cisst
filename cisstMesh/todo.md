To do
=====

* Use `std::string` instead of `char *` for strings
* Cleanup `utilities.h`, maybe move some of these to `cisstVector` or `cisstNumerical`
* Remove all static variables!  Unless large and should be allocated as data members
* Use `size_t` when possible
* Make sure `const` methods are marked as such
* Add missing `inline`
* Add missing `CMN_UNUSED`
* Class `TriangleClosestPointSolver` needs some work (std::cout for warnings, data members naming uses lower case and conflict with method parameters)