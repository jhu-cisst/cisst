/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Martin Kelly
  Created on: 2011-03-07

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsSphinx4QtComponent_h
#define _mtsSphinx4QtComponent_h

#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstMultiTask/mtsFunctionQualifiedRead.h>
#include <cisstMultiTask/mtsFunctionWrite.h>

#include <QObject>

#include <map>

#include <cisstSphinx4/mtsSphinx4QtWidget.h>

// Always include last!
#include <cisstSphinx4/mtsSphinx4ExportQt.h>

class CISST_EXPORT mtsSphinx4QtComponent: public QObject, public mtsComponent
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

 public:
    mtsSphinx4QtComponent(const std::string & componentName);
    ~mtsSphinx4QtComponent(void) {};

    void Configure(const std::string & CMN_UNUSED(filename) = "") {};
    void Start(void);

    QWidget * GetWidget(void);

 signals:
    void WordRecognizedQSignal(QString word);
    void NoWordRecognizedQSignal(void);
    void ContextChangedQSignal(QString word);
    void WordAddedQSignal(QString context, QString word);

 public slots:
    void TriggerWord(QString word);

 protected:
    mtsSphinx4QtWidget CentralWidget;

    void WordRecognizedHandler(const mtsStdString & word);
    void NoWordRecognizedHandler(void);
    void ContextChangedHandler(const mtsStdString & context);

    mtsFunctionRead GetContexts;
    mtsFunctionQualifiedRead GetContextWords;
    mtsFunctionWrite TriggerWordFromUI;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsSphinx4QtComponent);

#endif  // _mtsSphinx4QtComponent_h
