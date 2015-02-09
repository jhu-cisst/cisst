/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2013-04-20

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstVector/vctQtWidgetFrame.h>

vctQtWidgetFrameDoubleRead::vctQtWidgetFrameDoubleRead(const DisplayModeType displayMode):
    QWidget()
{
    RotationWidget = new vctQtWidgetRotationDoubleRead(displayMode);
    TranslationWidget = new vctQtWidgetDynamicVectorDoubleRead();
    TranslationWidget->SetPrecision(3);
    Layout = new QVBoxLayout;
    this->setLayout(Layout);
    this->setWindowTitle("vctQtWidgetFrameDoubleRead");
    Layout->setSpacing(0);
    Layout->addWidget(RotationWidget);
    Layout->addWidget(TranslationWidget);
}
