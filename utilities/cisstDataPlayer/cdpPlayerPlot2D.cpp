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
    ZoomInOut->setText("Set Visualization Scale");
    ScaleZoom->setMaximum(9999);

    // create the user interface
    Plot = new vctPlot2DOpenGLQtWidget(mainWidget);
    Plot->SetNumberOfPoints(100);
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


    //// Add Parser Thread
    //taskManager = mtsTaskManager::GetInstance();
    //taskManager->AddComponent(&Parser);
}


cdpPlayerPlot2D::~cdpPlayerPlot2D()
{
    // cleanup
    //taskManager->KillAll();
    //taskManager->Cleanup();
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
    // Start Parser Thread
    //taskManager->CreateAll();
    //taskManager->StartAll();
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
    
    CS.Enter();
    //update the model (load data) etc.
    if (State == PLAY) {

        double currentTime = TimeServer.GetAbsoluteTimeInSeconds();
        Time = currentTime - PlayStartTime.Timestamp() + PlayStartTime.Data;

        if (Time.Data > PlayUntilTime.Data)  {
            Time = PlayUntilTime;
            State = STOP;
        }
        else {            
            if((ZoomScaleValue)  > (TimeBoundary-Time )*(0.8) && TimeBoundary <  PlayUntilTime.Data){
                Parser.LoadDataFromFile(TracePointer, Time, ZoomScaleValue, false);
                //Parser.TriggerLoadDataFromFile(TracePointer, Time, ZoomScaleValue, false);
                Parser.GetBoundary(TracePointer, TopBoundary, LowBoundary);
                TimeBoundary  =TopBoundary;
            }
            // update plot
            UpdatePlot();
        }
    }
    //make sure we are at the correct seek position.
    else if (State == SEEK) {
        //// Everything here should be moved to Qt thread since we have to re-alloc a new Plot object
        //size_t i = 0;
        if(LastTime.Data != Time.Data ){          
            LastTime = Time;
            PlayStartTime = Time;
            Parser.LoadDataFromFile(TracePointer, Time, ZoomScaleValue, true);
            //Parser.TriggerLoadDataFromFile(TracePointer, Time, ZoomScaleValue, true);
            Parser.GetBoundary(TracePointer, TopBoundary, LowBoundary);
            TimeBoundary  =TopBoundary;
            // update plot
            UpdatePlot();
        }
    }
    else if (State == STOP) {
        //do Nothing

        //// update plot
        //UpdatePlot();
    }
    
    CS.Leave();
    //now display updated data in the qt thread space.
    if (Widget.isVisible()) {
        emit QSignalUpdateQT();
    }
}


void cdpPlayerPlot2D::UpdatePlot(void)
{
    double timeStamp = 0.0;
    double ScaleValue = 0.0; 
    mtsInt index;

    Plot2DAccess.GetVectorIndex(index);       
    Plot2DAccess.GetZoomScale(ScaleValue);

    Plot->SetContinuousFitX(false);    
    Plot->FitX(Time.Data-ScaleValue ,  Time.Data+ScaleValue, 0);
    VerticalLinePointer->SetX(Time.Data);
    // UpdateGL should be called at Qt thread
}

//in QT thread space
void cdpPlayerPlot2D::UpdateQT(void)
{
    mtsDouble timevalue;
    CS.Enter();
    //BaseAccess.GetTime(timevalue);
    timevalue = Time;
    if (State == PLAY) {
        //Display the last datasample before Time.
        ExWidget.TimeSlider->setValue((int)timevalue.Data);
        //update Plot in Qt Thread
        if(Plot)
            Plot->updateGL();
    }    
    else if (State == STOP) {
        //Optional: Test if the data needs to be updated:
        ExWidget.TimeSlider->setValue((int)timevalue.Data);
        //update Plot in Qt Thread
        if(Plot)
            Plot->updateGL();
    }
    else if (State == SEEK) {     
        //Optional: Test if the data needs to be updated:
        ExWidget.TimeSlider->setValue((int)timevalue.Data);
        //update Plot in Qt Thread
        if(Plot)
            Plot->updateGL();
    }   
    CS.Leave();
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
    static mtsDouble lasttime =0 ;
    if (Sync && lasttime.Data != time.Data) {
        CMN_LOG_CLASS_RUN_DEBUG << "Seek " << time << std::endl;

        State = SEEK;
        PlayUntilTime = PlayerDataInfo.DataEnd();
        // this will cause state table write command overflow
        //BaseAccess.WriteTime(time);        
        Time = time;
        lasttime = time;
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
    mtsDouble playTime;
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
    static mtsDouble lasttime;
    if(lasttime.Data == t.Data)
        return; 
    else
        lasttime = t;

    if (Sync) {
        SeekRequest(t);       
    }         
    State = SEEK;      
    Time = t ;
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


    if (Time.Data < PlayerDataInfo.DataStart()) {
        Time = PlayerDataInfo.DataStart();
    }

    if (Time.Data > PlayerDataInfo.DataEnd()) {
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
    ZoomScaleValue = value;
    ScaleZoom->setValue(ZoomScaleValue);    
    UpdatePlot();
}


// read data from file
void cdpPlayerPlot2D::OpenFile(void)
{
   QString result;


    result = QFileDialog::getOpenFileName(mainWidget, "Open File", 0, 0);
    if (!result.isNull()) {
        size_t i = 0;

        // read Data from file
	    ExtractDataFromStateTableCSVFile(result);

        Parser.GetBoundary(TracePointer,TopBoundary,LowBoundary);
        TimeBoundary = TopBoundary;
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


bool cdpPlayerPlot2D::ExtractDataFromStateTableCSVFile(QString & path){

    const std::string TimeFieldName("SineData-timestamp");
    const std::string DataFieldName("SineData-data");
    std::string Path(path.toStdString());

    // open header file
    Parser.ParseHeader(Path);
    Parser.GenerateIndex();
    // we sould name the file Path - .desc + .idx
    Parser.WriteIndexToFile("Parser.idx");
    Parser.SetDataFieldForSearch(DataFieldName);
    Parser.SetTimeFieldForSearch(TimeFieldName);
    Parser.LoadDataFromFile(TracePointer, 0.0, ZoomScaleValue,  false);
    //Parser.TriggerLoadDataFromFile(TracePointer, 0.0, ZoomScaleValue,  false);

    Parser.GetBoundary(TracePointer,TopBoundary,LowBoundary);

    Parser.GetBeginEndTime(PlayerDataInfo.DataStart(), PlayerDataInfo.DataEnd());

//    Data = &DataPool1;
//    TimeStamps =&TimeStampsPool1;

    return true;
}

// reset player to initial state
//! TODO: make this function thread safe
void cdpPlayerPlot2D::ResetPlayer(void)
{    
    // set to maximun period we read
    //ZoomScaleValue = (PlayerDataInfo.DataStart() != 0) ? ((PlayerDataInfo.DataEnd() - PlayerDataInfo.DataStart()) / 2.0) : 1.0 ;    
    //if(TimeStamps->size() != 0)
    //    ZoomScaleValue = (TimeStamps->at(TimeStamps->size()-1) - TimeStamps->at(0))/2.0;   

    ScaleZoom->setValue(ZoomScaleValue);    
    BaseAccess.WriteTime(0.0);
    Plot2DAccess.WriteVectorIndex(0); 
}

