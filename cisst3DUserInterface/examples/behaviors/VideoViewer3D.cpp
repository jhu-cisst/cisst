/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2009-09-13

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <VideoViewer3D.h>

#include <cisstStereoVision/svlFilterBase.h>
#include <cisstStereoVision/svlFilterOutput.h>

#include <cisst3DUserInterface/ui3ImagePlane.h>
#include <cisst3DUserInterface/ui3Manager.h>

VideoViewer3D::VideoViewer3D(const std::string & name):
    ui3BehaviorBase(std::string("VideoViewer3D::") + name, 0),
    Stream(1),
    Source(1),
    Widget3D(0),
    Widget3DHandlesActive(true),
	ImagePlane(0),
	Playing(false)
{
	// add video source interfaces
    AddStream(svlTypeImageRGB, "Video");
    this->Widget3D = new ui3Widget3D("VideoViewer3D");
    CMN_ASSERT(this->Widget3D);
    this->AddWidget3D(this->Widget3D);
}


VideoViewer3D::~VideoViewer3D()
{
    if (this->Widget3D) {
        delete this->Widget3D;
    }
}


void VideoViewer3D::Configure(const std::string & filename)
{
	if (filename == "") {
        this->Source.DialogFilePath();
	} else {
        if (this->Source.SetFilePath(filename) != SVL_OK) {
            CMN_LOG_CLASS_INIT_ERROR << "Configure: wrong file name \"" << filename << "\"" << std::endl;
        }
	}

	this->Stream.SetSourceFilter(&this->Source);
	this->Source.GetOutput()->Connect(this->GetStreamSamplerFilter("Video")->GetInput());
    this->Stream.Initialize();
	this->Stream.Play();
}


void VideoViewer3D::Startup()
{
    // Adding image plane
    ImagePlane = new ui3ImagePlane();
    CMN_ASSERT(ImagePlane);

    // Get bitmap dimensions from pipeline.
    // The pipeline has to be already initialized to get the required info.
    ImagePlane->SetBitmapSize(GetStreamWidth("Video"), GetStreamHeight("Video"));

    // Set plane size (dimensions are already in millimeters), miltuplied by a scaling factor to fit probe dimensions
    ImagePlane->SetPhysicalSize(20.0, 20.0);

    // Change pivot position to move plane to the right location.
    // The pivot point will remain in the origin, only the plane moves.
    ImagePlane->SetPhysicalPositionRelativeToPivot(vct3(0.0, 0.0, 0.0));

    this->ImagePlane->Lock();
	this->Widget3D->Add(this->ImagePlane);
	this->ImagePlane->Show();
    this->ImagePlane->Unlock();
}


void VideoViewer3D::Cleanup(void)
{
	this->Stream.Release();
}


void VideoViewer3D::ToggleHandles(void)
{
    if (this->Widget3D->HandlesActive()) {
        this->Widget3DHandlesActive = false;
        this->Widget3D->SetHandlesActive(false);
    } else {
        this->Widget3DHandlesActive = false;
        this->Widget3D->SetHandlesActive(true);
    }
}


void VideoViewer3D::StartStop(void)
{
	if (!this->Playing) {
        this->Stream.Play();
		Playing = true;
	} else {
		this->Stream.Suspend();
		Playing = false;
	}
}


void VideoViewer3D::ConfigureMenuBar(void)
{
    this->MenuBar->AddClickButton("StartStop",
                                  1,
                                  "empty.png",
                                  &VideoViewer3D::StartStop,
                                  this);
    this->MenuBar->AddClickButton("Move",
                                  2,
                                  "move.png",
                                  &VideoViewer3D::ToggleHandles,
                                  this);
}


bool VideoViewer3D::RunForeground(void)
{
    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->Widget3D->Show();
        this->Widget3D->SetHandlesActive(this->Widget3DHandlesActive);
    }

    // detect transition, should that be handled as an event?
    // State is used by multiple threads ...
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->Widget3D->Show();
        this->Widget3D->SetHandlesActive(this->Widget3DHandlesActive);
    }
    // running in foreground GUI mode
    prmPositionCartesianGet position;
    this->GetPrimaryMasterPosition(position);
    return true;
}

bool VideoViewer3D::RunBackground(void)
{
    // detect transition
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->Widget3D->Show();
        this->Widget3D->SetHandlesActive(false);
    }
    return true;
}

bool VideoViewer3D::RunNoInput(void)
{
    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->Widget3D->SetHandlesActive(false);
    }
    return true;
}


void VideoViewer3D::OnStreamSample(svlSample * sample, int streamindex)
{
    if (State == Foreground) {
        ImagePlane->SetImage(dynamic_cast<svlSampleImage *>(sample), streamindex);
    }
}


void VideoViewer3D::OnQuit(void)
{
    this->Widget3D->Hide();
}


void VideoViewer3D::OnStart(void)
{
    this->Position.X() = 0.0;
    this->Position.Y() = 0.0;
    this->Position.Z() = -50.0;
    this->Widget3D->SetPosition(this->Position);
    this->Widget3D->SetSize(10.0);
    this->Widget3D->Show();

	this->Position.X() = 20.0/2;
    this->Position.Y() = 20.0/2;
    this->Position.Z() = 0.0;
	this->ImagePlane->SetPosition(this->Position);
}
