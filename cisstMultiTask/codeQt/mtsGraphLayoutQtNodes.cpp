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

#include <cisstMultiTask/mtsGraphLayoutQtNodes.h>
#include <cmath>

void mtsGraphLayoutQtNodes::Clear(void)
{
    Nodes.clear();
    Edges.clear();
}

void mtsGraphLayoutQtNodes::AddNode(const std::string & component)
{
    if (Nodes.find(component) != Nodes.end()) {
        return;
    }
    
    // Initialize node with random position in 1000x1000 area
    Node node;
    node.pos = QPointF(rand() % 1000, rand() % 1000);
    node.vel = QPointF(0, 0);
    node.force = QPointF(0, 0);
    
    Nodes[component] = node;
}

void mtsGraphLayoutQtNodes::RemoveNode(const std::string & component)
{
    Nodes.erase(component);
    
    // Remove any edges connected to this node
    auto it = Edges.begin();
    while (it != Edges.end()) {
        if (it->first == component || it->second == component) {
            it = Edges.erase(it);
        } else {
            ++it;
        }
    }
}

void mtsGraphLayoutQtNodes::AddEdge(const std::string & from, const std::string & to)
{
    if (from == to) {
        return;
    }
    
    // Check if nodes exist
    if (Nodes.find(from) == Nodes.end() || Nodes.find(to) == Nodes.end()) {
        return;
    }
    
    // Check if edge already exists
    for (const auto & edge : Edges) {
        if ((edge.first == from && edge.second == to) ||
            (edge.first == to && edge.second == from)) {
            return;
        }
    }
    
    Edges.push_back(std::make_pair(from, to));
}

void mtsGraphLayoutQtNodes::Step(void)
{
    // Reset forces
    for (auto & pair : Nodes) {
        pair.second.force = QPointF(0, 0);
    }
    
    ApplySpringForces();
    ApplyRepulsionForces();
    IntegrateForces();
}

void mtsGraphLayoutQtNodes::ApplySpringForces(void)
{
    for (const auto & edge : Edges) {
        auto & node1 = Nodes[edge.first];
        auto & node2 = Nodes[edge.second];
        
        QPointF diff = node2.pos - node1.pos;
        double distance = std::sqrt(QPointF::dotProduct(diff, diff));
        
        if (distance > 0) {
            // Normalize direction vector
            QPointF dir = diff / distance;
            
            // Calculate spring force (F = k * (x - rest_length))
            double force = SpringK * (distance - SpringLength);
            
            // Apply force to both nodes in opposite directions
            QPointF springForce = dir * force;
            node1.force += springForce;
            node2.force -= springForce;
        }
    }
}

void mtsGraphLayoutQtNodes::ApplyRepulsionForces(void)
{
    for (auto it1 = Nodes.begin(); it1 != Nodes.end(); ++it1) {
        for (auto it2 = std::next(it1); it2 != Nodes.end(); ++it2) {
            QPointF diff = it2->second.pos - it1->second.pos;
            double distance = std::sqrt(QPointF::dotProduct(diff, diff));
            
            if (distance > 0) {
                // Normalize direction vector
                QPointF dir = diff / distance;
                
                // Calculate repulsion force (F = k / r^2)
                double force = -RepulsionK / (distance * distance);
                
                // Apply force to both nodes in opposite directions
                QPointF repulsionForce = dir * force;
                it1->second.force += repulsionForce;
                it2->second.force -= repulsionForce;
            }
        }
    }
}

void mtsGraphLayoutQtNodes::IntegrateForces(void)
{
    for (auto & pair : Nodes) {
        Node & node = pair.second;
        
        // Update velocity (with damping)
        node.vel = (node.vel + node.force * TimeStep) * Damping;
        
        // Update position
        node.pos += node.vel * TimeStep;
    }
}

QPointF mtsGraphLayoutQtNodes::GetPosition(std::string & component) const
{
    auto it = Nodes.find(component);
    if (it != Nodes.end()) {
        return it->second.pos;
    }
    return QPointF(0, 0);
}
