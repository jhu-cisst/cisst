/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs P. Vagvolgyi
  Created on: 2011-06-03

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlQtDialog.h>


/*************************/
/*** svlQtDialog class ***/
/*************************/

svlQtDialog::svlQtDialog(QWidget *parent) :
    QDialog(parent)
{
}

void svlQtDialog::closeEvent(QCloseEvent *event)
{
    event->accept();
    Success = true;
    EventClosed.Raise();
}

