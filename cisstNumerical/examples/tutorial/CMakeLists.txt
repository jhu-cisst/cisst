#
#
# (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

set (REQUIRED_CISST_LIBRARIES cisstCommon cisstVector cisstNumerical)
find_package (cisst COMPONENTS ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND_AS_REQUIRED)
  include (${CISST_USE_FILE})

  if (CISST_HAS_CISSTNETLIB)
    add_executable (nmrExTutorial
                    main.cpp
		    ExampleSavitzkyGolay.h
                    ExampleSVDUserOutputWorkspace.h
                    ExampleSVDImplicitWorkspace.h
                    ExampleSVDWorkspaceSize.h
                    ExampleSVDDynamicData.h
                    ExampleSVDUpdateMatrixS.h
                    ExampleSVDFixedSize.h
                    ExampleSVDFixedSizeData.h
                    ExampleInverse.h
                    ExampleInverseSPD.h
                    ExamplePInverse.h
                    ExampleLUDynamicData.h
                    ExampleIsOrthonormal.h
                    ExampleGaussJordanInverse.h
                    )
    set_property (TARGET nmrExTutorial PROPERTY FOLDER "cisstNumerical/examples")
    cisst_target_link_libraries(nmrExTutorial ${REQUIRED_CISST_LIBRARIES})

  else (CISST_HAS_CISSTNETLIB)
    message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires CISST_HAS_CISSTNETLIB")
  endif (CISST_HAS_CISSTNETLIB)

else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
