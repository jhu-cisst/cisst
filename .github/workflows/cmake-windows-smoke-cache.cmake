# Initial CMake cache values for the Windows configure smoke test.
#
# This pass runs before cisstNetlib is downloaded/built, so keep
# cisstNetlib-dependent libraries disabled.  The full Windows build uses
# cmake-windows-cache.cmake instead.

# Use external projects, mostly to get recent version of libcpp on Ubuntu 16.04
set (CISST_USE_EXTERNAL OFF CACHE BOOL "")

# Compile for shared libraries
set (CISST_BUILD_SHARED_LIBS ON CACHE BOOL "")

# Use SI units
set (CISST_USE_SI_UNITS ON CACHE BOOL "")

# Keep the smoke test independent from cisstNetlib
set (CISST_cisstMesh OFF CACHE BOOL "")
set (CISST_cisstRobot OFF CACHE BOOL "")

# XML parsing
set (CISST_cisstCommonXML OFF CACHE BOOL "")

# JSON
set (CISST_HAS_JSON ON CACHE BOOL "")

# Unit tests
set (CISST_BUILD_TESTS ON CACHE BOOL "")
set (CISST_cisstRobot_TESTS OFF CACHE BOOL "")
set (CISST_cisstMesh_TESTS OFF CACHE BOOL "")

# Build examples
set (CISST_BUILD_EXAMPLES OFF CACHE BOOL "")

# cisstNetlib
set (CISSTNETLIB_USE_LOCAL_INSTALL OFF CACHE BOOL "")
set (CISST_HAS_CISSTNETLIB OFF CACHE BOOL "")

# Qt
set (CISST_QT_VERSION 5 CACHE STRING "")
set (CISST_HAS_QTNODES ON CACHE BOOL "")
