/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctPlot2DBase.cpp 1238 2010-02-27 03:16:01Z auneri1 $

  Author(s):  Anton Deguet
  Created on: 2010-05-05

  (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstVector/vctPlot2DBase.h>
#include <cisstCommon/cmnUnits.h>
#include <cisstCommon/cmnAssert.h>
#include <iostream>

vctPlot2DBase::Trace::Trace(const std::string & name, size_t numberOfPoints, size_t PointSize):
    Name(name),
    Empty(true),
    Visible(true),
    Frozen(false),
    Buffer(0),
    IndexFirst(0),
    IndexLast(0),
    Color(1.0, 1.0, 1.0),
    LineWidth(1.0),
    PointSize(PointSize)
{
    // create the underlaying buffer and fill it with zeros
    CMN_ASSERT(PointSize >= 2);
    this->Buffer = new double[PointSize * numberOfPoints];
    memset(this->Buffer, 0, PointSize * numberOfPoints * sizeof(double));
    this->Data.SetSize(numberOfPoints);
    // now set all the references
    size_t index;
    for (index = 0;
         index < numberOfPoints;
         index++) {
        this->Data.Element(index).SetRef(this->Buffer + PointSize * index);
    }
}


vctPlot2DBase::Trace::~Trace()
{
    if (this->Buffer) {
        delete this->Buffer;
    }
};


void vctPlot2DBase::Trace::AddPoint(const vctDouble2 & point)
{
    //this->CriticalSectionForBuffer.Enter();
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
    //this->CriticalSectionForBuffer.Leave();
}

void vctPlot2DBase::Trace::AppendPoint(const vctDouble2 & point)
{
    //this->CriticalSectionForBuffer.Enter();
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
    //this->CriticalSectionForBuffer.Leave();
}

vctDouble2 vctPlot2DBase::Trace::GetPointAt(size_t index){

    if(index >= Data.size() )
        cmnThrow("vctPlot2DBase: GetPointAt: Index bigger than bufferSize");
    return Data.at(index);
}

void vctPlot2DBase::Trace::SetPointAt(size_t index, const vctDouble2 & point){

    if(index >= Data.size() )
        cmnThrow("vctPlot2DBase: SetPointAt: Index bigger than bufferSize");
    this->Data.Element(index).Assign(point);
    return;
}

void vctPlot2DBase::Trace::SetArrayAt(size_t index, const double * pointArray, size_t size){

    if((index+ (size/this->PointSize) ) >= Data.size() )
        cmnThrow("vctPlot2DBase: SetArrayAt: Index+size bigger than bufferSize");

    memcpy((this->Buffer+index*this->PointSize), pointArray, size*sizeof(double));
    return;
}


bool vctPlot2DBase::Trace::AppendArray(const double * pointArray, size_t arraySize)
{
    bool result = false;
    // check if there is enough space for array we want to insert
    if(this->IndexFirst > this->IndexLast || ( (Data.size() - this->IndexLast-1) * this->PointSize) < arraySize)
        return result;

    //this->CriticalSectionForBuffer.Enter();
    if (!this->Frozen) {
        // copy to last
        if(!this->Empty){
            memcpy((this->Buffer+ (this->IndexLast+1)*this->PointSize), pointArray, arraySize*sizeof(double));
            this->IndexLast += arraySize/this->PointSize;
        }
        else{
            memcpy(this->Buffer, pointArray, arraySize*sizeof(double));
            this->IndexLast = (arraySize/this->PointSize -1);
        }
        result = true;
    }
    //this->CriticalSectionForBuffer.Leave();
    return result;
}



bool vctPlot2DBase::Trace::PrependArray(const double * pointArray, size_t arraySize)
{
    // check if there is enough space for array we want to insert
    bool result = false;
    if(this->IndexFirst > this->IndexLast || ( (Data.size() - this->IndexLast-1) * this->PointSize) < arraySize)
        return result;
    //this->CriticalSectionForBuffer.Enter();
    if (!this->Frozen) {
        double *NewBuffer = new double [Data.size()*this->PointSize];
        memset(NewBuffer, 0 , Data.size()*this->PointSize*sizeof(double));
        // first section
        memcpy(NewBuffer, pointArray, arraySize*sizeof(double));
        // second section
        if(!this->Empty)
            memcpy((NewBuffer+arraySize), Buffer, (this->IndexLast+1)*this->PointSize*sizeof(double));

        delete Buffer;
        Buffer = NewBuffer;
        for(size_t i = 0; i < Data.size(); i++){
            this->Data.Element(i).SetRef(this->Buffer + this->PointSize * i);
        }
        this->IndexFirst = 0;
        if(!this->Empty)
            this->IndexLast += (arraySize/ this->PointSize);
        else
            this->IndexLast = (arraySize/ this->PointSize)-1;
        result = true;
    }
    //this->CriticalSectionForBuffer.Leave();
    return result;
}


void vctPlot2DBase::Trace::Freeze(bool freeze)
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


void vctPlot2DBase::Trace::ComputeDataRangeXY(vctDouble2 & min, vctDouble2 & max)
{
    // using pointer arithmetic
    //CriticalSectionForBuffer.Enter();
    PointRef * currentPointer = this->Data.Pointer(0);
    size_t indexLast;
    const ptrdiff_t stridePointer = this->Data.stride();

    // if the buffer is full, just scan all elements otherwise scan up to last index
    if (this->IndexLast < this->IndexFirst) {
        indexLast = this->Data.size();
    } else {
        indexLast = this->IndexLast;
    }
    const PointRef * lastPointer = currentPointer + indexLast * stridePointer;

    // iterate
    double value;
    for (;
         currentPointer != lastPointer;
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
    //CriticalSectionForBuffer.Leave();
}

void vctPlot2DBase::Trace::GetLeftRightDataX(double &min, double &max){
    min = Data.at(this->IndexFirst).X();
    max = Data.at(this->IndexLast).X();

}
void vctPlot2DBase::Trace::ComputeDataRangeX(double & min, double & max)
{
    //CriticalSectionForBuffer.Enter();
    // using pointer arithmetic
    PointRef * currentPointer = this->Data.Pointer(0);
    size_t indexLast;
    const ptrdiff_t stridePointer = this->Data.stride();

    // if the buffer is full, just scan all elements otherwise scan up to last index
    if (this->IndexLast < this->IndexFirst) {
        indexLast = this->Data.size();
    } else {
        indexLast = ((this->IndexLast+1) >= this->Data.size()) ? this->Data.size()-1: this->IndexLast+1;
    }
    const PointRef * lastPointer = currentPointer + indexLast * stridePointer;

    // iterate
    double value;
    for (;
         currentPointer != lastPointer;
         currentPointer += stridePointer) {
        value = currentPointer->X();
        if (value < min) {
            min = value;
        } else if (value > max) {
            max = value;
        }
    }
    //CriticalSectionForBuffer.Leave();
}


void vctPlot2DBase::Trace::ComputeDataRangeY(double & min, double & max)
{
    // using pointer arithmetic
    //CriticalSectionForBuffer.Enter();
    PointRef * currentPointer = this->Data.Pointer(0);
    size_t indexLast;
    const ptrdiff_t stridePointer = this->Data.stride();

    // if the buffer is full, just scan all elements otherwise scan up to last index
    if (this->IndexLast < this->IndexFirst) {
        indexLast = this->Data.size();
    } else {
        indexLast = this->IndexLast;
    }
    const PointRef * lastPointer = currentPointer + indexLast * stridePointer;

    // iterate
    double value;
    for (;
         currentPointer != lastPointer;
         currentPointer += stridePointer) {
        value = currentPointer->Y();
        if (value < min) {
            min = value;
        } else if (value > max) {
            max = value;
        }
    }
    //CriticalSectionForBuffer.Leave();
}

void vctPlot2DBase::Trace::SetNumberOfPoints(size_t numberOfPoints){
    //CriticalSectionForBuffer.Enter();
    delete Buffer;
    Buffer = new double[PointSize * numberOfPoints];
    //memset(this->Buffer, 0, sizeof(vctDouble2) * numberOfPoints * sizeof(double));
    memset(this->Buffer, 0, sizeof(Buffer));
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
    //CriticalSectionForBuffer.Leave();
}

void  vctPlot2DBase::Trace::GetNumberOfPoints(size_t &numberOfPoints, size_t &bufferSize){
    //CriticalSectionForBuffer.Enter();
    if(IndexFirst < IndexLast)
        numberOfPoints = IndexLast - IndexFirst +1;
    else
        numberOfPoints =  Data.size() - IndexFirst+ IndexLast+1;

    bufferSize = Data.size();
    //CriticalSectionForBuffer.Leave();
    return ;
}

void vctPlot2DBase::Trace::SetSize(size_t numberOfPoints){
    //this->CriticalSectionForBuffer.Enter();
    delete Buffer;
    Buffer = new double[PointSize * numberOfPoints];
    //memset(this->Buffer, 0, sizeof(vctDouble2) * numberOfPoints * sizeof(double));
    memset(this->Buffer, 0, sizeof(Buffer));
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
    //this->CriticalSectionForBuffer.Leave();
}

void vctPlot2DBase::Trace::ReSize(size_t numberOfPoints){

    // same size, do nothing
    if(Data.size() == numberOfPoints) {
            return;
    }

    double *NewBuffer;
    int  DataAmount = (this->IndexLast -  this->IndexFirst)+1 ;

    if(DataAmount <= 0)
        DataAmount = Data.size() + DataAmount;

    // get a new one
    NewBuffer = new double[this->PointSize * numberOfPoints];
    memset(NewBuffer, 0, this->PointSize*numberOfPoints*sizeof(double));

    size_t TempIndexLast=0;
    // we move index to where it should be, while shrinking
    if(numberOfPoints < DataAmount){
        // we need to cut some data off
        this->IndexFirst += (DataAmount - numberOfPoints) ;
        if(this->IndexFirst  >= Data.size() ){
            // overflow, move to buffer head
            this->IndexFirst = this->IndexFirst-Data.size();
        }
        TempIndexLast = numberOfPoints- 1;
    }else
        TempIndexLast = DataAmount-1;

    // data copy from IndexFirst to IndexLast
    if(this-> IndexFirst <= this->IndexLast){
        // continuous array
        size_t BytesToCopy = ( this->IndexLast -  this->IndexFirst+1) * this->PointSize * sizeof(double);
        double *IndexNow = this->Buffer;
        memcpy(NewBuffer, IndexNow, BytesToCopy);
    }else{
        // first part
        double *IndexNow = (this->Buffer + this->PointSize * this->IndexFirst);
        //double *IndexNow = this->Buffer;
        size_t BytesToCopy = (Data.size() -  this->IndexFirst) * this->PointSize * sizeof(double);
        memcpy(NewBuffer, IndexNow, BytesToCopy);

        // second part
        IndexNow = this->Buffer;
        BytesToCopy = (this->IndexLast+1)* this->PointSize * sizeof(double);
        memcpy( (NewBuffer+(Data.size() -  this->IndexFirst)*this->PointSize), IndexNow, BytesToCopy);
    }

    //this->CriticalSectionForBuffer.Enter();
    this->Data.SetSize(numberOfPoints);
    size_t index;
    for (index = 0;
         index < numberOfPoints;
         index++) {
        this->Data.Element(index).SetRef(NewBuffer + this->PointSize * index);
    }
    delete Buffer;
    Buffer = NewBuffer;
    this->IndexFirst = 0;
    this->IndexLast = TempIndexLast;
    //this->CriticalSectionForBuffer.Leave();
    return;
}

void vctPlot2DBase::Trace::SetColor(const vctDouble3 & colorInRange0To1)
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
};


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
    Frozen(false),
    BackgroundColor(0.1, 0.1, 0.1)
{
    CMN_ASSERT(this->PointSize >= 2);
    SetNumberOfPoints(200);
    Translation.SetAll(0.0);
    Scale.SetAll(1.0);
    SetContinuousFitX(true);
    SetContinuousFitY(true);
}


vctPlot2DBase::Trace * vctPlot2DBase::AddTrace(const std::string & name, size_t & traceId)
{
    // check if the name already exists
    TracesIdType::const_iterator found = this->TracesId.find(name);
    const TracesIdType::const_iterator end = this->TracesId.end();
    if (found == end) {
        // new name
        traceId = Traces.size();
        Trace * newTrace = new Trace(name, this->NumberOfPoints, this->PointSize);
        Traces.push_back(newTrace);
        TracesId[name] = traceId;
        return newTrace;
    }
    return 0;
}


vctPlot2DBase::Trace * vctPlot2DBase::AddTrace(const std::string & name)
{
    size_t traceId;
    return this->AddTrace(name, traceId);
}


vctPlot2DBase::VerticalLine * vctPlot2DBase::AddVerticalLine(const std::string & name)
{
    VerticalLine * newLine = new VerticalLine(name);
    if (this->VerticalLines.AddItem(name, newLine)) {
        return newLine;
    }
    delete newLine;
    return 0;
}

void vctPlot2DBase::SetNumberOfPoints(size_t numberOfPoints)
{
    this->NumberOfPoints = numberOfPoints;
    if (this->NumberOfPoints < 3) {
        this->NumberOfPoints = 3;
    }
}


void vctPlot2DBase::AddPoint(size_t traceId, const vctDouble2 & point)
{
    this->Traces[traceId]->AddPoint(point);
}


void vctPlot2DBase::FitX(double padding)
{
    double min, max;
    this->ComputeDataRangeX(min, max);
    this->FitX(min, max, padding);
}


void vctPlot2DBase::FitX(double min, double max, double padding)
{
    this->ViewingRangeX.Assign(min, max);
    this->Scale.X() = this->Viewport.X() / ((max - min) * (1.0 + padding));
    this->Translation.X() =
        - min * this->Scale.X()
        + 0.5 * padding * this->Viewport.X();
}


void vctPlot2DBase::FitY(double padding)
{
    double min, max;
    this->ComputeDataRangeY(min, max);
    this->FitY(min, max, padding);
}


void vctPlot2DBase::FitY(double min, double max, double padding)
{
    this->ViewingRangeY.Assign(min, max);
    this->Scale.Y() = this->Viewport.Y() / ((max - min) * (1.0 + padding));
    this->Translation.Y() =
        - min * this->Scale.Y()
        + 0.5 * padding * this->Viewport.Y();
}


void vctPlot2DBase::FitXY(const vctDouble2 & padding)
{
    vctDouble2 min, max;
    this->ComputeDataRangeXY(min, max);
    this->FitXY(min, max, padding);
}


void vctPlot2DBase::FitXY(vctDouble2 min, vctDouble2 max, const vctDouble2 & padding)
{
    this->ViewingRangeX.Assign(min.X(), max.X());
    this->ViewingRangeY.Assign(min.Y(), max.Y());
    // compute scale
    vctDouble2 dataDiff;
    dataDiff.DifferenceOf(max, min);
    this->Scale.ElementwiseRatioOf(this->Viewport, dataDiff);
    vctDouble2 pad(padding);
    pad.Add(1.0);
    this->Scale.ElementwiseDivide(pad);
    // compute translation
    this->Translation.ElementwiseProductOf(min, this->Scale);
    this->Translation.NegationSelf();
    pad.ElementwiseProductOf(padding, this->Viewport);
    pad.Multiply(0.5);
    this->Translation.Add(pad);
}


void vctPlot2DBase::Freeze(bool freeze)
{
    size_t traceIndex;
    const size_t numberOfTraces = this->Traces.size();
    for (traceIndex = 0;
         traceIndex < numberOfTraces;
         traceIndex++) {
        this->Traces[traceIndex]->Freeze(freeze);
    }
    this->Frozen = freeze;
}


void vctPlot2DBase::SetContinuousFitX(bool fit)
{
    this->ContinuousFitX = fit;
    this->ContinuousAlignMaxX = false;
    this->Continuous = (this->ContinuousFitX
                        || this->ContinuousFitY
                        || this->ContinuousAlignMaxX);
}


void vctPlot2DBase::SetContinuousFitY(bool fit)
{
    this->ContinuousFitY = fit;
    this->Continuous = (this->ContinuousFitX
                        || this->ContinuousFitY
                        || this->ContinuousAlignMaxX);
}


void vctPlot2DBase::SetContinuousAlignMaxX(bool align)
{
    this->ContinuousAlignMaxX = align;
    this->ContinuousFitX = false;
    this->Continuous = (this->ContinuousFitX
                        || this->ContinuousFitY
                        || this->ContinuousAlignMaxX);
}


void vctPlot2DBase::ComputeDataRangeX(double & min, double & max)
{
    if (this->Traces.size() == 0) {
        min = -1.0;
        max = 1.0;
    } else {
        size_t traceIndex;
        const size_t numberOfTraces = this->Traces.size();
        vctPlot2DBase::Trace * trace = this->Traces[0]; // should really be looking for first visible trace
        //trace->CriticalSectionForBuffer.Enter();
        min = trace->Data.Element(trace->IndexFirst).X();
        max = min;
        for (traceIndex = 0;
             traceIndex < numberOfTraces;
             traceIndex++) {
            this->Traces[traceIndex]->ComputeDataRangeX(min, max);
        }
        //trace->CriticalSectionForBuffer.Leave();
    }
}


void vctPlot2DBase::ComputeDataRangeY(double & min, double & max)
{
    if (this->Traces.size() == 0) {
        min = -1.0;
        max = 1.0;
    } else {
        size_t traceIndex;
        const size_t numberOfTraces = this->Traces.size();
        vctPlot2DBase::Trace * trace = this->Traces[0]; // should really be looking for first visible trace
        //trace->CriticalSectionForBuffer.Enter();
        min = trace->Data.Element(trace->IndexFirst).Y();

        max = min;
        for (traceIndex = 0;
             traceIndex < numberOfTraces;
             traceIndex++) {
            this->Traces[traceIndex]->ComputeDataRangeY(min, max);
        }
        //trace->CriticalSectionForBuffer.Leave();
    }
}


void vctPlot2DBase::ComputeDataRangeXY(vctDouble2 & min, vctDouble2 & max)
{

    if (this->Traces.size() == 0) {
        min.SetAll(-1.0);
        max.SetAll(1.0);
    } else {
        size_t traceIndex;
        const size_t numberOfTraces = this->Traces.size();
        vctPlot2DBase::Trace * trace = this->Traces[0]; // should really be looking for first visible trace
         //trace->CriticalSectionForBuffer.Enter();
        min.Assign(trace->Data.Element(trace->IndexFirst));
        max.Assign(min);
        for (traceIndex = 0;
             traceIndex < numberOfTraces;
             traceIndex++) {
            this->Traces[traceIndex]->ComputeDataRangeXY(min, max);
        }
        //trace->CriticalSectionForBuffer.Leave();
    }
}


void vctPlot2DBase::ContinuousUpdate(void)
{
    if (this->Continuous) {
        if (this->ContinuousFitX && this->ContinuousFitY) {
            this->FitXY();
        } else if (this->ContinuousFitX) {
            this->FitX();
        } else if (this->ContinuousFitY) {
            this->FitY();
        }
    }
}


void vctPlot2DBase::SetColor(size_t traceId, const vctDouble3 & colorInRange0To1)
{
    this->Traces[traceId]->SetColor(colorInRange0To1);
}


void vctPlot2DBase::SetBackgroundColor(const vctDouble3 & colorInRange0To1)
{
    vctDouble3 clippedColor;
    clippedColor.ClippedAboveOf(colorInRange0To1, 1.0);
    clippedColor.ClippedBelowOf(clippedColor, 0.0);
    this->BackgroundColor.Assign(clippedColor);
}
