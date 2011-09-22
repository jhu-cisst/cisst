/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2005-12-29

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


%include "cisstNumerical/nmrSVD.h"

%template(nmrSVDDynamicData) nmrSVDDynamicData::nmrSVDDynamicData<vctDynamicMatrixOwner<double> >;
%template(nmrSVDDynamicData) nmrSVDDynamicData::nmrSVDDynamicData<vctDynamicMatrixRefOwner<double> >;

%template(nmrSVD) nmrSVD<vctDynamicMatrixOwner<double> >;
%template(nmrSVD) nmrSVD<vctDynamicMatrixRefOwner<double> >;

