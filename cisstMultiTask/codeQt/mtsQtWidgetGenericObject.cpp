/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Praneeth Sadda, Anton Deguet
  Created on: 2011-11-11

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstMultiTask/mtsQtWidgetGenericObject.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>

#include <QHBoxLayout>


mtsQtWidgetGenericObjectRead::mtsQtWidgetGenericObjectRead(void):
    QWidget()
{
    Layout = new QHBoxLayout();
    Layout->setContentsMargins(0, 0, 0, 0);
    setLayout(Layout);
    setFocusPolicy(Qt::StrongFocus);
}

mtsQtWidgetGenericObjectWrite::mtsQtWidgetGenericObjectWrite(void):
    mtsQtWidgetGenericObjectRead()
{
}
