/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2010-03-20

  (C) Copyright 2010-2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once

#ifndef _cmnLoggerQtWidget_h
#define _cmnLoggerQtWidget_h

#include <cisstCommon/cmnGenericObject.h>

// forward declarations
class QWidget;
class QVBoxLayout;
class QLabel;
class QGridLayout;
class QTableView;
class QLineEdit;
class QComboBox;
class QPlainTextEdit;
class QSortFilterProxyModel;
class QTimer;
#include <QStringList>

class cmnLoggerQtWidgetClassServicesModel;
class cmnLoggerQtWidgetLoDDelegate;
class cmnLoggerQtStreambufEmitter;
class cmnLoggerQtStreambuf;

// Always include last
#include <cisstCommon/cmnExportQt.h>

/*!
  \brief Qt widget for interactive log level management.

  Provides:
  - "Logs" tab: live log message display with timestamp and color coding.
  - "Settings" tab: per-class Init/Run mask controls, global overall/function
    mask controls, and a class name search filter.

  Typical use (via mtsCommandLineOptionsQt -L) or embedded directly:
  \code
    cmnLoggerQtWidget * logger = new cmnLoggerQtWidget(nullptr);
    layout->addWidget(logger->GetWidget());
  \endcode
*/
class CISST_EXPORT cmnLoggerQtWidget: public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    cmnLoggerQtWidget(QWidget * parent = nullptr);
    ~cmnLoggerQtWidget();

    /*! Return the top-level QWidget so it can be embedded or shown. */
    inline QWidget * GetWidget(void) { return this->Widget; }

protected:
    // top-level container
    QWidget    * Widget;
    QVBoxLayout * Layout;

    // Settings tab — global mask grid
    QWidget     * MainFilterWidget;
    QGridLayout * MainFilterLayout;
    QLabel      * OverallFilterLabel;
    QComboBox   * OverallFilterData;   // Init nibble of overall mask
    QComboBox   * OverallRunData;      // Run  nibble of overall mask
    QLabel      * FunctionFilterLabel;
    QComboBox   * FunctionFilterData;  // Init nibble of function mask
    QComboBox   * FunctionRunData;     // Run  nibble of function mask

    // Settings tab — class table
    QLineEdit                         * ClassNameFilterLineEdit;
    cmnLoggerQtWidgetClassServicesModel * Model;
    QSortFilterProxyModel               * ProxyModel;
    cmnLoggerQtWidgetLoDDelegate        * Delegate;
    QTableView                          * View;

    // Logs tab
    QPlainTextEdit * LogDisplay;
    QComboBox      * MaxLinesDisplay;

    // Log capture
    cmnLoggerQtStreambufEmitter * Emitter;
    cmnLoggerQtStreambuf        * Streambuf;
    std::ostream               * LogStream;
    QStringList                  PendingLines; // accumulated between GUI flush ticks

    // Periodic class-list refresh
    QTimer * Timer;
};

CMN_DECLARE_SERVICES_INSTANTIATION(cmnLoggerQtWidget);

#endif // _cmnLoggerQtWidget_h
