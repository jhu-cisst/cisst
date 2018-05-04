/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2010-05-05

  (C) Copyright 2010-2018 Johns Hopkins University (JHU), All Rights Reserved.

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
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>

// Always include last
#include <cisstVector/vctExport.h>

/*! Base class for 2D plotters.  Handles storage of 2D points, colors
  and thicknesses, computation of scales and offsets.

  \todo Rename "trace" to "signal", "data set", "set", "point set", ...
  \todo Add "grid" and/or "axis" (maybe a special grid) to all scales
  \todo Add a flag to all elements to determine if they should be used or not in auto fit methods (e.g. user might {not} want to see y=0 line)
  \todo Cleanup pointSize vs lineWidth in traces, lines, etc.  use linewidth for everything
  \todo Create a base class "element"/"data" for all elements visible (i.e. signal, vertical line, grid, ...), modifiedSinceLastRender, maybe even render method ...
  \todo Fit/Render should probably be defined at plot/scale/data level
  \todo Add flag/method Modified to all elements type to optimize fit/render
*/
class CISST_EXPORT vctPlot2DBase
{

 public:

    class Scale;

    /*! Storage for a given signal.  Each signal stores the data to
      display in a vector (vctDynamicVector) of points (vctDouble2).
      To prevent dynamic re-allocation, this class uses a "circular
      buffer".  */
    class CISST_EXPORT Signal
    {
        friend class vctPlot2DBase;
        friend class vctPlot2DBase::Scale;
        friend class vctPlot2DOpenGL;
        friend class vctPlot2DVTK;
    public:
        Signal(const std::string & name, size_t numberOfPoints, size_t pointDimension = 2);
        ~Signal();

        // see AppendPoint
        void CISST_DEPRECATED AddPoint(const vctDouble2 & point);

        /*! Insert point at last position and move last position
          forward.  If the circular buffer is full, this methods
          overwrite the first element. */
        void AppendPoint(const vctDouble2 & point);

        /*! Get point value in the buffer relative to first element.
          This method will throw an std::runtime_error if the index is
          invalid, i.e. greater than the buffer size. */
        vctDouble2 GetPointAt(size_t index) CISST_THROW(std::runtime_error);

        /*! Set point value at a given position, relative to first
          element.  This method will throw an std::runtime_error if
          the index is invalid, i.e. greater than the buffer size. */
        void SetPointAt(size_t index, const vctDouble2 & point) CISST_THROW(std::runtime_error);

        /*! Replaces "size" points data starting at "index".  This
          methods assumes that the point size of the user provided
          buffer matches the internal buffer.  If either the point
          size, index or size of user provided data is incorrect, an
          exception is thrown. */
        void SetArrayAt(size_t index, const double * pointArray, size_t arraySize, size_t pointDimension = 2) CISST_THROW(std::runtime_error);

        /*! Prepend user provided data at the beginning of the
          circular buffer.  If the buffer is full or doesn't have
          enough free space, data will be overwritten at the end of
          buffer.  This method will throw an exception if either the
          array size or index is invalid. */
        bool PrependArray(const double * pointArray, size_t arraySize, size_t pointDimension = 2) CISST_THROW(std::runtime_error);

        /*! Append user provided data at the end of the circular
          buffer.  If the buffer is full or doesn't have enough free
          space, data will be overwritten at the beginning of buffer.
          This method will throw an exception if either the array size
          or index is invalid. */
        bool AppendArray(const double * pointArray, size_t arraySize, size_t pointDimension = 2) CISST_THROW(std::runtime_error);

        void Freeze(bool freeze);
        bool GetFreeze(void) const;

        /*! Compute min and max over all points to recenter display.
            If the buffer contains no point, returns 0. */
        //@{
        void ComputeDataRangeX(double & min, double & max, bool assumesDataSorted = false) const;
        void ComputeDataRangeY(double & min, double & max) const;
        void ComputeDataRangeXY(vctDouble2 & min, vctDouble2 & max) const;
        //@}

        void CISST_DEPRECATED SetNumberOfPoints(size_t numberOfPoints);
        void CISST_DEPRECATED GetNumberOfPoints(size_t & numberOfPoints, size_t & bufferSize) const;

        /*! Get size of circular buffer. */
        size_t GetSize(void) const;

        /*! Get number of points. */
        size_t GetNumberOfPoints(void) const;

        void SetColor(const vctDouble3 & colorInRange0To1);

