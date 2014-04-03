/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2009 

  (C) Copyright 2006-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "svlVidCapSrcDirectShowInputSelector.h"


/**************************************************/
/*** svlVidCapSrcDirectShowInputSelector class ****/
/**************************************************/

svlVidCapSrcDirectShowInputSelector::svlVidCapSrcDirectShowInputSelector(IPin *pin) :
    StartingPin(pin),
    CurrentRoutingIndex(0)
{
    memset(&RoutingRoot, 0, sizeof(_Routing));
    BuildRoutingList(pin, &RoutingRoot, 0);
}

svlVidCapSrcDirectShowInputSelector::~svlVidCapSrcDirectShowInputSelector()
{
    DestroyRoutingList();
    RoutingList.clear();
}

bool svlVidCapSrcDirectShowInputSelector::BuildRoutingList(IPin *startinginputpin, _Routing *routing, int depth)
{
    LONG inputindexrelated, outputindexrelated;
    LONG inputphysicaltype, outputphysicaltype;
    LONG inputs, outputs, inputindex, outputindex;

    IPin *pin = 0;
    IPin *startoutputpin = 0;
    PIN_INFO pininfo;
    _Routing routingnext;
    _Routing *current, *pr;
    IAMCrossbar *xbar = 0;
    int j;

    if (!startinginputpin || !routing) return false;

    if (S_OK != startinginputpin->ConnectedTo(&startoutputpin)) return false;

    if (S_OK == startoutputpin->QueryPinInfo(&pininfo)) {

        if (S_OK == pininfo.pFilter->QueryInterface(IID_IAMCrossbar, (void**)&xbar)) {

            if (S_OK == xbar->get_PinCounts(&outputs, &inputs) &&
                GetCrossbarIndexFromIPin(xbar, &outputindex, false, startoutputpin) &&
                S_OK == xbar->get_CrossbarPinInfo(FALSE, outputindex, &outputindexrelated, &outputphysicaltype)) {

                for (inputindex = 0; inputindex < inputs; inputindex++) {

                    if (S_OK == xbar->get_CrossbarPinInfo(TRUE, inputindex, &inputindexrelated, &inputphysicaltype) &&
                        inputphysicaltype < PhysConn_Audio_Tuner && 
                        S_OK == xbar->CanRoute(outputindex, inputindex) &&
                        GetCrossbarIPinAtIndex(xbar, inputindex, true, &pin)) {

                        memset(&routingnext, 0, sizeof(_Routing));

                        routingnext.RightRouting = routing;
                        routing->LeftRouting = &routingnext;

                        routing->Xbar = xbar;
                        routing->VideoInputIndex = inputindex;
                        routing->VideoOutputIndex = outputindex;
                        routing->InputPhysicalType = inputphysicaltype;
                        routing->OutputPhysicalType = outputphysicaltype;
                        routing->Depth = depth;

                        if (!BuildRoutingList(pin, &routingnext, depth + 1)) {
                            routing->LeftRouting = 0;

                            current = routing;
                            pr = new _Routing[depth + 1];
                            RoutingList.push_back(pr);

                            for (j = 0; j <= depth; j ++, pr ++) {
                                *pr = *current;

                                current->Xbar->AddRef();
                                current = current->RightRouting;

                                pr->LeftRouting  = pr - 1;
                                pr->RightRouting = pr + 1;

                                if (j == 0) pr->LeftRouting = 0;
                                if (j == depth) pr->RightRouting = 0;
                            }
                        }
                    }
                }
            }

            xbar->Release();
        }
        else {
            pininfo.pFilter->Release();
            startoutputpin->Release();

            return false;
        }

        pininfo.pFilter->Release();
    }

    startoutputpin->Release();

    return true;
}

bool svlVidCapSrcDirectShowInputSelector::DestroyRoutingList()
{
    int k, depth;
    _Routing *current = 0, *first = 0;

    while (RoutingList.size()) {
        current = first = RoutingList.front();
        RoutingList.pop_front();

        if (current) {
            depth = current->Depth + 1;

            for (k = 0; k < depth; k ++)  {
                current->Xbar->Release();
                current = current->RightRouting;
            }
        }

        delete [] first;
    }

    return true;
}

bool svlVidCapSrcDirectShowInputSelector::GetCrossbarIPinAtIndex(IAMCrossbar *xbar, int pinindex, bool isinputpin, IPin **pin)
{
    LONG cntinpins, cntoutpins;
    IPin *tpin = 0;
    IBaseFilter *filter = 0;
    IEnumPins *pins = 0;
    ULONG n;

    if (!xbar || !pin || S_OK != xbar->get_PinCounts(&cntoutpins, &cntinpins)) return false;

    int trueindex = isinputpin ? pinindex : pinindex + cntinpins;

    *pin = 0;

    if (S_OK == xbar->QueryInterface(IID_IBaseFilter, (void**)&filter))  {
        if (filter->EnumPins(&pins) >= 0) {
            int i = 0;
            while (pins->Next(1, &tpin, &n) == S_OK)  {
                tpin->Release();
                if (i == trueindex) {
                    *pin = tpin;
                    break;
                }
                i ++;
            }
            pins->Release();
        }
        filter->Release();
    }

    return *pin ? true : false; 
}

