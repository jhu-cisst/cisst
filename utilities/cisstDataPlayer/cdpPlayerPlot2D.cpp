/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cdpPlayerExample.cpp 2308 2011-02-15 22:01:00Z adeguet1 $

  Author(s): Marcin Balicki
  Created on: 2011-02-10

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "cdpPlayerPlot2D.h"

#include <math.h>
#include <QMenu>
#include <QFileDialog>
#include <QLabel>
#include <QDoubleSpinBox>
#include <cisstOSAbstraction/osaGetTime.h>


#include <iostream>
#include <sstream>

CMN_IMPLEMENT_SERVICES(cdpPlayerPlot2D);

cdpPlayerPlot2D::cdpPlayerPlot2D(const std::string & name, double period):
    cdpPlayerBase(name, period)
{

    // create the user interface
    ExWidget.setupUi(&Widget);
    mainWidget = new QWidget();
    ScaleZoom = new QDoubleSpinBox(mainWidget);
    ScaleZoom->setValue(1);
    ZoomInOut = new QLabel(mainWidget);
    ZoomInOut->setText("Set Scale");

    // create the user interface
    Plot = new vctPlot2DOpenGLQtWidget(mainWidget);
    Plot->SetNumberOfPoints(100000);
    TracePointer = Plot->AddTrace("Data");
    VerticalLinePointer = Plot->AddVerticalLine("X");

    CentralLayout = new QGridLayout(mainWidget);

    CentralLayout->setContentsMargins(0, 0, 0, 0);
    CentralLayout->setRowStretch(0, 1);
    CentralLayout->setColumnStretch(1, 1);
    CentralLayout->addWidget(Plot, 0, 0, 1, 2);
    CentralLayout->addWidget(ScaleZoom, 1, 1, 1, 2);
    CentralLayout->addWidget(ZoomInOut, 1, 0, 1, 1);

    CentralLayout->addWidget(this->GetWidget(),2,0,4,4);
    mainWidget->resize(300,500);

    // Add elements to state table
    StateTable.AddData(ZoomScaleValue,  "ZoomScale");
    StateTable.AddData(VectorIndex,  "VectorIndex");

    mtsInterfaceProvided * provided = AddInterfaceProvided("Provides2DPlot");
    if (provided) {
        provided->AddCommandReadState(StateTable, ZoomScaleValue,         "GetZoomScale");
        provided->AddCommandReadState(StateTable, VectorIndex,        "GetVectorIndex");
        provided->AddCommandWrite(&cdpPlayerPlot2D::SetVectorIndex, this, "SetVectorIndex", mtsInt() );
    }
    // Connect to ourself, for Qt Thread

    mtsInterfaceRequired * interfaceRequired = AddInterfaceRequired("Get2DPlotStatus");
    if (interfaceRequired) {        
        interfaceRequired->AddFunction("GetZoomScale", Plot2DAccess.GetZoomScale);        
        interfaceRequired->AddFunction("GetVectorIndex", Plot2DAccess.GetVectorIndex);
        interfaceRequired->AddFunction("SetVectorIndex",  Plot2DAccess.WriteVectorIndex);        
    }

    ZoomScaleValue = 1;
    VectorIndex = 0;


}


cdpPlayerPlot2D::~cdpPlayerPlot2D()
{
}


void cdpPlayerPlot2D::MakeQTConnections(void)
{
    QObject::connect(ExWidget.PlayButton, SIGNAL(clicked()),
                     this, SLOT(QSlotPlayClicked()));

    QObject::connect(ExWidget.TimeSlider, SIGNAL(sliderMoved(int)),
                     this, SLOT(QSlotSeekSliderMoved(int)));

    QObject::connect(ExWidget.SyncCheck, SIGNAL(clicked(bool)),
                     this, SLOT(QSlotSyncCheck(bool)));

    QObject::connect(ExWidget.StopButton, SIGNAL(clicked()),
                     this, SLOT(QSlotStopClicked()));

    QObject::connect(ExWidget.SetSaveStartButton, SIGNAL(clicked()),
                     this, SLOT(QSlotSetSaveStartClicked()));

    QObject::connect(ExWidget.SetSaveEndButton, SIGNAL(clicked()),
                     this, SLOT(QSlotSetSaveEndClicked()));

    QObject::connect(ExWidget.OpenFileButton, SIGNAL(clicked()),
                     this, SLOT(QSlotOpenFileClicked()));

    QObject::connect(ScaleZoom , SIGNAL(valueChanged(double)),
                     this, SLOT(QSlotSpinBoxValueChanged(double)));
}


