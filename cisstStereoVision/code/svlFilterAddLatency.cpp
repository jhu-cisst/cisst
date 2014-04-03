/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Daniel Obenshain, Thomas Tantillo, Anton Deguet
  Created on: 2010

  (C) Copyright 2010-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterAddLatency.h>
#include <cisstStereoVision/svlFilterInput.h>


CMN_IMPLEMENT_SERVICES(svlFilterAddLatency)

svlFilterAddLatency::svlFilterAddLatency():
    svlFilterBase(),
    OutputImage(0),
    FramesDelayed(0),
    Length(0),
    Head(0),
    Tail(0)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}


svlFilterAddLatency::~svlFilterAddLatency()
{
    Release();
}


int svlFilterAddLatency::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();

    svlStreamType type = GetInput()->GetType();

    switch (type) {
    case svlTypeImageRGB:
        OutputImage = dynamic_cast<svlSampleImage*>(svlSample::GetNewFromType(type));
        break;

    case svlTypeImageRGBStereo:
        OutputImage = dynamic_cast<svlSampleImage*>(svlSample::GetNewFromType(type));
        break;

    case svlTypeCUDAImageRGB:
    case svlTypeCUDAImageRGBA:
    case svlTypeCUDAImageRGBStereo:
    case svlTypeCUDAImageRGBAStereo:
    case svlTypeCUDAImageMono8:
    case svlTypeCUDAImageMono8Stereo:
    case svlTypeCUDAImageMono16:
    case svlTypeCUDAImageMono16Stereo:
    case svlTypeCUDAImageMono32:
    case svlTypeCUDAImageMono32Stereo:
    case svlTypeCUDAImage3DMap:
    case svlTypeCameraGeometry:
    case svlTypeInvalid:
    case svlTypeStreamSource:
    case svlTypeStreamSink:
    case svlTypeImageRGBA:
    case svlTypeImageRGBAStereo:
    case svlTypeImageMono8:
    case svlTypeImageMono8Stereo:
    case svlTypeImageMono16:
    case svlTypeImageMono16Stereo:
    case svlTypeImageMono32:
    case svlTypeImageMono32Stereo:
    case svlTypeImage3DMap:
    case svlTypeMatrixInt8:
    case svlTypeMatrixInt16:
    case svlTypeMatrixInt32:
    case svlTypeMatrixInt64:
    case svlTypeMatrixUInt8:
    case svlTypeMatrixUInt16:
    case svlTypeMatrixUInt32:
    case svlTypeMatrixUInt64:
    case svlTypeMatrixFloat:
    case svlTypeMatrixDouble:
    case svlTypeTransform3D:
    case svlTypeTargets:
    case svlTypeText:
    case svlTypeBlobs:
        CMN_LOG_CLASS_INIT_ERROR << "Initialize: input type \"" << GetInput()->GetType() << "\" not supported" << std::endl;
        return SVL_INVALID_INPUT_TYPE;
    }

    if (OutputImage == 0) {
        CMN_LOG_CLASS_INIT_ERROR << "Initialize: failed to create output for type \"" << GetInput()->GetType() << "\"" << std::endl;
        return SVL_FAIL;
    }
    OutputImage->SetSize(*syncInput);
    syncOutput = OutputImage;

    return SVL_OK;
}


int svlFilterAddLatency::Process(svlProcInfo * procInfo, svlSample * syncInput, svlSample * & syncOutput)
{
    syncOutput = OutputImage;

    svlSampleImage* input = dynamic_cast<svlSampleImage*>(syncInput);

    _OnSingleThread(procInfo)
    {
        if (Add(input) < 0) {
            CMN_LOG_CLASS_RUN_ERROR << "Process: failed to add input to latency linked list" << std::endl;
            return SVL_FAIL;
        }
        if (Remove(OutputImage) < 0) {
            CMN_LOG_CLASS_RUN_ERROR << "Process: failed to remove input from latency linked list" << std::endl;
            return SVL_FAIL;
        }
    }
    return SVL_OK;
}


