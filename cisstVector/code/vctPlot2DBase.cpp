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

#include <cisstVector/vctPlot2DBase.h>

#include <cisstCommon/cmnAssert.h>
#include <cisstCommon/cmnTypeTraits.h>
#include <cisstCommon/cmnUnits.h>
#include <limits>

vctPlot2DBase::Scale::Scale(const std::string & name):
    ExpandYMin(std::numeric_limits<double>::max()),
    ExpandYMax(std::numeric_limits<double>::min())
{
    this->Translation.setZero();
    this->ScaleValue.setOnes();
    this->Name = name;
    AutoFitXY(Eigen::Vector2d::Ones());
    SetContinuousFitX(true);
    SetContinuousExpandY(true);
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
    this->ViewingRangeX = Eigen::Vector2d(min, max);
    this->ScaleValue.x() = this->Viewport.x() / ((max - min) * (1.0 + padding));
    if (!cmnTypeTraits<double>::IsFinite(ScaleValue.x())) { ScaleValue.x() = 1.0; }

    this->Translation.x() =
        - min * this->ScaleValue.x()
        + 0.5 * padding * this->Viewport.x();
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
    this->ViewingRangeY = Eigen::Vector2d(min, max);
    this->ScaleValue.y() = this->Viewport.y() / ((max - min) * (1.0 + padding));
    if (!cmnTypeTraits<double>::IsFinite(ScaleValue.y())) { ScaleValue.y() = 1.0; }

    this->Translation.y() =
        - min * this->ScaleValue.y()
        + 0.5 * padding * this->Viewport.y();
}

void vctPlot2DBase::Scale::AutoFitXY(const Eigen::Vector2d& padding)
{
    Eigen::Vector2d min, max;
    this->ComputeDataRangeXY(min, max);
    this->FitXY(min, max, padding);
}

void vctPlot2DBase::Scale::FitXY(Eigen::Vector2d min, Eigen::Vector2d max, const Eigen::Vector2d& padding)
{
    ViewingRangeX = Eigen::Vector2d(min.x(), max.x());
    ViewingRangeY = Eigen::Vector2d(min.y(), max.y());
    // compute scale
    Eigen::Vector2d dataDiff = max - min;
    this->ScaleValue = this->Viewport.cwiseQuotient(dataDiff);
    if (!cmnTypeTraits<double>::IsFinite(ScaleValue.x())) { ScaleValue.x() = 1.0; }
    if (!cmnTypeTraits<double>::IsFinite(ScaleValue.y())) { ScaleValue.y() = 1.0; }

    Eigen::Vector2d pad = padding + Eigen::Vector2d::Ones();
    this->ScaleValue = ScaleValue.cwiseQuotient(pad);
    // compute translation
    this->Translation = -min.cwiseProduct(ScaleValue);
    pad = 0.5 * padding.cwiseProduct(this->Viewport);
    this->Translation += pad;
}

void vctPlot2DBase::Scale::AutoFitXExpandY(const Eigen::Vector2d& padding)
{
    Eigen::Vector2d min, max;
    ComputeDataRangeXY(min, max);

    min.y() = std::min(min.y(), ExpandYMin);
    ExpandYMin = min.y();

    max.y() = std::max(max.y(), ExpandYMax);
    ExpandYMax = max.y();

    FitXY(min, max, padding);
}

bool vctPlot2DBase::Scale::ComputeDataRangeX(double & min, double & max, bool assumesDataSorted) const
{
    if (Signals.empty()) {
        min = -1.0;
        max = 1.0;
        return false;
    }

    min = std::numeric_limits<double>::max();
    max = std::numeric_limits<double>::min();

    for (const auto& signal : Signals) {
        if (signal.second->IsVisible()) {
            double signal_min, signal_max;
            signal.second->ComputeDataRangeX(signal_min, signal_max, assumesDataSorted);
            min = std::min(min, signal_min);
            max = std::max(max, signal_max);
        }
    }

    return true;
}

bool vctPlot2DBase::Scale::ComputeDataRangeY(double & min, double & max)
{
    if (Signals.empty()) {
        min = -1.0;
        max = 1.0;
        return false;
    }

    min = std::numeric_limits<double>::max();
    max = std::numeric_limits<double>::min();

    for (const auto& signal : Signals) {
        if (signal.second->IsVisible()) {
            double signal_min, signal_max;
            signal.second->ComputeDataRangeY(signal_min, signal_max);
            min = std::min(min, signal_min);
            max = std::max(max, signal_max);
        }
    }

    return true;
}

