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

#include <limits>

#include <cisstVector/vctPlot2DBase.h>
#include <cisstCommon/cmnUnits.h>
#include <cisstCommon/cmnAssert.h>

vctPlot2DBase::Scale::Scale(const std::string & name, size_t pointDimension):
    ExpandYMin(std::numeric_limits<double>::max()),
    ExpandYMax(std::numeric_limits<double>::min())
{
    this->PointSize = pointDimension;
    this->Translation.SetAll(0.0);
    this->ScaleValue.SetAll(1.0);
    this->Name = name;
    AutoFitXY(vct2(1.0));
    SetContinuousFitX(true);
    SetContinuousExpandY(true);
}

vctPlot2DBase::Scale::~Scale()
{
    {
        const SignalsType::iterator end = Signals.end();
        for (SignalsType::iterator iter = Signals.begin();
             iter != end; ++iter) {
            if (iter->second) {
                delete iter->second;
            }
        }
    }
    {
        const VerticalLinesType::iterator end = VerticalLines.end();
        for (VerticalLinesType::iterator iter = VerticalLines.begin();
             iter != end; ++iter) {
            if (iter->second) {
                delete iter->second;
            }
        }
    }
}

const std::string & vctPlot2DBase::Scale::GetName(void) const
{
    return this->Name;
}

void vctPlot2DBase::Scale::AutoFitX(double padding)
{
    double min, max;
    this->ComputeDataRangeX(min, max);
    this->FitX(min, max, padding);
}

void vctPlot2DBase::Scale::FitX(double min, double max, double padding)
{
    this->ViewingRangeX.Assign(min, max);
    this->ScaleValue.X() = this->Viewport.X() / ((max - min) * (1.0 + padding));
    this->Translation.X() =
        - min * this->ScaleValue.X()
        + 0.5 * padding * this->Viewport.X();
}

void vctPlot2DBase::Scale::AutoFitY(double padding)
{
    double min, max;
    this->ComputeDataRangeY(min, max);
    this->FitY(min, max, padding);
}

void vctPlot2DBase::Scale::AutoExpandY(double padding)
{
    double min, max;
    this->ComputeDataRangeY(min, max);
    if (min < this->ExpandYMin) {
        this->ExpandYMin = min;
    }
    if (max > this->ExpandYMax) {
        this->ExpandYMax = max;
    }
    this->FitY(min, max, padding);
}

void vctPlot2DBase::Scale::FitY(double min, double max, double padding)
{
    this->ViewingRangeY.Assign(min, max);
    this->ScaleValue.Y() = this->Viewport.Y() / ((max - min) * (1.0 + padding));
    this->Translation.Y() =
        - min * this->ScaleValue.Y()
        + 0.5 * padding * this->Viewport.Y();
}

void vctPlot2DBase::Scale::AutoFitXY(const vctDouble2 & padding)
{
    vctDouble2 min, max;
    this->ComputeDataRangeXY(min, max);
    this->FitXY(min, max, padding);
}

void vctPlot2DBase::Scale::FitXY(vctDouble2 min, vctDouble2 max, const vctDouble2 & padding)
{
    this->ViewingRangeX.Assign(min.X(), max.X());
    this->ViewingRangeY.Assign(min.Y(), max.Y());
    // compute scale
    vctDouble2 dataDiff;
    dataDiff.DifferenceOf(max, min);
    this->ScaleValue.ElementwiseRatioOf(this->Viewport, dataDiff);
    vctDouble2 pad(padding);
    pad.Add(1.0);
    this->ScaleValue.ElementwiseDivide(pad);
    // compute translation
    this->Translation.ElementwiseProductOf(min, this->ScaleValue);
    this->Translation.NegationSelf();
    pad.ElementwiseProductOf(padding, this->Viewport);
    pad.Multiply(0.5);
    this->Translation.Add(pad);
}

void vctPlot2DBase::Scale::AutoFitXExpandY(const vctDouble2 & padding)
{
    vctDouble2 min, max;
    this->ComputeDataRangeXY(min, max);
    if (min.Y() < this->ExpandYMin) {
        this->ExpandYMin = min.Y();
    } else {
        min.Y() = this->ExpandYMin;
    }
    if (max.Y() > this->ExpandYMax) {
        this->ExpandYMax = max.Y();
    } else {
        max.Y() = this->ExpandYMax;
    }
    this->FitXY(min, max, padding);
}

