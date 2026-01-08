/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2025-10-28

  (C) Copyright 2025 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsGraphLayoutQtNodes_h
#define _mtsGraphLayoutQtNodes_h

#include <QPointF>
#include <map>
#include <vector>
#include <string>

class mtsGraphLayoutQtNodes {
public:
    struct Node {
        QPointF pos;    // Current position
        QPointF vel;    // Current velocity
        QPointF force;  // Current force
    };

    void Clear(void);
    void AddNode(const std::string & component);
    void RemoveNode(const std::string & component);
    void AddEdge(const std::string & from, const std::string & to);
    
    // Run one iteration of force-directed layout
    void Step(void);
    
    // Get current position for a node
    QPointF GetPosition(const std::string & component) const;

    // Parameters
    double SpringK = 0.1;        // Spring constant
    double RepulsionK = 5000.0;  // Repulsion constant
    double Damping = 0.8;        // Velocity damping
    double TimeStep = 0.5;       // Integration time step
    double SpringLength = 200.0;  // Desired spring length

protected:
    void ApplySpringForces(void);
    void ApplyRepulsionForces(void);
    void IntegrateForces(void);

    std::map<std::string, Node> Nodes;
    std::vector<std::pair<std::string, std::string>> Edges;
};

#endif // _mtsGraphLayoutQtNodes_h
