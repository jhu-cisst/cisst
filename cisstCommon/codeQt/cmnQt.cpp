/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2017-12-19

  (C) Copyright 2017-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnQt.h>

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnLogger.h>

#include <QApplication>
#include <QPalette>
#include <QStyle>
#include <QStyleFactory>

#if (CISST_OS == CISST_LINUX)

#include <signal.h>

void cmnQtQApplicationExitsOnCtrlC_SignalHandler(int)
{
    CMN_LOG_INIT_WARNING << "cmnQtQApplicationExitsOnCtrlC_SignalHandler: exiting Qt Core Application" << std::endl;
    QCoreApplication::exit(0);
}
#endif // CISST_OS

void cmnQt::QApplicationExitsOnCtrlC(void) {
#if (CISST_OS == CISST_LINUX)
    signal(SIGINT, cmnQtQApplicationExitsOnCtrlC_SignalHandler);
#endif
}

void cmnQt::SetDarkMode(void)
{
    QPalette * palette = new QPalette();
    palette->setColor(QPalette::Window, QColor(53, 53, 53));
    palette->setColor(QPalette::WindowText, Qt::white);
    palette->setColor(QPalette::Base, QColor(75, 75, 75));
    palette->setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    palette->setColor(QPalette::ToolTipBase, Qt::white);
    palette->setColor(QPalette::ToolTipText, Qt::black);
    palette->setColor(QPalette::Text, Qt::white);
    palette->setColor(QPalette::Button, QColor(53, 53, 53));
    palette->setColor(QPalette::ButtonText, Qt::white);
    palette->setColor(QPalette::BrightText, Qt::red);
    palette->setColor(QPalette::Link, QColor(42, 130, 218));
    palette->setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette->setColor(QPalette::HighlightedText, Qt::black);
    QApplication::setPalette(*palette);
}

std::string cmnQt::SetStyle(const std::string & qtStyle)
{
    std::string result;
    QStyle * style = QApplication::setStyle(QString(qtStyle.c_str()));
    if (!style) {
        result += "Style \"" + qtStyle + "\" is not valid, pick one from: "
            + QStyleFactory::keys().join(", ").toStdString();
    }
    return result;
}