bool vctPlot2DBase::Scale::ComputeDataRangeX(double & min, double & max, bool assumesDataSorted) const
{
    double rmin = std::numeric_limits<double>::max();
    double rmax = std::numeric_limits<double>::min();
    // if we have at least one signal
    if (!Signals.empty()) {
        const SignalsType::const_iterator end = Signals.end();
        SignalsType::const_iterator signal = Signals.begin();
        for (; signal != end; ++signal) {
            if (signal->second->IsVisible()) {
                signal->second->ComputeDataRangeX(min, max, assumesDataSorted);
                rmin = (rmin > min) ? min : rmin;
                rmax = (rmax < max) ? max : rmax;
            }
        }
        min = rmin;
        max = rmax;
        return true;
    }
    // empty scale
    min = -1.0;
    max = 1.0;
    return false;
}

bool vctPlot2DBase::Scale::ComputeDataRangeY(double & min, double & max)
{
    double rmin = std::numeric_limits<double>::max();
    double rmax = std::numeric_limits<double>::min();
    // if we have at least one signal
    if (!Signals.empty()) {
        const SignalsType::const_iterator end = Signals.end();
        SignalsType::const_iterator signal = Signals.begin();
        for (; signal != end; ++signal) {
            if (signal->second->IsVisible()) {
                signal->second->ComputeDataRangeY(min, max);
                rmin = (rmin > min) ? min : rmin;
                rmax = (rmax < max) ? max : rmax;
            }
        }
        min = rmin;
        max = rmax;
        return true;
    }
    // empty scale
    min = -1.0;
    max = 1.0;
    return false;
}

bool vctPlot2DBase::Scale::ComputeDataRangeXY(vctDouble2 & min, vctDouble2 & max)
{
    vctDouble2 rmin, rmax;
    rmin.SetAll(std::numeric_limits<double>::max());
    rmax.SetAll(std::numeric_limits<double>::min());
    // if we have at least one signal
    if (!Signals.empty()) {
        const SignalsType::const_iterator end = Signals.end();
        SignalsType::const_iterator signal = Signals.begin();
        for (; signal != end; ++signal) {
            if (signal->second->IsVisible()) {
                signal->second->ComputeDataRangeXY(min, max);
                rmin.X() = (rmin.X() > min.X()) ? min.X():rmin.X();
                rmin.Y() = (rmin.Y() > min.Y()) ? min.Y():rmin.Y();
                rmax.X() = (rmax.X() < max.X()) ? max.X() : rmax.X();
                rmax.Y() = (rmax.Y() < max.Y()) ? max.Y() : rmax.Y();
            }
        }
        min = rmin;
        max = rmax;
        return true;
    }
    // empty scale
    min.SetAll(-1.0);
    max.SetAll(1.0);
    return false;
}

void vctPlot2DBase::Scale::SetContinuousFitX(bool fit)
{
    this->ContinuousFitX = fit;
}

void vctPlot2DBase::Scale::SetContinuousFitY(bool fit)
{
    this->ContinuousExpandY = false;
    this->ContinuousFitY = fit;
}

void vctPlot2DBase::Scale::SetContinuousExpandY(bool expand)
{
    this->ContinuousFitY = false;
    this->ContinuousExpandY = expand;
    // reset Y range
    this->ExpandYMin = std::numeric_limits<double>::max();
    this->ExpandYMax = std::numeric_limits<double>::min();
}

void vctPlot2DBase::Scale::Freeze(bool freeze)
{
    const SignalsType::const_iterator end = Signals.end();
    SignalsType::const_iterator signal = Signals.begin();
    for (; signal != end; ++signal) {
        signal->second->Freeze(freeze);
    }
}

bool vctPlot2DBase::Scale::GetFreeze(void) const
{
    bool allFrozen = true;
    const SignalsType::const_iterator end = Signals.end();
    for (SignalsType::const_iterator signal = Signals.begin();
         signal != end;
         ++signal) {
        allFrozen = allFrozen && signal->second->GetFreeze();
    }
    return allFrozen;
}

void vctPlot2DBase::Scale::SetColor(const vctDouble3 & colorInRange0To1)
{
    vctDouble3 clippedColor;
    clippedColor.ClippedAboveOf(colorInRange0To1, 1.0);
    clippedColor.ClippedBelowOf(clippedColor, 0.0);
    this->Color.Assign(clippedColor);
}

vctPlot2DBase::Signal * vctPlot2DBase::Scale::AddSignal(const std::string & name)
{
    // check if the name already exists
    SignalsType::const_iterator found = Signals.find(name);
    if (found == Signals.end()) {
        // new signal
        Signal * newSignal = new Signal(name, 100, this->PointSize);
        Signals[name] = newSignal;
        return newSignal;
    }
    // existing signal
    return 0;
}

