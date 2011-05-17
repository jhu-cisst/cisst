/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctPlot2DBase.h 1238 2010-02-27 03:16:01Z auneri1 $

  Author(s):  Anton Deguet
  Created on: 2010-05-05

  (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights Reserved.

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

#include <cisstCommon/cmnNamedMap.h>
#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstOSAbstraction.h>

// Always include last
#include <cisstVector/vctExport.h>

/*! Base class for 2D plotters.  Handles storage of 2D points, colors
  and thicknesses, computation of scales and offsets.

  \todo Rename "trace" to "signal", "data set", "set", "point set", ...
  \todo Add notion of scale, each line and signal should be managed within a "scale"
  \todo Add "grid" and/or "axis" (maybe a special grid) to all scales
  \todo Add a flag to all elements to determine if they should be used or not in auto fit methods (e.g. user might {not} want to see y=0 line)
  \todo Cleanup pointSize vs lineWidth in traces, lines, etc.  use linewidth for everything
  \todo Create a base class "element"/"data" for all elements visible (i.e. signal, vertical line, grid, ...), modifiedSinceLastRender, maybe even render method ...
  \todo Remove all traceId code, Ids are not that efficient anyway and users should keep a pointer on the trace/line/grid they need or retrieve these by name
  \todo Fit/Render should probably be defined at plot/scale/data level
  \tobo Add flag/method Modified to all elements type to optimize fit/render
*/
class CISST_EXPORT vctPlot2DBase
{

public:

    /*! Storage for a given signal.  Each signal stores the data to
      display in a vector (vctDynamicVector) of points (vctDouble2).
      To prevent dynamic re-allocation, this class uses a "circular
      buffer".  */
    class CISST_EXPORT Trace
    {
        friend class vctPlot2DBase;
        friend class vctPlot2DOpenGL;
        friend class vctPlot2DVTK;
    public:
        Trace(const std::string & name, size_t numberOfPoints, size_t PointSize = 2);
        ~Trace();

        void AddPoint(const vctDouble2 & point);
        void AppendPoint(const vctDouble2 & point);
        vctDouble2 GetPointAt(size_t index);
        void SetPointAt(size_t index, const vctDouble2 & point);
        void SetArrayAt(size_t index, const double * pointArray, size_t size);
        bool PrependArray(const double * pointArray, size_t arraySize);
        bool AppendArray(const double * pointArray, size_t arraySize);
        void Freeze(bool freeze);

        void ComputeDataRangeX(double & min, double & max);
        void ComputeDataRangeY(double & min, double & max);
        void ComputeDataRangeXY(vctDouble2 & min, vctDouble2 & max);
        void GetLeftRightDataX(double & min, double &max);

        void SetNumberOfPoints(size_t numberOfPoints);
        void GetNumberOfPoints(size_t &numberOfPoints, size_t &bufferSize);
        void SetColor(const vctDouble3 & colorInRange0To1);

        void SetSize(size_t numberOfPoints);
        void ReSize(size_t numberOfPoints);

    protected:
        /*! Trace name, used for GUI */
        std::string Name;
        bool Empty;
        bool Visible;
        bool Frozen;
        /*! Actual buffer containing the data, contiguous for rendering */
        double * Buffer;
        size_t PointSize;
        /*! Vector of references to the data to add, compute min/max, ... */
        typedef vctFixedSizeVectorRef<double, 2, 1> PointRef;
        vctDynamicVector<PointRef> Data;
        size_t IndexFirst;
        size_t IndexLast;
        vctDouble3 Color;
        double LineWidth;
        // Critical Section
        //osaCriticalSection CriticalSectionForBuffer;
    };

    /*! Storage for a given vertical line. */
    class CISST_EXPORT VerticalLine
    {
        friend class vctPlot2DBase;
        friend class vctPlot2DOpenGL;
        friend class vctPlot2DVTK;
    public:
        VerticalLine(const std::string & name, const double x = 0.0);
        ~VerticalLine();

        void SetX(const double x);
        void SetColor(const vctDouble3 & colorInRange0To1);

    protected:
        std::string Name;
        double X;
        bool Visible;
        vctDouble3 Color;
        double LineWidth;
    };


    vctPlot2DBase(size_t PointSize = 2);
    virtual ~vctPlot2DBase(void) {};

    /*! Set the number of points for all traces. */
    void SetNumberOfPoints(size_t numberOfPoints);

    /*! Create a new trace, user needs to provide a placeholder to
      retrieve the traceId assigned.  This method checks if the name
      has already been used.  If so, the trace won't be added and the
      method returns a 0 pointer. */
    Trace * AddTrace(const std::string & name, size_t & traceId);

    /*! Create a new trace.  This method checks if the name has
      already been used.  If so, the trace won't be added and the
      method returns a 0 pointer. */
    Trace * AddTrace(const std::string & name);

    /*! Create a new vertical line.  This method checks if the name
      has already been used.  If so, the line won't be added and the
      method returns a 0 pointer. */
    VerticalLine * AddVerticalLine(const std::string & name);

    /*! Add a point to a given trace.  Deprecated, use
      tracePointer->AddPoint instead. */
    void CISST_DEPRECATED AddPoint(size_t trace, const vctDouble2 & point);

    /*! Data recentering, these methods re-align the data once only,
      based on all traces.  Padding is used to make sure the data is
      not plotted at the extreme edges of the window.  The padding
      parameter indicates the percentage of space that should be left
      empty.  For example, a 200 pixel window with a padding of 0.1
      (10%) will leave a band of 10 pixels empty on each side of the
      window.  By default, the padding in X (horizontal) is null and
      10% (0.1) in Y (vertical). */
    //@{
    void FitX(double padding = 0.0);
    void FitX(double min, double max, double padding = 0.0);
    void FitY(double padding = 0.1);
    void FitY(double min, double max, double padding = 0.1);
    void FitXY(const vctDouble2 & padding = vctDouble2(0.0, 0.1));
    void FitXY(vctDouble2 min, vctDouble2 max, const vctDouble2 & padding = vctDouble2(0.0, 0.1));
    void AlignMaxX(void);
    //@}

    /*! Freeze the circular buffers, i.e. AddPoint does nothing.  When
      turned off (parameter is false), this is equivalent to starting
      with an empty data set.  These methods work on all traces at
      once. */
    //@{
    void Freeze(bool freeze);
    inline bool GetFreeze(void) const {
        return this->Frozen;
    }
    //@}

    /*! Automatic recentering */
    //@{
    void SetContinuousFitX(bool fit);
    void SetContinuousFitY(bool fit);
    void SetContinuousAlignMaxX(bool align);

    inline bool GetContinuousFitX(void) const {
        return this->ContinuousFitX;
    }
    inline bool GetContinuousFitY(void) const {
        return this->ContinuousFitY;
    }
    inline bool GetContinuousAlignMaxX(void) const {
        return this->ContinuousAlignMaxX;
    }
    //@}

    /*! To fit the data in the viewport we need to compute the range
      for all traces.  To reduce the number of computations, three
      methods are provided, one that compute the X range only, one for
      the Y range only and one for both X and Y.  Each method uses a
      single loop. */
    //@{
    void ComputeDataRangeX(double & min, double & max);
    void ComputeDataRangeY(double & min, double & max);
    void ComputeDataRangeXY(vctDouble2 & min, vctDouble2 & max);
    //@}

    //*! Get currently used viewing range */
    //@{
    inline const vctDouble2 & GetViewingRangeX(void) const {
        return this->ViewingRangeX;
    }
    inline const vctDouble2 & GetViewingRangeY(void) const {
        return this->ViewingRangeY;
    }
    //@}

#if 0
    // no yet implemented
    void SetGridColor(const vctDouble3 & colorInRange0To1);

#endif

    /*! Set color for a specific trace.  Deprecated, use
      tracePointer->SetColor. */
    void CISST_DEPRECATED SetColor(size_t traceId, const vctDouble3 & colorInRange0To1);

    /*! Set background color, defined as RGB between 0 and 1. */
    void SetBackgroundColor(const vctDouble3 & colorInRange0To1);

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
    /*! Point size in memory, i.e. offset in sizeof(double) between
      points.  Different for OpenGL in 2D, VTK, ... */
    size_t PointSize;

    // keep traces in a vector
    typedef std::vector<Trace *> TracesType;
    TracesType Traces;
    // maintain a map to find trace Id by name
    typedef std::map<std::string, size_t> TracesIdType;
    TracesIdType TracesId;

    // keep vertical lines in a vector
    typedef cmnNamedMap<VerticalLine> VerticalLinesType;
    VerticalLinesType VerticalLines;

    // default number of points for all traces
    size_t NumberOfPoints;
    bool Frozen;

    // viewport sizes
    vctDouble2 Viewport;
    // stores the min and max corresponding to the viewport
    vctDouble2 ViewingRangeX, ViewingRangeY;
    vctDouble2 Translation;
    vctDouble2 Scale;

    // continuous computation of scales and offsets
    bool Continuous;
    bool ContinuousFitX;
    bool ContinuousFitY;
    bool ContinuousAlignMaxX;

    /*! Method called at each iteration to figure out if an automatic
      update is needed or not. */
    void ContinuousUpdate(void);

    // background color
    vctDouble3 BackgroundColor;
};

#endif  // _vctPlot2DBase_h
