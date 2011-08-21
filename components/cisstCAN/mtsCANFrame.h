/*

  Author(s): Simon Leonard
  Created on: Dec 02 2009

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsCANFrame_h
#define _mtsCANFrame_h

#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstCAN/cisstCANFrame.h>
#include <cisstCAN/cisstCANExport.h>

class CISST_EXPORT mtsCANFrame : 

  public mtsGenericObject,
  public cisstCANFrame {
  
  CMN_DECLARE_SERVICES( CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR );
  
 public:

  //! Create an empty CAN frame
  mtsCANFrame();

  //! Create a mtsCANFrame from a cisstCANFrame
  mtsCANFrame( const cisstCANFrame& frame );

  //! Create a mtsCANFrame from a CAN id and data field
  mtsCANFrame( ID canid, DataField data, DataLength nbytes );
  
  //! Create a mtsCANFrame from a vctVector
  mtsCANFrame( ID canid, const vctDynamicVector<cisstCANFrame::Data>& data );

};

CMN_DECLARE_SERVICES_INSTANTIATION( mtsCANFrame )

#endif