bool vctPlot2DBase::Scale::RemoveSignal(const std::string & name)
{
    SignalsType::iterator found = Signals.find(name);
    if (found != Signals.end()) {
        Signal * signal = found->second;
        Signals.erase(found);
        delete signal;
        return true;
    }
    return false;
}

bool vctPlot2DBase::Scale::RemoveSignal(const Signal * signal)
{
    if (signal) {
        return this->RemoveSignal(signal->GetName());
    }
    return false;
}

vctPlot2DBase::VerticalLine * vctPlot2DBase::Scale::AddVerticalLine(const std::string & name)
{
    // check if the name already exists
    VerticalLinesType::const_iterator found = VerticalLines.find(name);
    if (found == VerticalLines.end()) {
        // new line
        VerticalLine * newVerticalLine = new VerticalLine(name);
        VerticalLines[name] = newVerticalLine;
        return newVerticalLine;
    }
    // existing line
    return 0;
}

void vctPlot2DBase::Scale::ContinuousUpdate(void)
{
    if (this->ContinuousFitX) {
        if (this->ContinuousFitY) {
            this->AutoFitXY();
        } else if (this->ContinuousExpandY) {
            this->AutoFitXExpandY();
        } else {
            this->AutoFitX();
        }
    } else {
        if (this->ContinuousFitY) {
            this->AutoFitY();
        } else if (this->ContinuousExpandY) {
            this->AutoExpandY();
        }
    }
}

vctPlot2DBase::Signal::Signal(const std::string & name, size_t numberOfPoints, size_t PointSize):
    Name(name),
    Empty(true),
    Visible(true),
    Frozen(false),
    Buffer(0),
    PointSize(PointSize),
    IndexFirst(0),
    IndexLast(0),
    Color(1.0, 1.0, 1.0),
    LineWidth(1.0)
{
    // create the underlaying buffer and fill it with zeros
    CMN_ASSERT(PointSize >= 2);
    this->Buffer = new double[PointSize * numberOfPoints];
    memset(this->Buffer, 0, this->PointSize * numberOfPoints * sizeof(double));
    this->Data.SetSize(numberOfPoints);
    // now set all the references
    size_t index;
    for (index = 0;
         index < numberOfPoints;
         ++index) {
        this->Data.Element(index).SetRef(this->Buffer + PointSize * index);
    }
}

vctPlot2DBase::Signal::~Signal()
{
    if (this->Buffer) {
        delete[] this->Buffer;
    }
}

vctPlot2DBase::Signal * vctPlot2DBase::Signal::AddSignal(const std::string & name)
{
    return Parent->AddSignal(name);
}

vctPlot2DBase::Scale * vctPlot2DBase::Signal::GetParent(void)
{
    return Parent;
}

const std::string & vctPlot2DBase::Signal::GetName(void) const
{
    return this->Name;
}

void vctPlot2DBase::Signal::AppendPoint(const vctDouble2 & point)
{
    if (!this->Frozen) {
        // look where to store this point
        if (!this->Empty) {
            this->IndexLast++;
            if (this->IndexLast == this->Data.size()) {
                this->IndexLast = 0;
            }
            if (this->IndexFirst == this->IndexLast ) {
                this->IndexFirst++;
                if (this->IndexFirst == this->Data.size()) {
                    this->IndexFirst = 0;
                }
            }
        } else {
            this->Empty = false;
        }
        this->Data.Element(IndexLast).Assign(point);
    }
}

vctDouble2 vctPlot2DBase::Signal::GetPointAt(size_t index)
    CISST_THROW(std::runtime_error)
{
    if (index >= Data.size()) {
        cmnThrow("vctPlot2DBase::Signal::GetPointAt: index bigger than bufferSize");
    }
    index = ((index + this->IndexFirst) % Data.size());
    return Data.at(index);
}

void vctPlot2DBase::Signal::SetPointAt(size_t index, const vctDouble2 & point)
    CISST_THROW(std::runtime_error)
{
    if (index >= Data.size()) {
        cmnThrow("vctPlot2DBase::Signal::SetPointAt: index bigger than bufferSize");
    }
    index = ((index + this->IndexFirst) % Data.size());
    this->Data.Element(index).Assign(point);
    return;
}

void vctPlot2DBase::Signal::SetArrayAt(size_t index, const double * pointArray, size_t size, size_t pointDimension)
    CISST_THROW(std::runtime_error)
{
    if (((index + (size/this->PointSize) ) >= Data.size())
        && (pointDimension != this->PointSize)) {
        cmnThrow("vctPlot2DBase::Signal::SetArrayAt: index + size bigger than bufferSize");
    }
    memcpy((this->Buffer + index * this->PointSize),
           pointArray,
           size * sizeof(double));
    return;
}