void cdpPlayerPlot2D::Configure(const std::string & CMN_UNUSED(filename))
{
    MakeQTConnections();
    Widget.setWindowTitle(QString::fromStdString(GetName()));
    Widget.show();
    mainWidget->show();
    ResetPlayer();
}


void cdpPlayerPlot2D::Startup(void)
{
    LoadData();
    UpdateLimits();
}


void cdpPlayerPlot2D::Run(void)
{
    ProcessQueuedEvents();
    ProcessQueuedCommands();

    //update the model (load data) etc.
    if (State == PLAY) {

        double currentTime = TimeServer.GetAbsoluteTimeInSeconds();
        Time = currentTime - PlayStartTime.Timestamp() + PlayStartTime.Data;

        if (Time > PlayUntilTime)  {
            Time = PlayUntilTime;
            State = STOP;
        }
        else {
            //Load and Prep current data
            // update plot
            if (Data.size() != 0) 
                UpdatePlot();
        }
    }
    //make sure we are at the correct seek position.
    else if (State == SEEK) {
        if(LastTime != Time){
            if (TimeStamps.size() != 0) {
                size_t i = 0;
                //seek to where it really is
                for (i = 0; i < TimeStamps.size()-1;i++) {
                    if (TimeStamps.at(i) <=Time.Data && TimeStamps.at(i+1) >= Time.Data) 
                        break;		
                }

                if (TimeStamps.at(0) >= Time.Data) 
                    VectorIndex = 0;
                else
                    VectorIndex = i;

                PlayStartTime = Time;
                LastTime = Time;            
                UpdatePlot();
            }
        }else
            State = STOP;
    }
    else if (State == STOP) {
        //do Nothing
    }

    //now display updated data in the qt thread space.
    if (Widget.isVisible()) {
        emit QSignalUpdateQT();
    }
}


void cdpPlayerPlot2D::UpdatePlot(void)
{
    double dataToPlot = 0.0;
    double timeStamp = 0.0;
    double ScaleValue = 0.0; 
    mtsInt index;

    Plot2DAccess.GetVectorIndex(index);       
    Plot2DAccess.GetZoomScale(ScaleValue);

     // if no data, do nothing
    if ((unsigned int)index.Data >= TimeStamps.size()) {
        return;
    }

    // find out current timeStamps & data
    timeStamp =TimeStamps.at(index.Data );
    // set vertical line position

    while ((unsigned int) index < TimeStamps.size() && Time.Data >= timeStamp) {
        dataToPlot = Data.at(index.Data );
        timeStamp = TimeStamps.at(index.Data );
        index++;        
    }
    Plot2DAccess.WriteVectorIndex(index);

/*
    // Calculate Scale
    // This section will "forze" graph while it is at starting point or end point
    double minShow, maxShow;

    minShow = ((Time.Data-ZoomScaleValue) < PlayerDataInfo.DataStart()) ? PlayerDataInfo.DataStart(): (Time.Data-ZoomScaleValue);
    maxShow =((Time.Data+ZoomScaleValue) > PlayerDataInfo.DataEnd()) ?  PlayerDataInfo.DataEnd(): (Time.Data+ZoomScaleValue);

    if (maxShow - minShow < ZoomScaleValue*2 && minShow ==  PlayerDataInfo.DataStart()) {
        maxShow = PlayerDataInfo.DataStart() +ZoomScaleValue*2;
    } else if (maxShow - minShow < ZoomScaleValue*2  && maxShow ==  PlayerDataInfo.DataEnd()) {
         minShow = PlayerDataInfo.DataEnd() -ZoomScaleValue*2;
    }
*/

    Plot->SetContinuousFitX(false);
    Plot->FitX(Time.Data-ScaleValue ,  Time.Data+ScaleValue, 0);
    VerticalLinePointer->SetX(Time.Data);
    // UpdateGL should be called at Qt thread
}