        /*! Destructive resize, this method resize the circular buffer
          and sets the first and last pointers to the buffer's
          beginning. */
        void SetSize(size_t numberOfPoints);

        bool IsVisible(void) const;
        void SetVisible(const bool visible);

        /*! Non destructive resize.  If the new buffer is bigger,
          preserves all points.  If the new buffer is smaller,
          preserves the data at the end by default. */
        void Resize(size_t numberOfPoints, bool trimOlder = true);

        /* Helper Function */
        vctPlot2DBase::Signal * AddSignal(const std::string & name);
        vctPlot2DBase::Scale * GetParent(void);
        const std::string & GetName(void) const;

    protected:
        Scale * Parent;
        /*! Signal name, used for GUI */
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

    class CISST_EXPORT Scale
    {
        friend class vctPlot2DBase;
        friend class vctPlot2DOpenGL;
        friend class vctPlot2DVTK;
    public:

        // keep signals in a std::map
        typedef std::map<std::string, Signal *> SignalsType;
        SignalsType Signals;

        // keep vertical lines in a std::map
        typedef std::map<std::string, VerticalLine *> VerticalLinesType;
        VerticalLinesType VerticalLines;

        Scale(const std::string & name, size_t pointDimension = 2);
        ~Scale();

        const std::string & GetName(void) const;

        vctPlot2DBase::Signal * AddSignal(const std::string & name);
        bool RemoveSignal(const std::string & name);
        bool RemoveSignal(const Signal * signal);

        inline const SignalsType & GetSignals(void) const {
            return Signals;
        }

        vctPlot2DBase::VerticalLine * AddVerticalLine(const std::string & name);

        void SetColor(const vctDouble3 & colorInRange0To1);

        void Freeze(bool freeze);
        bool GetFreeze(void) const;

        void ContinuousUpdate(void);
        /*! Set options to tell Continuous update how to recenter the
          scale based on the signals. */
        //@{
        void SetContinuousFitX(bool fit);
        void SetContinuousFitY(bool fit);
        void SetContinuousExpandY(bool expand);
        //@}
        /*! Query automatic recentering settings. */
        //@{
        inline bool GetContinuousFitX(void) const {
            return ContinuousFitX;
        }
        inline bool GetContinuousFitY(void) const {
            return ContinuousFitY;
        }
        inline bool GetContinuousExpandY(void) const {
            return ContinuousExpandY;
        }
        const vctDouble2 & GetViewingRangeX(void) {
            return this->ViewingRangeX;
        }
        const vctDouble2 & GetViewingRangeY(void) {
            return this->ViewingRangeY;
        }
        //@}

        /*! To fit the data in the viewport we need to compute the
          range for all signals.  To reduce the number of
          computations, three methods are provided, one that compute
          the X range only, one for the Y range only and one for both
          X and Y.  Each method calls the corresponding compute data
          range for all signals in the scale. */
        //@{
        bool ComputeDataRangeX(double & min, double & max, bool assumesDataSorted = false) const;
        bool ComputeDataRangeY(double & min, double & max);
        bool ComputeDataRangeXY(vctDouble2 & min, vctDouble2 & max);
        //@}

        /*! Data recentering, these methods re-align the data once
          only, based on all signals.  Padding is used to make sure
          the data is not plotted at the extreme edges of the window.
          The padding parameter indicates the percentage of space that
          should be left empty.  For example, a 200 pixel window with
          a padding of 0.1 (10%) will leave a band of 10 pixels empty
          on each side of the window.  By default, the padding in X
          (horizontal) is null and 10% (0.1) in Y (vertical). */
        //@{
        void AutoFitX(double padding = 0.0);
        void FitX(double min, double max, double padding = 0.0);
        void AutoFitY(double padding = 0.1);
        void AutoExpandY(double padding = 0.1);
        void FitY(double min, double max, double padding = 0.1);
        void AutoFitXY(const vctDouble2 & padding = vctDouble2(0.0, 0.1));
        void AutoFitXExpandY(const vctDouble2 & padding = vctDouble2(0.0, 0.1));
        void FitXY(vctDouble2 min, vctDouble2 max, const vctDouble2 & padding = vctDouble2(0.0, 0.1));
        //@}

    protected:
        bool ContinuousFitX;
        bool ContinuousFitY;
        bool ContinuousExpandY;
        double ExpandYMin, ExpandYMax;

