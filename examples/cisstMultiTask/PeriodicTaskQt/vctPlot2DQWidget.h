/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctPlot2DQWidget.h 1238 2010-02-27 03:16:01Z auneri1 $

  Author(s):  Anton Deguet
  Created on: 2010-05-05

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _vctPlot2DQWidget_h
#define _vctPlot2DQWidget_h

#include <map>
#include <string>

#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>

#include <QGLWidget>

class vctPlot2DQWidget: public QGLWidget
{
    Q_OBJECT;

public:

    class Trace 
    {
        friend class vctPlot2DQWidget;
    public:
        Trace(const std::string & name, size_t numberOfPoints);
        ~Trace() {};

        void AddPoint(const vctDouble2 & point);

        void UpdateMinAndMax(vctDouble2 & min, vctDouble2 & max);

    protected:
        bool Active;
        std::string Name;
        vctDynamicVector<vctDouble2> Data;
        size_t IndexFirst;
        size_t IndexLast;
        vctDouble3 Color;
        double LineWidth;
    };

    vctPlot2DQWidget(QWidget * parent);
    ~vctPlot2DQWidget(void) {};

    /*! Create a new trace, user needs to provide a placeholder to
      retrieve the traceId assigned.  This method checks if the name
      has already been used.  If so, the trace won't be added and the
      method returns false. */
    bool AddTrace(const std::string & name, size_t & traceId);

    /*! Set the number of points for all traces. */
    void SetNumberOfPoints(size_t numberOfPoints);

    /*! Add a point to a given trace */
    void AddPoint(size_t trace, const vctDouble2 & point);


#if 0
    // no yet implemented
    void SetGridColor(const vctDouble3 & color);
#endif 

    /*! Set background color, defined as RGB between 0 and 1. */
    void SetBackgroundColor(const vctDouble3 & color);

 protected:

#if 0
    // no yet implemented
    DrawGrid(void);
#endif

    /*! Methods required for Qt */
    //@{
    virtual void initializeGL(void);
    virtual void resizeGL(int w, int h);
    virtual void paintGL(void);
    //@}

protected:
    // keep traces in a vector
    typedef std::vector<Trace *> TracesType;
    TracesType Traces;
    // maintain a map to find trace Id by name
    typedef std::map<std::string, size_t> TracesIdType;
    TracesIdType TracesId;

    // default number of points for all traces
    size_t NumberOfPoints;

    // viewport sizes
    vctDouble2 Viewport;

    // background color
    vctDouble3 BackgroundColor;
};

#endif  // _vctPlot2DQWidget_h