//in QT thread space
void cdpPlayerPlot2D::UpdateQT(void)
{
    mtsDouble timevalue;
    BaseAccess.GetTime(timevalue);
    if (State == PLAY) {
        //Display the last datasample before Time.
        ExWidget.TimeSlider->setValue((int)timevalue.Data);
        //update Plot in Qt Thread
        if(Plot)
            Plot->updateGL();
    }
    //Make sure we are at the correct seek location.
    else if (State == STOP) {
        //Optional: Test if the data needs to be updated:
        ExWidget.TimeSlider->setValue((int)timevalue.Data);
    }
    else if (State == SEEK) { 
        //Optional: Test if the data needs to be updated:
        ExWidget.TimeSlider->setValue((int)timevalue.Data);
        //update Plot in Qt Thread
        if(Plot)
            Plot->updateGL();
    }   
    ExWidget.TimeLabel->setText(QString::number(timevalue.Data,'f', 3));
}


void cdpPlayerPlot2D::Play(const mtsDouble & time)
{
    if (Sync) {
        CMN_LOG_CLASS_RUN_DEBUG << "Play " << PlayStartTime << std::endl;
        State = PLAY;
        PlayUntilTime = PlayerDataInfo.DataEnd();
        PlayStartTime = time;
    }
}


void cdpPlayerPlot2D::Stop(const mtsDouble & time)
{
    if (Sync) {
        CMN_LOG_CLASS_RUN_DEBUG << "Stop " << time << std::endl;
        PlayUntilTime = time;
    }
}


void cdpPlayerPlot2D::Seek(const mtsDouble & time)
{
    if (Sync) {
        CMN_LOG_CLASS_RUN_DEBUG << "Seek " << time << std::endl;

        State = SEEK;
        PlayUntilTime = PlayerDataInfo.DataEnd();
        BaseAccess.WriteTime(time);
    }
}


void cdpPlayerPlot2D::Save(const cdpSaveParameters & saveParameters)
{
    if (Sync) {
        CMN_LOG_CLASS_RUN_DEBUG << "Save " << saveParameters << std::endl;
    }
}


void cdpPlayerPlot2D::Quit(void)
{
    CMN_LOG_CLASS_RUN_DEBUG << "Quit" << std::endl;
    this->Kill();
}


void cdpPlayerPlot2D::QSlotPlayClicked(void)
{
    mtsDouble playTime; //this should be read from the state table!!!
    BaseAccess.GetTime(playTime);
    playTime.Timestamp() = TimeServer.GetAbsoluteTimeInSeconds();

    if (Sync) {
        PlayRequest(playTime);
    } else {
        //not quite thread safe, if there is mts play call this can be corrupt.
        State = PLAY;
        PlayUntilTime = PlayerDataInfo.DataEnd();
        PlayStartTime = playTime;
    }
}


void cdpPlayerPlot2D::QSlotSeekSliderMoved(int c)
{
    mtsDouble t = c;

    if (Sync) {
        SeekRequest(t);
    } else {
        State = SEEK;
        BaseAccess.WriteTime(t);
    }
    PlayUntilTime = PlayerDataInfo.DataEnd();
}


void cdpPlayerPlot2D::QSlotSyncCheck(bool checked)
{
    Sync = checked;
}


void cdpPlayerPlot2D::QSlotStopClicked(void)
{
    mtsDouble now = Time;

    if (Sync) {
        StopRequest(now);
    } else {
        PlayUntilTime = now;
    }
}


void cdpPlayerPlot2D::LoadData(void)
{
    //PlayerDataInfo.DataStart() = 1297723451.415;
    //PlayerDataInfo.DataEnd() = 1297723900.022;

    if (Data.size() == 0) {
        PlayerDataInfo.DataStart() = 0;
        PlayerDataInfo.DataEnd() = 1;
    }

    if (Time < PlayerDataInfo.DataStart()) {
        Time = PlayerDataInfo.DataStart();
    }

    if (Time > PlayerDataInfo.DataEnd()) {
        Time = PlayerDataInfo.DataEnd();
    }

    //This is the standard.
    PlayUntilTime = PlayerDataInfo.DataEnd();

    UpdatePlayerInfo(PlayerDataInfo);
    UpdateLimits();
}


void cdpPlayerPlot2D::QSlotSetSaveStartClicked(void)
{
    mtsDouble timevalue;
    BaseAccess.GetTime(timevalue);
    ExWidget.SaveStartSpin->setValue(timevalue.Data);
}