bool vctPlot2DBase::Signal::AppendArray(const double * pointArray, size_t arraySize, size_t pointDimension)
    CISST_THROW(std::runtime_error)
{
    bool result = false;

    // check if there is enough space for array we want to insert
    if ((arraySize > (Data.size()*this->PointSize))
        || (pointDimension != this->PointSize)) {
        cmnThrow("vctPlot2DBase::Signal::AppendArray: arraySize invalid or pointDimension not match");
    }

    double * newBuffer = new double [Data.size() * this->PointSize];
    memset(newBuffer, 0, Data.size() * this->PointSize * sizeof(double));

    if (!this->Frozen) {
        size_t tempIndexLast = this->IndexLast, tempIndexFirst = this->IndexFirst;
        if (!this->Empty){
            if(tempIndexLast < tempIndexFirst ){
                // buffer full, move tempIndexFirst for pointArray
                tempIndexFirst += arraySize / this->PointSize;
                if (tempIndexFirst > Data.size()) {
                    tempIndexFirst = tempIndexFirst - Data.size() - 1;
                }
            } else {
                size_t sizeToMove = 0;
                sizeToMove = (Data.size() - this->IndexLast - 1);
                sizeToMove = (sizeToMove > (arraySize / this->PointSize)) ?
                    0 :
                    ((arraySize / this->PointSize) - sizeToMove);
                tempIndexFirst = tempIndexFirst + sizeToMove;
                if (tempIndexFirst == Data.size()) {
                    this->Empty = true;
                }
            }
        }
        size_t dataCopied = 0;
        if (!this->Empty) {
            if (tempIndexLast < tempIndexFirst){
                memcpy((newBuffer),
                       Buffer + (tempIndexFirst * this->PointSize),
                       (Data.size() - tempIndexFirst) * this->PointSize * sizeof(double));
                dataCopied = (Data.size() - tempIndexFirst);
                memcpy((newBuffer + (dataCopied * this->PointSize)),
                       Buffer,
                       (tempIndexLast + 1) * this->PointSize * sizeof(double));
                dataCopied += (tempIndexLast + 1);
            } else {
                memcpy((newBuffer),
                       Buffer + (tempIndexFirst * this->PointSize),
                       (tempIndexLast - tempIndexFirst +  1) * this->PointSize*sizeof(double));
                dataCopied = (tempIndexLast - tempIndexFirst + 1);
            }
        }
        memcpy((newBuffer + (dataCopied * this->PointSize)),
               pointArray,
               arraySize * sizeof(double));
        dataCopied += (arraySize / this->PointSize);
        delete Buffer;
        Buffer = newBuffer;
        for (size_t i = 0;
             i < Data.size();
             i++) {
            this->Data.Element(i).SetRef(this->Buffer + this->PointSize * i);
        }
        this->IndexFirst = 0;
        this->IndexLast = dataCopied - 1;
        this->Empty = false;
        result = true;
    }
    return result;
}

bool vctPlot2DBase::Signal::PrependArray(const double * pointArray, size_t arraySize, size_t pointDimension)
    CISST_THROW(std::runtime_error)
{
    // check if there is enough space for array we want to insert
    bool result = false;

    if ((arraySize > (Data.size()*this->PointSize))
        || (pointDimension != this->PointSize)) {
        cmnThrow("vctPlot2DBase::Signal::PrependArray: arraySize invalid or pointDimension not match");
    }

    if (!this->Frozen) {
        ptrdiff_t tempIndexLast = this->IndexLast;
        ptrdiff_t tempIndexFirst = this->IndexFirst;
        double * newBuffer = new double [Data.size()*this->PointSize];
        memset(newBuffer, 0, Data.size() * this->PointSize * sizeof(double));
        if (!this->Empty) {
            if (tempIndexLast < tempIndexFirst ) {
                // buffer full, move tempIndexFirst for pointArray
                tempIndexLast -= arraySize / this->PointSize;
                if (tempIndexLast < 0) {
                    tempIndexLast = Data.size() + tempIndexLast;
                }
            } else {
                size_t sizeToMove = 0;
                sizeToMove = (Data.size() - this->IndexLast - 1);
                sizeToMove = (sizeToMove > (arraySize / this->PointSize)) ?
                    0:
                    ((arraySize / this->PointSize) - sizeToMove);
                tempIndexLast = tempIndexLast - sizeToMove;
                if (tempIndexLast < 0) {
                    this->Empty = true;
                }
            }
        }
        size_t dataCopied = 0;
        if (!this->Empty) {
            if (tempIndexLast < tempIndexFirst) {
                memcpy((newBuffer + arraySize),
                       Buffer + (tempIndexFirst * this->PointSize),
                       (Data.size() - tempIndexFirst) * this->PointSize * sizeof(double));
                dataCopied = (Data.size() - tempIndexFirst);
                memcpy((newBuffer + arraySize + (dataCopied * this->PointSize)),
                       Buffer, (tempIndexLast + 1) * this->PointSize * sizeof(double));
                dataCopied += (tempIndexLast + 1);
            } else {
                memcpy((newBuffer + arraySize),
                       Buffer + (tempIndexFirst * this->PointSize),
                       (tempIndexLast - tempIndexFirst + 1) * this->PointSize*sizeof(double));
                dataCopied = (tempIndexLast - tempIndexFirst + 1);
            }
        }
        memcpy(newBuffer,
               pointArray,
               arraySize * sizeof(double));
        dataCopied += (arraySize / this->PointSize);
        delete Buffer;
        Buffer = newBuffer;
        for (size_t i = 0;
             i < Data.size();
             i++) {
            this->Data.Element(i).SetRef(this->Buffer + this->PointSize * i);
        }
        this->IndexFirst = 0;
        this->IndexLast = dataCopied - 1;
        this->Empty = false;
        result = true;
    }
    return result;
}

