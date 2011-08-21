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

#include <cisstCAN/mtsCANFrame.h>
#include <cisstCommon/cmnLogger.h>

CMN_IMPLEMENT_SERVICES( mtsCANFrame )

mtsCANFrame::mtsCANFrame() : 
  mtsGenericObject(),
  cisstCANFrame(){}

mtsCANFrame::mtsCANFrame( const cisstCANFrame& frame ) :
  mtsGenericObject(),
  cisstCANFrame( frame ){}

mtsCANFrame::mtsCANFrame( ID canid, DataField data, DataLength nbytes ) : 
  mtsGenericObject(),
  cisstCANFrame( canid, data, nbytes ){}

mtsCANFrame::mtsCANFrame( ID canid, 
			  const vctDynamicVector<cisstCANFrame::Data>& data ): 
  mtsGenericObject(),
  cisstCANFrame( canid, data ){}

