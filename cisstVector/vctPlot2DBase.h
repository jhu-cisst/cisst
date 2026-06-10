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
#include <Eigen/Dense>
#include <memory>
#include <string>

#include <cisstCommon/cmnPortability.h>

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
      display in a contiguous circular buffer.  */
    class CISST_EXPORT Signal
    {
        friend class vctPlot2DBase;
        friend class vctPlot2DBase::Scale;
        friend class vctPlot2DOpenGL;

    public:
        Signal(const std::string & name, size_t numberOfPoints);
        ~Signal();

        /*! Insert point at last position and move last position
          forward.  If the circular buffer is full, this methods
          overwrite the first element. */
        void AppendPoint(const Eigen::Vector2d& point);

        /*! Get point value in the buffer relative to first element.
          This method will throw an std::runtime_error if the index is
          invalid, i.e. greater than the buffer size. */
        Eigen::Vector2d GetPointAt(size_t index) CISST_THROW(std::runtime_error);

        /*! Set point value at a given position, relative to first
          element.  This method will throw an std::runtime_error if
          the index is invalid, i.e. greater than the buffer size. */
        void SetPointAt(size_t index, const Eigen::Vector2d& point) CISST_THROW(std::runtime_error);

        void Freeze(bool freeze);
        bool GetFreeze(void) const;

        /*! Compute min and max over all points to recenter display.
            If the buffer contains no point, returns 0. */
        //@{
        void ComputeDataRangeX(double & min, double & max, bool assumesDataSorted = false) const;
        void ComputeDataRangeY(double & min, double & max) const;
        void ComputeDataRangeXY(Eigen::Vector2d& min, Eigen::Vector2d& max) const;
        //@}

        /*! Get size of circular buffer. */
        size_t GetSize(void) const;

        /*! Get number of points. */
        size_t GetNumberOfPoints(void) const;

        void SetColor(const Eigen::Vector3d& colorInRange0To1);

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
        std::string Name;

        bool Empty;
        bool Visible;
        bool Frozen;
        
        Eigen::Vector3d Color;
        double LineWidth;

        // data series is stored as circular buffer in a 2xN matrix, where N is maximum number of points
        Eigen::Matrix2Xd data;

        // first/last (inclusive) valid entries in the circular buffer
        Eigen::Index IndexFirst;
        Eigen::Index IndexLast;
    };

    /*! Storage for a given vertical line. */
    class CISST_EXPORT VerticalLine
    {
        friend class vctPlot2DBase;
        friend class vctPlot2DOpenGL;

    public:
        VerticalLine(const std::string & name, const double x = 0.0);

        void SetX(const double x);
        void SetColor(const Eigen::Vector3d& colorInRange0To1);

    protected:
        std::string Name;
        double X;
        bool Visible;
        Eigen::Vector3d Color;
        double LineWidth;
    };

    class CISST_EXPORT Scale
    {
        friend class vctPlot2DBase;
        friend class vctPlot2DOpenGL;

    public:
        // keep signals in a std::map
        typedef std::map<std::string, std::unique_ptr<Signal>> SignalsType;
        SignalsType Signals;

        // keep vertical lines in a std::map
        typedef std::map<std::string, std::unique_ptr<VerticalLine>> VerticalLinesType;
        VerticalLinesType VerticalLines;

        Scale(const std::string & name);
        ~Scale();

        const std::string & GetName(void) const;

        vctPlot2DBase::Signal * AddSignal(const std::string & name);
        bool RemoveSignal(const std::string & name);
        bool RemoveSignal(const Signal * signal);

        inline const SignalsType & GetSignals(void) const {
            return Signals;
        }

        vctPlot2DBase::VerticalLine * AddVerticalLine(const std::string & name);

        void SetColor(const Eigen::Vector3d& colorInRange0To1);

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
        const Eigen::Vector2d& GetViewingRangeX(void) {
            return this->ViewingRangeX;
        }
        const Eigen::Vector2d& GetViewingRangeY(void) {
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
        bool ComputeDataRangeXY(Eigen::Vector2d& min, Eigen::Vector2d& max);
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
        void AutoFitXY(const Eigen::Vector2d& padding = Eigen::Vector2d(0.0, 0.1));
        void AutoFitXExpandY(const Eigen::Vector2d& padding = Eigen::Vector2d(0.0, 0.1));
        void FitXY(Eigen::Vector2d min, Eigen::Vector2d max, const Eigen::Vector2d& padding = Eigen::Vector2d(0.0, 0.1));
        //@}

    protected:
        bool ContinuousFitX;
        bool ContinuousFitY;
        bool ContinuousExpandY;
        double ExpandYMin, ExpandYMax;

        // viewport sizes
        Eigen::Vector2d Viewport;
        // stores the min and max corresponding to the viewport
        Eigen::Vector2d ViewingRangeX, ViewingRangeY;
        Eigen::Vector2d Translation;
        Eigen::Vector2d ScaleValue;

    private:
        std::string Name;
        Eigen::Vector3d Color;
        double LineWidth;
    };

    /*! Type used to store all scales used by the plot.  Each scale
      can contain multiple signals, lines, ... that will be scaled
      together. */
    typedef std::map<std::string, std::unique_ptr<Scale>> ScalesType;
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

    vctPlot2DBase();

    /*! Set the number of points for all signals. */
    void SetNumberOfPoints(size_t numberOfPoints);

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
    inline const Eigen::Vector2d& GetViewingRangeX(void) const {
        return this->ViewingRangeX;
    }
    inline const Eigen::Vector2d& GetViewingRangeY(void) const {
        return this->ViewingRangeY;
    }
    //@}

    /*! Set background color, defined as RGB between 0 and 1. */
    void SetBackgroundColor(const Eigen::Vector3d& colorInRange0To1);

protected:

    /*! Methods required in all derived classes */
    //@{
    virtual void RenderInitialize(void) = 0;
    virtual void RenderResize(double width, double height) = 0;
    virtual void Render(void) = 0;
    //@}

protected:
    // default number of points for all signals
    size_t NumberOfPoints;

    // viewport sizes
    Eigen::Vector2d Viewport;
    // stores the min and max corresponding to the viewport
    Eigen::Vector2d ViewingRangeX, ViewingRangeY;

    /*! Method called before each iteration to figure out if an
      automatic update is needed or not for each scale. */
    void ContinuousUpdate(void);

    // background color
    Eigen::Vector3d BackgroundColor;
};

#endif  // _vctPlot2DBase_h