void vctPlot2DBase::Signal::Freeze(bool freeze)
{
    if (freeze != this->Frozen) {
        this->Frozen = freeze;
        if (!freeze) {
            this->IndexFirst = 0;
            this->IndexLast = 0;
            this->Empty = true;
        }
    }
}

bool vctPlot2DBase::Signal::GetFreeze(void) const
{
    return this->Frozen;
}

void vctPlot2DBase::Signal::ComputeDataRangeXY(vctDouble2 & min, vctDouble2 & max) const
{
    // using pointer arithmetic
    const PointRef * currentPointer = this->Data.Pointer(0);
    size_t indexLast;
    const ptrdiff_t stridePointer = this->Data.stride();

    // if the buffer is full, just scan all elements otherwise scan up to last index
    if (this->IndexLast < this->IndexFirst) {
        indexLast = this->Data.size()-1;
    } else {
        indexLast = this->IndexLast;
    }
    const PointRef * lastPointer = currentPointer + indexLast * stridePointer;

    // iterate
    double value;
    max.X() = min.X() = currentPointer->X();
    max.Y() = min.Y() = currentPointer->Y();
    for (;
         currentPointer <= lastPointer;
         currentPointer += stridePointer) {
        value = currentPointer->X();
        if (value < min.X()) {
            min.X() = value;
        } else if (value > max.X()) {
            max.X() = value;
        }
        value = currentPointer->Y();
        if (value < min.Y()) {
            min.Y() = value;
        } else if (value > max.Y()) {
            max.Y() = value;
        }
    }
}

void vctPlot2DBase::Signal::ComputeDataRangeX(double & min, double & max,  bool assumesDataSorted) const
{
    const PointRef * currentPointer = this->Data.Pointer(0);
    size_t indexLast;
    const ptrdiff_t stridePointer = this->Data.stride();

    if (assumesDataSorted) {
        min = Data.at(this->IndexFirst).X();
        max = Data.at(this->IndexLast).X();
        return;
    }

    // if the buffer is full, just scan all elements otherwise scan up to last index
    if (this->IndexLast < this->IndexFirst) {
        indexLast = this->Data.size()-1;
    } else {
        indexLast = this->IndexLast;
    }
    const PointRef * lastPointer = currentPointer + indexLast * stridePointer;

    // iterate
    double value;
    max = min = currentPointer->X();
    for (;
         currentPointer <= lastPointer;
         currentPointer += stridePointer) {
        value = currentPointer->X();
        if (value < min) {
            min = value;
        } else if (value > max) {
            max = value;
        }
    }
}

void vctPlot2DBase::Signal::ComputeDataRangeY(double & min, double & max) const
{
    // using pointer arithmetic
    const PointRef * currentPointer = this->Data.Pointer(0);
    size_t indexLast;
    const ptrdiff_t stridePointer = this->Data.stride();

    // if the buffer is full, just scan all elements otherwise scan up to last index
    if (this->IndexLast < this->IndexFirst) {
        indexLast = this->Data.size()-1;
    } else {
        indexLast = this->IndexLast;
    }
    const PointRef * lastPointer = currentPointer + indexLast * stridePointer;

    // iterate
    double value;
    min = max = currentPointer->Y();
    for (;
         currentPointer <= lastPointer;
         currentPointer += stridePointer) {
        value = currentPointer->Y();
        if (value < min) {
            min = value;
        } else if (value > max) {
            max = value;
        }
    }
}