bool vctPlot2DBase::Scale::ComputeDataRangeXY(Eigen::Vector2d& min, Eigen::Vector2d& max)
{
    if (Signals.empty()) {
        min.fill(-1.0);
        max.fill(1.0);
        return false;
    }

    min.fill(std::numeric_limits<double>::max());
    max.fill(std::numeric_limits<double>::min());

    for (const auto& signal : Signals) {
        if (signal.second->IsVisible()) {
            Eigen::Vector2d signal_min, signal_max;

            signal.second->ComputeDataRangeXY(min, max);
            min = min.cwiseMin(signal_min);
            max = max.cwiseMax(signal_max);
        }
    }

    return true;
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
    for (auto& signal : Signals) {
        signal.second->Freeze(freeze);
    }
}

bool vctPlot2DBase::Scale::GetFreeze(void) const
{
    for (auto& signal : Signals) {
        if (!signal.second->GetFreeze()) {
            return false;
        }
    }

    return true;
}

void vctPlot2DBase::Scale::SetColor(const Eigen::Vector3d& colorInRange0To1)
{
    Color = colorInRange0To1.cwiseMax(0.0)
                            .cwiseMin(1.0);
}

vctPlot2DBase::Signal * vctPlot2DBase::Scale::AddSignal(const std::string & name)
{
    // check if the name already exists
    decltype(Signals)::const_iterator found = Signals.find(name);
    if (found != Signals.end()) {
        return nullptr;
    }

    Signals[name] = std::make_unique<Signal>(name, 100);
    return Signals[name].get();
}

bool vctPlot2DBase::Scale::RemoveSignal(const std::string & name)
{
    decltype(Signals)::const_iterator found = Signals.find(name);
    if (found == Signals.end()) {
        return false;
    }

    Signals.erase(found);
    return true;
}

bool vctPlot2DBase::Scale::RemoveSignal(const Signal* signal)
{
    if (signal == nullptr) {
        return false;
    }

    return this->RemoveSignal(signal->GetName());
}

