
// check if this module is build as a DLL
#ifdef cisstCAN_EXPORTS
#define CISST_THIS_LIBRARY_AS_DLL
#endif

// include common defines
#include <cisstCommon/cmnExportMacros.h>

// avoid impact on other modules
#undef CISST_THIS_LIBRARY_AS_DLL