void vctPlot2DBase::Signal::SetNumberOfPoints(size_t numberOfPoints)
{
    delete Buffer;
    Buffer = new double[PointSize * numberOfPoints];
    memset(this->Buffer, 0, this->PointSize * numberOfPoints * sizeof(double));
    IndexFirst = 0;
    IndexLast = 0;
    Empty = true;

    this->Data.SetSize(numberOfPoints);
    // now set all the references
    size_t index;
    for (index = 0;
         index < numberOfPoints;
         index++) {
        this->Data.Element(index).SetRef(this->Buffer + this->PointSize * index);
    }
}

size_t vctPlot2DBase::Signal::GetSize(void) const
{
    return Data.size();
}

size_t vctPlot2DBase::Signal::GetNumberOfPoints(void) const
{
    size_t numberOfPoints = 0;
    if (this->IndexFirst < this->IndexLast) {
        numberOfPoints = this->IndexLast - this->IndexFirst + 1;
    } else {
        numberOfPoints = Data.size() - this->IndexFirst + this->IndexLast + 1;
    }
    return numberOfPoints;
}

void vctPlot2DBase::Signal::GetNumberOfPoints(size_t & numberOfPoints, size_t & bufferSize) const
{
    if (this->IndexFirst < this->IndexLast) {
        numberOfPoints = this->IndexLast - this->IndexFirst + 1;
    } else {
        numberOfPoints = Data.size() - this->IndexFirst + this->IndexLast + 1;
    }
    bufferSize = Data.size();
    return ;
}

void vctPlot2DBase::Signal::SetSize(size_t numberOfPoints)
{
    delete Buffer;
    this->Buffer = new double[PointSize * numberOfPoints];
    memset(this->Buffer, 0, this->PointSize * numberOfPoints * sizeof(double));
    this->IndexFirst = 0;
    this->IndexLast = 0;
    Empty = true;

    this->Data.SetSize(numberOfPoints);
    // now set all the references
    size_t index;
    for (index = 0;
         index < numberOfPoints;
         index++) {
        this->Data.Element(index).SetRef(this->Buffer + this->PointSize * index);
    }
}

bool vctPlot2DBase::Signal::IsVisible(void) const
{
    return this->Visible;
}

void vctPlot2DBase::Signal::SetVisible(const bool visible)
{
    this->Visible = visible;
}

void vctPlot2DBase::Signal::Resize(size_t numberOfPoints, bool trimOlder)
{
    // same size, do nothing
    if (Data.size() == numberOfPoints) {
        return;
    }

    double * newBuffer;
    size_t dataAmount = (this->IndexLast - this->IndexFirst) + 1;
    if (dataAmount <= 0) {
        dataAmount = Data.size() + dataAmount;
    }

    // get a new one
    newBuffer = new double[this->PointSize * numberOfPoints];
    memset(newBuffer, 0, this->PointSize * numberOfPoints * sizeof(double));

    size_t tempIndexLast = 0;
    // we move index to where it should be, while shrinking
    if (numberOfPoints < dataAmount) {
        // we need to cut some data off
        if (trimOlder) {
            this->IndexFirst += (dataAmount - numberOfPoints);
            if (this->IndexFirst >= Data.size()) {
                // overflow, move to buffer head
                this->IndexFirst = this->IndexFirst - Data.size();
            }
        } else {
            this->IndexLast = this->IndexFirst + numberOfPoints - 1;
            if (this->IndexLast >= Data.size()) {
                this->IndexLast = this->IndexLast - Data.size();
            }
        }
        tempIndexLast = numberOfPoints- 1;
    } else {
        tempIndexLast = dataAmount - 1;
    }

    // data copy from IndexFirst to IndexLast
    if (this->IndexFirst <= this->IndexLast) {
        // continuous array
        size_t bytesToCopy = (this->IndexLast - this->IndexFirst + 1) * this->PointSize * sizeof(double);
        double * indexNow = this->Buffer + this->IndexFirst * this->PointSize;
        memcpy(newBuffer, indexNow, bytesToCopy);
    } else {
        // first part
        double * indexNow = (this->Buffer + this->PointSize * this->IndexFirst);
        size_t bytesToCopy = (Data.size() - this->IndexFirst) * this->PointSize * sizeof(double);
        memcpy(newBuffer, indexNow, bytesToCopy);
        // second part
        indexNow = this->Buffer;
        bytesToCopy = (this->IndexLast + 1)* this->PointSize * sizeof(double);
        memcpy((newBuffer + (Data.size() - this->IndexFirst) * this->PointSize),
               indexNow,
               bytesToCopy);
    }

    this->Data.SetSize(numberOfPoints);
    size_t index;
    for (index = 0;
         index < numberOfPoints;
         index++) {
        this->Data.Element(index).SetRef(newBuffer + this->PointSize * index);
    }
    delete Buffer;
    Buffer = newBuffer;
    this->IndexFirst = 0;
    this->IndexLast = tempIndexLast;
    return;
}