bool svlVidCapSrcDirectShowInputSelector::GetCrossbarIndexFromIPin(IAMCrossbar *xbar, LONG *pinindex, bool isinputpin, IPin *pin)
{
    LONG cntinpins, cntoutpins;
    IPin *tpin = 0;
    IBaseFilter *filter = 0;
    IEnumPins *pins = 0;
    ULONG n;
    bool success = false;

    if (!xbar || !pinindex || !pin || S_OK != xbar->get_PinCounts(&cntoutpins, &cntinpins)) return false;

    if (S_OK == xbar->QueryInterface(IID_IBaseFilter, (void**)&filter)) {
        if(filter->EnumPins(&pins) >= 0) {
            int i = 0;
            while(pins->Next(1, &tpin, &n) == S_OK) {
                tpin->Release();
                if (pin == tpin) {
                    *pinindex = isinputpin ? i : i - cntinpins;
                    success = true;
                    break;
                }
                i++;
            }
            pins->Release();
        }
        filter->Release();
    }

    return success;
}

int svlVidCapSrcDirectShowInputSelector::GetInputCount()
{
    return (int)RoutingList.size();
}

int svlVidCapSrcDirectShowInputSelector::GetInputType(int index)
{
    if (index < 0 || index >= (int)RoutingList.size()) return -1;

    _Routing* current = 0;
    std::list<_Routing*>::iterator iter = RoutingList.begin();
    for (int i = 0; i < index && iter != RoutingList.end(); i ++, iter ++) {}
    current = *iter;

    return current->InputPhysicalType;
}

bool svlVidCapSrcDirectShowInputSelector::GetInputName(int index, std::string & name)
{
    if (index < 0 || index >= (int)RoutingList.size()) return false;

    _Routing* current = 0;
    std::list<_Routing*>::iterator iter = RoutingList.begin();
    for (int i = 0; i < index && iter != RoutingList.end(); i ++, iter ++) {}
    current = *iter;

    if (!current) return false;

    switch (current->InputPhysicalType)  {   
        case PhysConn_Video_Tuner:              name = "Video Tuner";           break;
        case PhysConn_Video_Composite:          name = "Video Composite";       break;
        case PhysConn_Video_SVideo:             name = "Video SVideo";          break;
        case PhysConn_Video_RGB:                name = "Video RGB";             break;
        case PhysConn_Video_YRYBY:              name = "Video YRYBY";           break;
        case PhysConn_Video_SerialDigital:      name = "Video SerialDigital";   break;
        case PhysConn_Video_ParallelDigital:    name = "Video ParallelDigital"; break;
        case PhysConn_Video_SCSI:               name = "Video SCSI";            break;
        case PhysConn_Video_AUX:                name = "Video AUX";             break;
        case PhysConn_Video_1394:               name = "Video 1394";            break;
        case PhysConn_Video_USB:                name = "Video USB";             break;
        case PhysConn_Video_VideoDecoder:       name = "Video Decoder";         break;
        case PhysConn_Video_VideoEncoder:       name = "Video Encoder";         break;
        case PhysConn_Audio_Tuner:              name = "Audio Tuner";           break;
        case PhysConn_Audio_Line:               name = "Audio Line";            break;
        case PhysConn_Audio_Mic:                name = "Audio Mic";             break;
        case PhysConn_Audio_AESDigital:         name = "Audio AESDigital";      break;
        case PhysConn_Audio_SPDIFDigital:       name = "Audio SPDIFDigital";    break;
        case PhysConn_Audio_SCSI:               name = "Audio SCSI";            break;
        case PhysConn_Audio_AUX:                name = "Audio AUX";             break;
        case PhysConn_Audio_1394:               name = "Audio 1394";            break;
        case PhysConn_Audio_USB:                name = "Audio USB";             break;
        case PhysConn_Audio_AudioDecoder:       name = "Audio Decoder";         break;
        default:                                name = "Unknown";               break;
    }

    return true;
}

bool svlVidCapSrcDirectShowInputSelector::SetInputIndex(int index)
{
    if (index < 0 || index >= (int)RoutingList.size()) return false;

    _Routing* current = 0;
    std::list<_Routing*>::iterator iter = RoutingList.begin();
    for (int i = 0; i < index && iter != RoutingList.end(); i ++, iter ++) {}
    current = *iter;

    int depth= current->Depth + 1;

    for (int j = 0; j < depth; j ++)  {
        if (S_OK != current->Xbar->Route(current->VideoOutputIndex, current->VideoInputIndex)) return false;
        current ++;
    }

    CurrentRoutingIndex = index;

    return true;
}

int svlVidCapSrcDirectShowInputSelector::GetInputIndex()
{
    return CurrentRoutingIndex;
}
