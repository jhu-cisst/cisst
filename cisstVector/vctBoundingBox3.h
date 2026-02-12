/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Seth Billings, Russell Taylor, Anton Deguet
  Created on:	2003-08-18

  (C) Copyright 2014-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _vctBoundingBox3_h
#define _vctBoundingBox3_h

#include <cisstVector/vctFixedSizeVectorTypes.h>

// Always include last!
#include <cisstVector/vctExport.h>

template <typename _elementType>
class vctBoundingBox3Base
{
public:
    typedef _elementType value_type;
    typedef vctBoundingBox3Base<value_type> ThisType;
    typedef vctFixedSizeVector<value_type, 3> PointType;

protected:
    PointType mMinCorner;
    PointType mMaxCorner;
    size_t mCounter;

public:

    /*! Default constructor.  Initializes the corners to infinite
      values and set the counter to 0. */
    vctBoundingBox3Base(void):
        mMinCorner(HUGE_VAL, HUGE_VAL, HUGE_VAL)
        , mMaxCorner(-HUGE_VAL, -HUGE_VAL, -HUGE_VAL)
        , mCounter(0)
        {};

    /*! Creates a bounding box from two points.  These points can be
      the min and max corner. */
    vctBoundingBox3Base(const PointType & pointA, const PointType & pointB)
    {
        Reset(pointA);
        Expand(pointB);
    }

    /*! Copy constructor */
    vctBoundingBox3Base(const ThisType & other):
        mMinCorner(other.mMinCorner)
        , mMaxCorner(other.mMaxCorner)
        , mCounter(other.mCounter)
        {};

    ThisType & operator=(const ThisType & other) {
        mMinCorner = other.mMinCorner;
        mMaxCorner = other.mMaxCorner;
        mCounter = other.mCounter;
        return *this;
    }

    /*! Resets the bounding box with a single point.  Use the method
      Include or Expand to expand.  See also Reset(const PointType
      &). */
    ThisType & operator=(const PointType & point) {
        Reset(point);
        return *this;
    }

    /*! Reset the bounding box.  See default constructor. */
    void Reset(void) {
        mMinCorner.SetAll(HUGE_VAL);
        mMaxCorner.SetAll(-HUGE_VAL);
        mCounter = 0;
    }

    /*! Reset with a starting point.  This allows to skip a Reset()
      call without parameters which would lead to an extra assign
      HUGE_VAL to the min and max corners. */
    void Reset(const PointType & point) {
        mMinCorner = mMaxCorner = point;
        mCounter = 1;
    }

    /*! Reset a bounding box using two points. */
    void Reset(const PointType & pointA, const PointType & pointB) {
        Reset(pointA);
        Expand(pointB);
    }

    /*! Get the minimum corner.  This accessor is "const", one can not change the min corner values directly. */
    const PointType & MinCorner(void) const {
        return mMinCorner;
    }

    /*! Get the maximum corner.  This accessor is "const", one can not change the max corner values directly. */
    const PointType & MaxCorner(void) const {
        return mMaxCorner;
    }

    /*! Get the counter value, i.e. number of points used to define
      the bounding box.  The counter is set to zero when the bounding
      box is undefined (i.e. empty).  If the counter is set to 1, the
      min and max corners are equal.  If the counter is greater than
      one, min and max corners are defined and potentially equal. */
    size_t Counter(void) const {
        return mCounter;
    }

    /*! Get the vector max - min, elements of the diagonal are
      always positive */
    PointType Diagonal(void) const {
        return mMaxCorner - mMinCorner;
    }


    double DiagonalLength(void) const {
        return Diagonal().Norm();
    }

    /*! Get the center (aka mid-point) of the bounding box. */
    PointType MidPoint(void) const {
        return (mMinCorner + mMaxCorner) * 0.5;
    }

    /*! Deprecated, use Expand instead */
    ThisType & Include(const PointType & point) {
        if (mMinCorner[0] > point[0]) mMinCorner[0] = point[0];
        if (mMinCorner[1] > point[1]) mMinCorner[1] = point[1];
        if (mMinCorner[2] > point[2]) mMinCorner[2] = point[2];
        if (mMaxCorner[0] < point[0]) mMaxCorner[0] = point[0];
        if (mMaxCorner[1] < point[1]) mMaxCorner[1] = point[1];
        if (mMaxCorner[2] < point[2]) mMaxCorner[2] = point[2];
        mCounter++;
        return *this;
    }

