/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctPlot2DBase.h 1238 2010-02-27 03:16:01Z auneri1 $

  Author(s):  Anton Deguet
  Created on: 2010-05-05

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _vctPlot2DBase_h
#define _vctPlot2DBase_h

#include <map>
#include <string>

#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>

// Always include last
#include <cisstVector/vctExport.h>

/*! Base class for 2D plotters.  Handles storage of 2D points, colors
  and thicknesses, computation of scales and offsets. */
class CISST_EXPORT vctPlot2DBase
{

public:

    /*! Storage for a given signal */
    class Trace 
    {
        friend class vctPlot2DBase;
        friend class vctPlot2DOpenGL;
        friend class vctPlot2DOpenVTK;
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

    vctPlot2DBase(void);
    ~vctPlot2DBase(void) {};

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

    /*! Methods required in all derived classes */
    //@{
    virtual void RenderInitialize(void) = 0;
    virtual void RenderResize(double width, double height) = 0;
    virtual void Render(void) = 0;
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

#endif  // _vctPlot2DBase_h