void vctPlot2DBase::Signal::SetColor(const vctDouble3 & colorInRange0To1)
{
    vctDouble3 clippedColor;
    clippedColor.ClippedAboveOf(colorInRange0To1, 1.0);
    clippedColor.ClippedBelowOf(clippedColor, 0.0);
    this->Color.Assign(clippedColor);
}

vctPlot2DBase::VerticalLine::VerticalLine(const std::string & name, const double x):
    Name(name),
    X(x),
    Visible(true),
    Color(1.0, 1.0, 1.0),
    LineWidth(1.0)
{
}

vctPlot2DBase::VerticalLine::~VerticalLine()
{
}

void vctPlot2DBase::VerticalLine::SetX(const double x)
{
    this->X = x;
}

void vctPlot2DBase::VerticalLine::SetColor(const vctDouble3 & colorInRange0To1)
{
    vctDouble3 clippedColor;
    clippedColor.ClippedAboveOf(colorInRange0To1, 1.0);
    clippedColor.ClippedBelowOf(clippedColor, 0.0);
    this->Color.Assign(clippedColor);
}

vctPlot2DBase::vctPlot2DBase(size_t pointSize):
    PointSize(pointSize),
    BackgroundColor(0.1, 0.1, 0.1)
{
    CMN_ASSERT(this->PointSize >= 2);
    SetNumberOfPoints(200);
    SetContinuousFitX(true);
    SetContinuousFitY(true);
}

vctPlot2DBase::~vctPlot2DBase()
{
    const ScalesType::iterator end = Scales.end();
    for (ScalesType::iterator scale = Scales.begin();
         scale != end;
         ++scale) {
        if (scale->second) {
            delete scale->second;
        }
    }
}

vctPlot2DBase::Signal * vctPlot2DBase::AddSignal(const std::string & name)
{
    // check if the name already exists
    const std::string delimiter("-");
    std::string scaleName;
    size_t delimiterPosition = name.find(delimiter);
    scaleName = name.substr(0, delimiterPosition);
    // find or create new scale
    Scale * scalePointer = this->FindScale(scaleName);
    if (!scalePointer) {
        scalePointer = this->AddScale(scaleName);
    }
    return scalePointer->AddSignal(name);
}

/*
bool vctPlot2DBase::RemoveSignal(const std::string & name)
{
    const std::string delimiter("-");
    std::string scaleName;
    size_t delimiterPosition = name.find(delimiter);
    Scale * scalePointer = this->FindScale(name);

    scaleName = name.substr(0, delimiterPosition);

    if (!scalePointer) {// not found
        return false;
    }
    return scalePointer->RemoveSignal(name);
}
*/

vctPlot2DBase::VerticalLine * vctPlot2DBase::AddVerticalLine(const std::string & name)
{
    const std::string delimiter("-");
    std::string scaleName;
    size_t delimiterPosition = name.find(delimiter);
    Scale * scalePointer = this->FindScale(name);

    scaleName = name.substr(0, delimiterPosition);

    // no such Scale, create one
    if (!scalePointer) {
        std::cerr << "AddScale now\n";
        scalePointer = this->AddScale(scaleName);
    }
    return scalePointer->AddVerticalLine(name);
}

void vctPlot2DBase::SetNumberOfPoints(size_t numberOfPoints)
{
    this->NumberOfPoints = numberOfPoints;
    if (this->NumberOfPoints < 3) {
        this->NumberOfPoints = 3;
    }
}

void vctPlot2DBase::AutoFitX(double padding)
{
    const ScalesType::iterator end = Scales.end();
    for (ScalesType::iterator scale = Scales.begin();
         scale != end;
         ++scale) {
        scale->second->AutoFitX(padding);
    }
}

void vctPlot2DBase::FitX(double min, double max, double padding)
{
    const ScalesType::iterator end = Scales.end();
    for (ScalesType::iterator scale = Scales.begin();
         scale != end;
         ++scale) {
        scale->second->FitX(min, max, padding);
    }
}