    /*! */
    ThisType & Include(const ThisType & other) {
        if (mMinCorner[0] > other.mMinCorner[0]) mMinCorner[0] = other.mMinCorner[0];
        if (mMinCorner[1] > other.mMinCorner[1]) mMinCorner[1] = other.mMinCorner[1];
        if (mMinCorner[2] > other.mMinCorner[2]) mMinCorner[2] = other.mMinCorner[2];
        if (mMaxCorner[0] < other.mMaxCorner[0]) mMaxCorner[0] = other.mMaxCorner[0];
        if (mMaxCorner[1] < other.mMaxCorner[1]) mMaxCorner[1] = other.mMaxCorner[1];
        if (mMaxCorner[2] < other.mMaxCorner[2]) mMaxCorner[2] = other.mMaxCorner[2];
        mCounter += other.mCounter;
        return *this;
    }


    /*! Expand the bounding box using a single point.  Return true iff
      the bounding box corners have been updated. */
    //@{
    bool Expand(const value_type x, const value_type y, const value_type z) {
        bool expanded = false;
        if (x < mMinCorner.X()) {
            mMinCorner.X() = x; expanded = true;
        } else if (x > mMaxCorner.X()) {
            mMaxCorner.X() = x; expanded = true;
        }
        if (y < mMinCorner.Y()) {
            mMinCorner.Y() = y; expanded = true;
        } else if (y > mMaxCorner.Y()) {
            mMaxCorner.Y() = y; expanded = true;
        }
        if (z < mMinCorner.Z()) {
            mMinCorner.Z() = z; expanded = true;
        } else if (z > mMaxCorner.Z()) {
            mMaxCorner.Z() = z; expanded = true;
        }
        mCounter++;
        return expanded;
    }

    bool Expand(const PointType & point) {
        return Expand(point.X(), point.Y(), point.Z());
    }
    //@}

    /*! Check if a bounding box intersects (overlaps) with this bounding box. */
    bool Intersect(const ThisType & other) const {
        // intersect if the max > other.min and min < other.max
        if (mMaxCorner[0] >= other.mMinCorner[0]
            && mMaxCorner[1] >= other.mMinCorner[1]
            && mMaxCorner[2] >= other.mMinCorner[2]
            && mMinCorner[0] <= other.mMaxCorner[0]
            && mMinCorner[1] <= other.mMaxCorner[1]
            && mMinCorner[2] <= other.mMaxCorner[2]) {
            return true;
        } else {
            return false;
        }
    }

    /*! Enlarge the bounding box by a constant value in all directions. */
    void EnlargeBy(const double dist) {
        if (mCounter != 0) {
            mMinCorner -= dist;
            mMaxCorner+= dist;
        }
    }

    /*! Check if a point is included in this bounding box, +/- a
      certain distance. */
    int Includes(const PointType & point, const double dist = 0.0) const
    {
        if (point[0] + dist < mMinCorner[0]) return 0;
        if (point[1] + dist < mMinCorner[1]) return 0;
        if (point[2] + dist < mMinCorner[2]) return 0;
        if (point[0] - dist > mMaxCorner[0]) return 0;
        if (point[1] - dist > mMaxCorner[1]) return 0;
        if (point[2] - dist > mMaxCorner[2]) return 0;
        return 1; // point is within distance "dist" of this bounding box
    }

    /*! Check if a bounding box is fully included in this bounding
      box, +/- a certain distance. */
    int Includes(const ThisType & other, const double dist = 0.0) const
    {
        return Includes(other.mMinCorner, dist) && Includes(other.mMaxCorner, dist);
    }

};

// typedefs for convenience
typedef vctBoundingBox3Base<double> vctBoundingBox3;
typedef vctBoundingBox3Base<double> vctBoundingBoxDouble3;
typedef vctBoundingBox3Base<float> vctBoundingBoxFloat3;

#endif // _vctBoundingBox3_h
