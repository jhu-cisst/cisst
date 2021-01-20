# initial CMake cache values for travis.ci builds

# Use external projects, mostly to get recent version of libcpp on Ubuntu 16.04
set (CISST_USE_EXTERNAL ON CACHE BOOL "")

# Compile for shared libraries
set (CISST_BUILD_SHARED_LIBS ON CACHE BOOL "")

# Use SI units
set (CISST_USE_SI_UNITS ON CACHE BOOL "")

# Force compilation for optional libraries
set (CISST_cisstMesh ON CACHE BOOL "")
set (CISST_cisstRobot ON CACHE BOOL "")

# XML parsing
set (CISST_cisstCommonXML OFF CACHE BOOL "")

# JSON
set (CISST_HAS_JSON ON CACHE BOOL "")

# Unit tests
set (CISST_BUILD_TESTS ON CACHE BOOL "")

# cisstNetlib
set (CISSTNETLIB_USE_LOCAL_INSTALL ON CACHE BOOL "")
set (CISST_HAS_CISSTNETLIB ON CACHE BOOL "")

# Qt
set (CISST_HAS_QT5 ON CACHE BOOL "")