int svlFilterAddLatency::Add(svlSampleImage * input)
{
    NodeType * node;
    svlStreamType type = GetInput()->GetType();

    while (Length < FramesDelayed) {
        //this repeats the oldest data on the head of the list
        node = (NodeType*) malloc(sizeof(NodeType)); // todo: replace with new?  adeguet1
        node->Data = dynamic_cast<svlSampleImage*>(svlSample::GetNewFromType(type));
        node->Data->SetSize(*input);

        if (Head == 0) {
            node->Data->CopyOf(OutputImage);
            node->Next = 0;
            Head = node;
            Tail = node;
        } else {
            node->Data->CopyOf(Head->Data);
            node->Next = Head;
            Head = node;
        }
        Length++;
    }


    // add the data in in to a new NodeType after the tail of the
    // list.

    // return 0 on success and -1 on failure
    node = (NodeType*) malloc(sizeof(NodeType));
    node->Data = dynamic_cast<svlSampleImage*>(svlSample::GetNewFromType(type));
    node->Data->SetSize(*input);
    node->Data->CopyOf(input);
    Length++;

    if (Head == 0) {
        Head = node;
        Tail = node;
        node->Next = 0;
        return 0;
    } else {
        Tail->Next = node;
        node->Next = 0;
        Tail = node;
        return 0;
    }
    // todo, do we ever get there?  adeguet1
    return -1;
}


int svlFilterAddLatency::Remove(svlSampleImage * output)
{
    // remove nodes not used anymore
    while (Length > FramesDelayed + 1) {
        if (Head != Tail) {
            NodeType * temp = Head;
            if (Head->Next == 0) {
                Head = Tail = 0;
            } else {
                Head = Head->Next;
            }
            delete temp->Data;
            temp->Data = 0;
            free(temp); // todo should we use delete for nodes?  adeguet1
            temp = 0;
        } else {
            delete Head->Data;
            Head->Data = 0;
            free(Head);
            Head = 0;
            Tail = 0;
        }
        Length--;
    }

    //remove the data at the head NodeType, move it to out
    //and move the list down by one (delete head)

    //return 0 on success and -1 on failure

    if (Head != 0) {
        output->CopyOf(Head->Data);
        if (Head != Tail) {
            NodeType * temp = Head;
            if (Head->Next == 0) {
                Head = Tail = 0;
            } else {
                Head = Head->Next;
            }
            delete temp->Data;
            temp->Data = 0;
            free(temp); // todo, delete?  adeguet1
            temp = 0;
        } else {
            delete Head->Data;
            Head->Data = 0;
            free(Head);
            Head = 0;
            Tail = 0;
        }
        Length--;
        return 0;
    }
    return -1;
}


void svlFilterAddLatency::UpLatency(void)
{
    // assuming 30 fps, 3 frames to delay 100 ms
    FramesDelayed += 3;
    CMN_LOG_CLASS_RUN_DEBUG << "UpLatency: latency is now " << (FramesDelayed * 1000.0) / 30.0 << " ms." << std::endl;
}


void svlFilterAddLatency::DownLatency(void)
{
    unsigned int i;
    // assuming 30 fps, 3 frames to delay 100 ms
    for (i = 0; i < 3; i++)    {
        if (FramesDelayed > 0) {
            FramesDelayed--;
        }
    }
    CMN_LOG_CLASS_RUN_DEBUG << "DownLatency: latency is now " << (FramesDelayed * 1000.0) / 30.0 << " ms." << std::endl;
}


void svlFilterAddLatency::SetFrameDelayed(const unsigned int numberOfFrames)
{
    FramesDelayed = numberOfFrames;
    CMN_LOG_CLASS_RUN_DEBUG << "DownLatency: latency is now " << (FramesDelayed * 1000.0) / 30.0 << " ms." << std::endl;
}


int svlFilterAddLatency::Release(void)
{
    if (OutputImage) {
        delete OutputImage;
        // TODO: delete structure list
        OutputImage = 0;
    }
    return SVL_OK;
}