vctPlot2DBase::VerticalLine * vctPlot2DBase::Scale::AddVerticalLine(const std::string & name)
{
    // check if the name already exists
    const auto found = VerticalLines.find(name);
    if (found != VerticalLines.end()) {
        return nullptr;
    }

    VerticalLines[name] = std::make_unique<VerticalLine>(name);
    return VerticalLines[name].get();
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

vctPlot2DBase::Signal::Signal(const std::string & name, size_t numberOfPoints):
    Name(name),
    Empty(true),
    Visible(true),
    Frozen(false),
    Color(1.0, 1.0, 1.0),
    LineWidth(1.0),
    data(2, numberOfPoints),
    IndexFirst(0),
    IndexLast(0)
{}

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

void vctPlot2DBase::Signal::AppendPoint(const Eigen::Vector2d& point)
{
    if (Frozen) { return; }

    // look where to store this point
    if (!this->Empty) {
        IndexLast = (IndexLast + 1) % data.cols();

        if (IndexFirst == IndexLast) {
            IndexFirst = (IndexFirst + 1) % data.cols();
        }
    }

    data.col(IndexLast) = point;
}

Eigen::Vector2d vctPlot2DBase::Signal::GetPointAt(size_t index)
    CISST_THROW(std::runtime_error)
{
    if ((Eigen::Index)index >= data.cols()) {
        cmnThrow("vctPlot2DBase::Signal::GetPointAt: index bigger than bufferSize");
    }
    index = ((index + this->IndexFirst) % data.cols());
    return data.col(index);
}

void vctPlot2DBase::Signal::SetPointAt(size_t index, const Eigen::Vector2d& point)
    CISST_THROW(std::runtime_error)
{
    if ((Eigen::Index)index >= data.cols()) {
        cmnThrow("vctPlot2DBase::Signal::SetPointAt: index bigger than bufferSize");
    }
    index = ((index + this->IndexFirst) % data.cols());
    data.col(index) = point;
    return;
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

void vctPlot2DBase::Signal::ComputeDataRangeXY(Eigen::Vector2d& min, Eigen::Vector2d& max) const
{
    min.fill(std::numeric_limits<double>::min());
    max.fill(std::numeric_limits<double>::max());

    if (IndexFirst <= IndexLast) {
        for (Eigen::Index idx = IndexFirst; idx < IndexLast; idx++) {
            min = min.cwiseMin(data.col(idx));
            max = max.cwiseMax(data.col(idx));
        }
    } else {
        for (Eigen::Index idx = 0; idx < IndexLast; idx++) {
            min = min.cwiseMin(data.col(idx));
            max = max.cwiseMax(data.col(idx));
        }

        for (Eigen::Index idx = IndexFirst; idx < data.cols(); idx++) {
            min = min.cwiseMin(data.col(idx));
            max = max.cwiseMax(data.col(idx));
        }
    }
}

void vctPlot2DBase::Signal::ComputeDataRangeX(double & min, double & max,  bool assumesDataSorted) const
{
    if (assumesDataSorted) {
        min = data.col(IndexFirst).x();
        max = data.col(IndexLast).x();
        return;
    }

    min = std::numeric_limits<double>::min();
    max = std::numeric_limits<double>::max();

    if (IndexFirst <= IndexLast) {
        for (Eigen::Index idx = IndexFirst; idx < IndexLast; idx++) {
            min = std::min(min, data.col(idx).x());
            max = std::max(max, data.col(idx).x());
        }
    } else {
        for (Eigen::Index idx = 0; idx < IndexLast; idx++) {
            min = std::min(min, data.col(idx).x());
            max = std::max(max, data.col(idx).x());
        }

        for (Eigen::Index idx = IndexFirst; idx < data.cols(); idx++) {
            min = std::min(min, data.col(idx).x());
            max = std::max(max, data.col(idx).x());
        }
    }
}

void vctPlot2DBase::Signal::ComputeDataRangeY(double & min, double & max) const
{
    min = std::numeric_limits<double>::min();
    max = std::numeric_limits<double>::max();

    if (IndexFirst <= IndexLast) {
        for (Eigen::Index idx = IndexFirst; idx < IndexLast; idx++) {
            min = std::min(min, data.col(idx).y());
            max = std::max(max, data.col(idx).y());
        }
    } else {
        for (Eigen::Index idx = 0; idx < IndexLast; idx++) {
            min = std::min(min, data.col(idx).y());
            max = std::max(max, data.col(idx).y());
        }

        for (Eigen::Index idx = IndexFirst; idx < data.cols(); idx++) {
            min = std::min(min, data.col(idx).y());
            max = std::max(max, data.col(idx).y());
        }
    }
}

size_t vctPlot2DBase::Signal::GetSize(void) const
{
    return data.cols();
}

size_t vctPlot2DBase::Signal::GetNumberOfPoints() const
{
    size_t numberOfPoints = 0;
    if (this->IndexFirst < this->IndexLast) {
        numberOfPoints = this->IndexLast - this->IndexFirst + 1;
    } else {
        numberOfPoints = data.cols() - this->IndexFirst + this->IndexLast + 1;
    }
    return numberOfPoints;
}

void vctPlot2DBase::Signal::SetSize(size_t numberOfPoints)
{
    data = Eigen::MatrixXd::Zero(2, numberOfPoints);
    IndexFirst = 0;
    IndexLast = 0;
    Empty = true;
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
    if (data.cols() == (Eigen::Index)numberOfPoints) {
        return;
    }

    Eigen::Matrix2Xd new_data = Eigen::Matrix2Xd::Zero(2, numberOfPoints);
    size_t current_number_points = (this->IndexLast - this->IndexFirst) + 1;
    if (current_number_points <= 0) {
        current_number_points = data.cols() + current_number_points;
    }

    // Shrink IndexFirst/IndexLast if necessary to fit into smaller buffer
    if (numberOfPoints < current_number_points) {
        int extra_points = current_number_points - numberOfPoints;
        if (trimOlder) {
            IndexFirst = (IndexFirst + extra_points) % data.cols();
        } else {
            IndexLast = (IndexLast + data.cols() - extra_points) % data.cols();
        }
    }

    // Copy [IndexFirst, IndexLast] to [0, IndexLast - IndexFirst - 1]
    if (IndexFirst <= IndexLast) {
    Eigen::Index points_to_move = IndexLast - IndexFirst + 1;
        new_data.leftCols(points_to_move) = data.middleCols(IndexFirst, points_to_move);
    } else {
        new_data.leftCols(data.cols() - IndexFirst) = data.rightCols(data.cols() - IndexFirst);
        new_data.middleCols(data.cols() - IndexFirst, IndexLast + 1) = data.leftCols(IndexLast + 1);
    }

    data = std::move(new_data);
}

void vctPlot2DBase::Signal::SetColor(const Eigen::Vector3d& colorInRange0To1)
{
    Color = colorInRange0To1.cwiseMax(0.0).cwiseMin(1.0);
}

vctPlot2DBase::VerticalLine::VerticalLine(const std::string & name, const double x):
    Name(name),
    X(x),
    Visible(true),
    Color(1.0, 1.0, 1.0),
    LineWidth(1.0)
{}

void vctPlot2DBase::VerticalLine::SetX(const double x)
{
    this->X = x;
}

void vctPlot2DBase::VerticalLine::SetColor(const Eigen::Vector3d& colorInRange0To1)
{
    Color = colorInRange0To1.cwiseMax(0.0).cwiseMin(1.0);
}

vctPlot2DBase::vctPlot2DBase():
    BackgroundColor(0.1, 0.1, 0.1)
{
    SetNumberOfPoints(200);
    SetContinuousFitX(true);
    SetContinuousFitY(true);
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
    for (auto& scale : Scales) {
        scale.second->AutoFitX(padding);
    }
}

void vctPlot2DBase::FitX(double min, double max, double padding)
{
    for (auto& scale : Scales) {
        scale.second->FitX(min, max, padding);
    }
}

void vctPlot2DBase::AutoFitY(double padding)
{
    for (auto& scale : Scales) {
        scale.second->AutoFitY(padding);
    }
}

void vctPlot2DBase::FitY(double min, double max, double padding)
{
    for (auto& scale : Scales) {
        scale.second->FitY(min, max, padding);
    }
}

void vctPlot2DBase::Freeze(bool freeze)
{
    for (auto& scale : Scales) {
        scale.second->Freeze(freeze);
    }
}

bool vctPlot2DBase::GetFreeze(void) const
{
    bool allFrozen = true;
    for (auto& scale : Scales) {
        allFrozen = allFrozen && scale.second->GetFreeze();
    }

    return allFrozen;
}

void vctPlot2DBase::SetContinuousFitX(bool fit)
{
    for (auto& scale : Scales) {
        scale.second->SetContinuousFitX(fit);
    }
}

void vctPlot2DBase::SetContinuousFitY(bool fit)
{
    for (auto& scale : Scales) {
        scale.second->SetContinuousFitY(fit);
    }
}

void vctPlot2DBase::SetContinuousExpandY(bool expand)
{
    for (auto& scale : Scales) {
        scale.second->SetContinuousExpandY(expand);
    }
}

bool vctPlot2DBase::GetContinuousFitX(void) const
{
    bool continuous = true;
    for (auto& scale : Scales) {
        continuous = continuous && scale.second->GetContinuousFitX();
    }
    return continuous;
}

bool vctPlot2DBase::GetContinuousFitY(void) const
{
    bool continuous = true;
    for (auto& scale : Scales) {
        continuous = continuous && scale.second->GetContinuousFitY();
    }
    return continuous;
}

bool vctPlot2DBase::GetContinuousExpandY(void) const
{
    bool continuous = true;
    for (auto& scale : Scales) {
        continuous = continuous && scale.second->GetContinuousExpandY();
    }
    return continuous;
}

void vctPlot2DBase::ContinuousUpdate(void)
{
    for (auto& scale : Scales) {
        scale.second->ContinuousUpdate();
    }
}

void vctPlot2DBase::SetBackgroundColor(const Eigen::Vector3d& colorInRange0To1)
{
    BackgroundColor = colorInRange0To1.cwiseMax(0.0).cwiseMin(1.0);
}

vctPlot2DBase::Scale * vctPlot2DBase::AddScale(const std::string & name)
{
    // check if the name already exists
    const auto found = Scales.find(name);
    if (found != Scales.end()) {
        return nullptr;
    }

    Scales[name] = std::make_unique<Scale>(name);
    return Scales[name].get();
}

vctPlot2DBase::Scale * vctPlot2DBase::FindScale(const std::string & name)
{
    const auto found = Scales.find(name);
    if (found == Scales.end()) {
        return nullptr;
    }
    return found->second.get();
}

bool vctPlot2DBase::RemoveScale(const std::string & name)
{
    const auto found = Scales.find(name);
    if (found == Scales.end()) {
        return false;
    }
    Scales.erase(found);
    return true;
}

bool vctPlot2DBase::RemoveScale(const Scale * scale)
{
    if (scale) {
        return this->RemoveScale(scale->GetName());
    }
    return false;
}
