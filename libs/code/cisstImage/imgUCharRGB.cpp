/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Anton Deguet
  Created on: 2006-03-28

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstImage/imgUCharRGB.h>


const imgUCharRGB::Pixel imgUCharRGB::Black((unsigned char)(0));
const imgUCharRGB::Pixel imgUCharRGB::White((unsigned char)(255));
const imgUCharRGB::Pixel imgUCharRGB::Red((unsigned char)(255), (unsigned char)(0), (unsigned char)(0));
const imgUCharRGB::Pixel imgUCharRGB::Green((unsigned char)(0), (unsigned char)(255), (unsigned char)(0));
const imgUCharRGB::Pixel imgUCharRGB::Blue((unsigned char)(0), (unsigned char)(0), (unsigned char)(255));

