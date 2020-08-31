# initial CMake cache values for travis.ci builds

# Force compilation for cisstMesh
set (CISST_cisstMesh ON CACHE BOOL "")

# XML parsing
set (CISST_cisstCommonXML ON CACHE BOOL "")

# JSON
set (CISST_HAS_JSON ON CACHE BOOL "")

# Unit tests
set (CISST_BUILD_TESTS ON CACHE BOOL "")

# cisstNetlib
set (CISST_HAS_CISSTNETLIB ON CACHE BOOL "")
set (CISSTNETLIB_DIR "/home/travis/cisstNetlib-install" CACHE PATH "")
