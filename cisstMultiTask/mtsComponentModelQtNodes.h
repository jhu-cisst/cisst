/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2025-10-28

  (C) Copyright 2025-2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsComponentModelQtNodes_h
#define _mtsComponentModelQtNodes_h

#include <cisstConfig.h>
#if !CISST_HAS_QTNODES
#error "mtsComponentModelQtNodes.h requires CISST_HAS_QTNODES"
#endif

#include <QtNodes/NodeDelegateModel>
#include <vector>

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

class CISST_EXPORT mtsComponentModelQtNodes
    : public QtNodes::NodeDelegateModel {
  Q_OBJECT

public:
  mtsComponentModelQtNodes(const std::string &name);
  ~mtsComponentModelQtNodes() = default;

  // NodeDelegateModel interface
  QString caption(void) const override;
  QString name(void) const override;

  unsigned int nPorts(QtNodes::PortType portType) const override;

  QtNodes::NodeDataType dataType(QtNodes::PortType portType,
                                 QtNodes::PortIndex portIndex) const override;

  QString portCaption(QtNodes::PortType portType,
                      QtNodes::PortIndex portIndex) const override;

  bool portCaptionVisible(QtNodes::PortType portType,
                          QtNodes::PortIndex portIndex) const override;

  void setInData(std::shared_ptr<QtNodes::NodeData> nodeData,
                 QtNodes::PortIndex port) override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

  QWidget *embeddedWidget(void) override;

  bool AddInterfaceProvided(const std::string &name);
  bool AddInterfaceRequired(const std::string &name);

protected:
  std::string m_name;
  std::vector<std::string> m_interfaces_provided;
  std::vector<std::string> m_interfaces_required;
  QWidget *m_widget = nullptr;
};

#endif // _mtsComponentModelQtNodes_h