        // viewport sizes
        vctDouble2 Viewport;
        // stores the min and max corresponding to the viewport
        vctDouble2 ViewingRangeX, ViewingRangeY;
        vctDouble2 Translation;
        vctDouble2 ScaleValue;


    private:
        std::string Name;
        size_t PointSize;
        vctDouble3 Color;
        double LineWidth;
    };

    /*! Type used to store all scales used by the plot.  Each scale
      can contain multiple signals, lines, ... that will be scaled
      together. */
    typedef std::map<std::string, Scale *> ScalesType;
    ScalesType Scales;

    /*! Create a new scale and return a pointer to the newly created
      scale.  If a scale with the same name already exists the method
      returns a pointer to the existing scale. */
    vctPlot2DBase::Scale * AddScale(const std::string & name);

    /*! Find an existing scale by name. */
    vctPlot2DBase::Scale * FindScale(const std::string & name);

    /*! Remove an existing scale by name. */
    bool RemoveScale(const std::string & name);

    /*! Remove an existing scale by address. */
    bool RemoveScale(const vctPlot2DBase::Scale * scale);

    /*! Reference to the map of scales. */
    inline ScalesType & GetScales(void) {
        return Scales;
    }

    vctPlot2DBase(size_t PointSize = 2);
    virtual ~vctPlot2DBase();

    /*! Set the number of points for all signals. */
    void SetNumberOfPoints(size_t numberOfPoints);

    /*! This method is now deprecated.  The user should first use
      AddScale and then AddSignal on the newly created scale.  Create
      a new signal.  This method checks if the name has already been
      used.  If so, the signal won't be added and the method returns a
      0 pointer. */
    Signal CISST_DEPRECATED * AddSignal(const std::string & name);

    /*! This method is now deprecated.  The user should first use
      AddScale and then AddVecticalLine on the newly created scale.
      Create a new vertical line.  This method checks if the name has
      already been used.  If so, the line won't be added and the
      method returns a 0 pointer. */
    VerticalLine CISST_DEPRECATED * AddVerticalLine(const std::string & name);

    /*! Data recentering, these methods re-align the data once only,
      based on all signals.  Padding is used to make sure the data is
      not plotted at the extreme edges of the window.  The padding
      parameter indicates the percentage of space that should be left
      empty.  For example, a 200 pixel window with a padding of 0.1
      (10%) will leave a band of 10 pixels empty on each side of the
      window.  By default, the padding in X (horizontal) is null and
      10% (0.1) in Y (vertical). */
    //@{
    void AutoFitX(double padding = 0.0);
    void FitX(double min, double max, double padding = 0.0);
    void AutoFitY(double padding = 0.1);
    void FitY(double min, double max, double padding = 0.1);
    //@}

    /*! Freeze the circular buffers, i.e. AddPoint does nothing.  When
      turned off (parameter is false), this is equivalent to starting
      with an empty data set.  These methods work on all signals at
      once. */
    void Freeze(bool freeze);

    /*! Check if all signals are frozen. */
    bool GetFreeze(void) const;

    /*! Automatic recentering.  Propagate settings to all scales. */
    //@{
    void SetContinuousFitX(bool fit);
    void SetContinuousFitY(bool fit);
    void SetContinuousExpandY(bool expand);
    //@}

    /*! Check if all scales are on continous fit for X. */
    bool GetContinuousFitX(void) const;

    /*! Check if all scales are on continous fit for Y. */
    bool GetContinuousFitY(void) const;

    /*! Check if all scales are on continous fit for X and Y. */
    bool GetContinuousExpandY(void) const;

    //*! Get currently used viewing range */
    //@{
    inline const vctDouble2 & GetViewingRangeX(void) const {
        return this->ViewingRangeX;
    }
    inline const vctDouble2 & GetViewingRangeY(void) const {
        return this->ViewingRangeY;
    }
    //@}

    /*! Set background color, defined as RGB between 0 and 1. */
    void SetBackgroundColor(const vctDouble3 & colorInRange0To1);

 protected:

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

    // default number of points for all signals
    size_t NumberOfPoints;

    // viewport sizes
    vctDouble2 Viewport;
    // stores the min and max corresponding to the viewport
    vctDouble2 ViewingRangeX, ViewingRangeY;

    /*! Method called before each iteration to figure out if an
      automatic update is needed or not for each scale. */
    void ContinuousUpdate(void);

    // background color
    vctDouble3 BackgroundColor;
};

#endif  // _vctPlot2DBase_h