void cdpPlayerPlot2D::QSlotSetSaveEndClicked(void)
{
    mtsDouble timevalue;
    BaseAccess.GetTime(timevalue);
    ExWidget.SaveEndSpin->setValue(timevalue.Data);
}


void cdpPlayerPlot2D::QSlotOpenFileClicked(void)
{
    // read data and update relatives
    OpenFile();
    LoadData();
    UpdateLimits();
}


// Executed in Qt Thread
void cdpPlayerPlot2D::QSlotSpinBoxValueChanged(double value)
{
    if (value*2 > (PlayerDataInfo.DataEnd() - PlayerDataInfo.DataStart())) 
        value = (PlayerDataInfo.DataEnd() - PlayerDataInfo.DataStart())/2;        
    
    ZoomScaleValue = value;
    ScaleZoom->setValue(ZoomScaleValue);    
    UpdatePlot();
}


// read data from file
void cdpPlayerPlot2D::OpenFile(void)
{
   QString result;

    // clear Data and TimeStamps
    Data.clear();
    TimeStamps.clear();

    result = QFileDialog::getOpenFileName(mainWidget, "Open File", 0, 0);
    if (!result.isNull()) {
        size_t i = 0;

        // read Data from file
	    ExtractDataFromStateTableCSVFile(result);

        // reinitial plot widget
        Plot = new vctPlot2DOpenGLQtWidget(mainWidget);
        Plot->SetNumberOfPoints(Data.size());
        TracePointer = Plot->AddTrace("Data");
        VerticalLinePointer = Plot->AddVerticalLine("X");
        for(i = 0 ; i < Data.size(); i++)
            TracePointer->AddPoint(vctDouble2(TimeStamps.at(i), Data.at(i)));
        CentralLayout->addWidget(Plot, 0, 0, 1, 2);

        ResetPlayer();
        UpdatePlot();
    }
}


void cdpPlayerPlot2D::UpdateLimits()
{
    ExWidget.TimeSlider->setRange((int)PlayerDataInfo.DataStart(), (int)PlayerDataInfo.DataEnd());

    ExWidget.TimeStartLabel->setText(QString::number(PlayerDataInfo.DataStart(),'f', 3));
    ExWidget.TimeEndLabel->setText(QString::number(PlayerDataInfo.DataEnd(),'f', 3));

    ExWidget.SaveStartSpin->setRange(PlayerDataInfo.DataStart(), PlayerDataInfo.DataEnd());
    ExWidget.SaveEndSpin->setRange(PlayerDataInfo.DataStart(), PlayerDataInfo.DataEnd());
}


// Only extract time and data field
bool cdpPlayerPlot2D::ExtractDataFromStateTableCSVFile(QString & path)
{
    char charPeeked;
    char DoubleNumber[100];
    char TimeStamp[100];
    std::ifstream inf(path.toAscii());

     // clear Data and TimeStamps
    Data.clear();
    TimeStamps.clear();

    // no file exists
    if (! inf.is_open()) {
        return false;
    }

    do {
        charPeeked = inf.peek();
        if (charPeeked != '#') 
            break;	    
    } while (!inf.getline(DoubleNumber, 100).eof());

    while (!inf.getline(DoubleNumber, 100, ',').eof()) {
        // get last number as value
        inf.getline(TimeStamp,100, ',');
        double timeFromFile =strtod(TimeStamp, NULL);
        TimeStamps.push_back(timeFromFile);

        inf.ignore(100, ',');
        inf.ignore(100, ',');
        inf.getline(DoubleNumber, 100);

        double dataFromFile =strtod(DoubleNumber, NULL);
        Data.push_back(dataFromFile);
    }

    PlayerDataInfo.DataStart() = TimeStamps[0];
    PlayerDataInfo.DataEnd() = TimeStamps.back();

    return true;
}


// reset player to initial state
//! TODO: make this function thread safe
void cdpPlayerPlot2D::ResetPlayer(void)
{    
    ZoomScaleValue = (PlayerDataInfo.DataStart() != 0) ? ((PlayerDataInfo.DataEnd() - PlayerDataInfo.DataStart()) / 2.0) : 1.0 ;
    ScaleZoom->setValue(ZoomScaleValue);    
    BaseAccess.WriteTime(0.0);
    Plot2DAccess.WriteVectorIndex(0); 
}