void vctPlot2DBase::AutoFitY(double padding)
{
    const ScalesType::iterator end = Scales.end();
    for (ScalesType::iterator scale = Scales.begin();
         scale != end;
         ++scale) {
        scale->second->AutoFitY(padding);
    }
}

void vctPlot2DBase::FitY(double min, double max, double padding)
{
    const ScalesType::iterator end = Scales.end();
    for (ScalesType::iterator scale = Scales.begin();
         scale != end;
         ++scale) {
        scale->second->FitY(min, max, padding);
    }
}

void vctPlot2DBase::Freeze(bool freeze)
{
    const ScalesType::iterator end = Scales.end();
    for (ScalesType::iterator scale = Scales.begin();
         scale != end;
         ++scale) {
        scale->second->Freeze(freeze);
    }
}

bool vctPlot2DBase::GetFreeze(void) const
{
    bool allFrozen = true;
    const ScalesType::const_iterator end = Scales.end();
    for (ScalesType::const_iterator scale = Scales.begin();
         scale != end;
         ++scale) {
        allFrozen = allFrozen && scale->second->GetFreeze();
    }
    return allFrozen;
}

void vctPlot2DBase::SetContinuousFitX(bool fit)
{
    const ScalesType::iterator end = Scales.end();
    for (ScalesType::iterator scale = Scales.begin();
         scale != end;
         ++scale) {
        scale->second->SetContinuousFitX(fit);
    }
}

void vctPlot2DBase::SetContinuousFitY(bool fit)
{
    const ScalesType::iterator end = Scales.end();
    for (ScalesType::iterator scale = Scales.begin();
         scale != end;
         ++scale) {
        scale->second->SetContinuousFitY(fit);
    }

}

void vctPlot2DBase::SetContinuousExpandY(bool expand)
{
    const ScalesType::iterator end = Scales.end();
    for (ScalesType::iterator scale = Scales.begin();
         scale != end;
         ++scale) {
        scale->second->SetContinuousExpandY(expand);
    }
}

bool vctPlot2DBase::GetContinuousFitX(void) const
{
    bool continuous = true;
    const ScalesType::const_iterator end = Scales.end();
    for (ScalesType::const_iterator scale = Scales.begin();
         scale != end;
         ++scale) {
        continuous = continuous && scale->second->GetContinuousFitX();
    }
    return continuous;
}

bool vctPlot2DBase::GetContinuousFitY(void) const
{
    bool continuous = true;
    const ScalesType::const_iterator end = Scales.end();
    for (ScalesType::const_iterator scale = Scales.begin();
         scale != end;
         ++scale) {
        continuous = continuous && scale->second->GetContinuousFitY();
    }
    return continuous;
}

bool vctPlot2DBase::GetContinuousExpandY(void) const
{
    bool continuous = true;
    const ScalesType::const_iterator end = Scales.end();
    for (ScalesType::const_iterator scale = Scales.begin();
         scale != end;
         ++scale) {
        continuous = continuous && scale->second->GetContinuousExpandY();
    }
    return continuous;
}

void vctPlot2DBase::ContinuousUpdate(void)
{
    const ScalesType::iterator end = Scales.end();
    for (ScalesType::iterator scale = Scales.begin();
         scale != end;
         ++scale) {
        scale->second->ContinuousUpdate();
    }
}

void vctPlot2DBase::SetBackgroundColor(const vctDouble3 & colorInRange0To1)
{
    vctDouble3 clippedColor;
    clippedColor.ClippedAboveOf(colorInRange0To1, 1.0);
    clippedColor.ClippedBelowOf(clippedColor, 0.0);
    this->BackgroundColor.Assign(clippedColor);
}

vctPlot2DBase::Scale * vctPlot2DBase::AddScale(const std::string & name)
{
    // check if the name already exists
    ScalesType::const_iterator found = Scales.find(name);
    if (found == Scales.end()) {
        // new scale
        Scale * newScale = new Scale(name);
        Scales[name] = newScale;
        return newScale;
    }
    // existing scale
    return 0;
}

vctPlot2DBase::Scale * vctPlot2DBase::FindScale(const std::string & name)
{
    ScalesType::const_iterator found = Scales.find(name);
    if (found == Scales.end()) {
        return 0;
    }
    return found->second;
}

bool vctPlot2DBase::RemoveScale(const std::string & name)
{
    ScalesType::iterator found = Scales.find(name);
    if (found != Scales.end()) {
        Scale * scale = found->second;
        Scales.erase(found);
        delete scale;
        return true;
    }
    return false;
}

bool vctPlot2DBase::RemoveScale(const Scale * scale)
{
    if (scale) {
        return this->RemoveScale(scale->GetName());
    }
    return false;
}
