
# add OpenNi
set( HEADER_FILES	../cisstOpenNI.h
					../cisstOpenNISkeleton.h )
set( SOURCE_FILES	cisstOpenNI.cpp
					cisstOpenNISkeleton.cpp )

# Finally create the library
add_library( cisstOpenNI ${HEADER_FILES} ${SOURCE_FILES} )

target_link_libraries( cisstOpenNI ${OPENNI_LIBRARIES} )

cisst_target_link_libraries ( 
  cisstOpenNI
  cisstCommon
  cisstVector
  cisstOSAbstraction
  cisstMultiTask )


