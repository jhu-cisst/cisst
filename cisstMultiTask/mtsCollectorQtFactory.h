/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2014-03-03

  (C) Copyright 2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _mtsCollectorQtFactory_h
#define _mtsCollectorQtFactory_h

#include <cisstMultiTask/mtsTaskFromSignal.h>

#include <string>
#include <list>

class QWidget;

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

/*!
  \ingroup cisstMultiTask

  Create collectors Qt components (mtsCollectorQtComponent)
  corresponding to data collector created using an instance of the
  class mtsCollectorFactory.
*/
class CISST_EXPORT mtsCollectorQtFactory: public mtsTaskFromSignal
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
 public:
    /*! Constructor */
    mtsCollectorQtFactory(const std::string & componentName);
    inline ~mtsCollectorQtFactory() {};

    void Run(void);
    void Cleanup(void);

    /*! Define which collector factory was used to create the data
      collectors to be connected to the QtWidget.  This method must be
      called after the mtsCollectorFactory is fully configured
      (i.e. after all calls to mtsCollectorFactory::Configure or
      mtsCollectorFactory::AddStateCollectors).  This method must be
      called before the methods mtsCollectorQtFactory::Connect and
      mtsCollectorQtFactory::ConnectToWidget. */
    void SetFactory(const std::string & factoryName);
    
    /*! Connect all the state collectors created by the
      mtsCollectorFactory to all the mtsCollectorQtComponent created
      by this class. */
    void Connect(void) const;

    /*! Connect to a QtWidget using slots and signals.  One can use
      the widget mtsCollectorQtWidget or any custom widget with the
      same slots and signals. */
    void ConnectToWidget(QWidget * widget) const;

 protected:
    typedef std::pair<std::string, std::string> CollectorQtComponent;
    typedef std::map<std::string, std::string> CollectorQtComponents;
    CollectorQtComponents mQtComponents;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsCollectorQtFactory)

#endif // _mtsCollectorQtFactory_h
