/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 Author(s):  Ankur Kapoor, Tian Xia
 Created on: 2004-04-30

 (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
 Reserved.

 --- begin cisst license - do not edit ---

 This software is provided "as is" under an open source license, with
 no warranty.  The complete license can be found in license.txt and
 http://www.cisst.org/cisst/license.txt.

 --- end cisst license ---
 */

/*!
 \file
 \brief Defines the JHU LoPoMoCo device, the Low Power Motion Controller
 \ingroup cisstDevices
 */

#ifndef _devLoPoMoCo_h
#define _devLoPoMoCo_h

#include <cisstDevices/devConfig.h>


#include <cisstCommon/cmnGenericObject.h>
#include <cisstMultiTask/mtsDevice.h>
#include <cisstMultiTask/mtsDeviceInterface.h>
#include <cisstMultiTask/mtsVector.h>
#include <cisstMultiTask/mtsTask.h>

#include <vector>
#include <string>
#include <ostream>

// forward declarations
class devLoPoMoCoBoardIO;

/*!
 \ingroup mtsDeviceInterface
 A specific device class for the LoPoMoCo motion controller card.
 */
class devLoPoMoCo: public mtsDevice {

	CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);

	/*! A pointer to the low level object that converts make bit level
	 operation more readable. */
	std::vector<devLoPoMoCoBoardIO *> Board;

	/*! Absolute starting number of axis on the boards. */
	std::vector<int> StartAxis;

	/*! Absolute ending number of axis on on the boards. */
	std::vector<int> EndAxis;

	/*! Max number of axis on the board. */
	std::vector<unsigned int> MaxAxis;

	/*! Base addresses of all boards. */
	std::vector<int> BaseAddress;

	/*! Total Number of Axes used */
	int numberOfAxes;
	int numberOfBoards;

	//std::string RelativePathToConfigFiles;

	/*! Need to save motor voltages so that correct correction could
	 be applied to the current limit. */
	mtsShortVec MotorVoltages;

	/*! Need to read encoder periods along with each encoder read. */
	mtsShortVec EncoderPeriods;

	/*! Need to read encoder frequencies along with each encoder read. */
	mtsShortVec EncoderFrequencies;

	/*! Frequency To RPS Ratio */
	mtsDoubleVec FrequencyToRPSRatio;

	/*! CountsToDeg */
	// anton mtsDoubleVec CountsToDeg;

	/*! CountsToMotorCurrents */
	mtsDoubleVec CountsToMotorCurrents;

	/*! CountsToPotFeedback */
	mtsDoubleVec CountsToPotFeedback;

	/*! MotorSpeedToCounts */
	mtsDoubleVec MotorSpeedToCounts;

	/*! PositiveSlope */
	mtsDoubleVec PositiveSlope;

	/*! PositiveIntercept */
	mtsDoubleVec PositiveIntercept;

	/*! NegativeSlope */
	mtsDoubleVec NegativeSlope;

	/*! NegativeIntercept */
	mtsDoubleVec NegativeIntercept;

	/*! VoltageToCounts */
	mtsDoubleVec VoltageToCounts;

	// TODO: Tian: ankur mixes unsigned short and int, is this a problem?
	/*!
	 * Map a composite index to the index {0 ~ 3} of a board
	 * Returns this index, range between 0 ~ 3,
	 * Return -1, if mapping does not exist
	 */
	inline int MapAxisToBoard(unsigned short axisIndex) {
		return axisIndex/4;
	}

	void ConfigureOneBoard(const std::string & filename, const int boardIndex);
	void parseInputArgument(const std::string &inputArgument, std::string &relativeFilePath, std::string &fileName);

public:

    /*! Number of axis per board */
    enum {NB_AXIS = 4};
 
	/*! Constructor. Prepares the maps. */
	devLoPoMoCo(const std::string& deviceName, unsigned int numberOfBoards);

	/*! Default destructor. Disables motor power. */
	virtual ~devLoPoMoCo();

	/*!
	 * Configure takes the top level configuration file which points to the configuration file
	 * for each board.
	 */
	virtual void Configure(const std::string &filename);

protected:
	/*******************************************
	 * Start of the Read Commands, no parameters
	 *******************************************/
	/*! Latch the encoder positions.  It is indexed by the string
	 LatchEncoders in the PreReadMap. */
	//mtsCommandBase::ReturnType LatchEncoders(void);
	void LatchEncoders(void);

	/*! Start conversion on ADC connected to current limits.  It is
	 indexed by the string StartMotorCurrentConv in the
	 PreReadMap. */
	//mtsCommandBase::ReturnType StartMotorCurrentConv(void);
	void StartMotorCurrentConv(void);

	/*! Start conversion on ADC connected to pots.  It is indexed by
	 the string StartPotFeedbackConv in the PreReadMap. */
	//mtsCommandBase::ReturnType StartPotFeedbackConv(void);
	void StartPotFeedbackConv(void);

	/*! Enable all axis on this board
	 */
    void EnableAll(void);
	/*! Disable all axes on this board
	 */
    void DisableAll(void);

	/*******************************
	 * Start of the Read Commands
	 *******************************/
	/*! Get the latest encoder positions.  Assumes that encoders are
	 latched using the void operation LatchEncoders.  It is indexed
	 by the string Positions in the ReadMap. */
	// type is Long
	void GetPositions(mtsLongVec & Positions) const;

	/*! Get the velocity.  Assumes that encoders are latched using the
	 void operation LatchEncoders and that Encoders are read using
	 GetPosition.  It is indexed by the string Velocities in the
	 ReadMap. */
	// type is Short
	void GetVelocities(mtsShortVec & Velocities) const;

	/*! Get the latest motor currents.  Assumes that a conversion has
	 been started using the void operation StartMotorCurrentConv.
	 It is indexed by the string MotorCurrents in the ReadMap. */
	// type is short
	void GetMotorCurrents(mtsShortVec & MotorCurrents) const;

	/*! Get the latest pot position.  Assumes that a conversion has
	 been started using the void operation StartPotFeedbackConv.
	 It is indexed by the string PotFeedbacks in the ReadMap. */
	// type is short
	void GetPotFeedbacks(mtsShortVec & PotFeedbacks) const;

	/*******************************
	 * Start of the Write Commands
	 *******************************/
	/*! Write motor votlages to the DAC buffer.  Assumes that these
	 would be loaded using the void operation LoadMotorVoltages
	 or LoadMotorVoltagesCurrentLimits.  It is indexed by the string
	 MotorVoltages in the WriteMap. */
	// type is short
	void SetMotorVoltages(const mtsShortVec & MotorVoltages);

	/*! Write current limits to the DAC buffer.  Assumes that these
	 would be loaded using the void operation LoadCurrentLimits
	 or LoadMotorVoltagesCurrentLimits.  It is indexed by the string
	 CurrentLimits in the WriteMap. */
	// type is short
	void SetCurrentLimits(const mtsShortVec & CurrentLimits);

	/*! Write the current positions to the encoder buffer.
	 It is indexed by the string SetPositions in the
	 WriteMap. */
	// type is Long (raw positions or encoder counts)
	void SetPositions(const mtsLongVec & Positions);

	/*! Load the motor voltage DAC buffer to actual analog output.  It
	 is indexed by the string LoadMotorVoltages in the VoidMap. */
	// type is void
	void LoadMotorVoltages(void);

	/*! Load the current limits DAC buffer to actual analog output.
	 It is indexed by the string LoadCurrentLimits in the
	 VoidMap. */
	// type is void
	void LoadCurrentLimits(void);

	/*! Load the motor voltages and current limits DAC buffer to
	 actual analog output.  It is indexed by the string
	 LoadMotorVoltagesCurrentLimits in the VoidMap. */
	void LoadMotorVoltagesCurrentLimits(void);

	/*! Convert encoder frequencies to revolutions per second.  It is
	 indexed by the string FrequencyToRPS in the QualifiedReadMap. */
	// type is shor to double
	void FrequencyToRPS(const mtsShortVec & fromData, mtsDoubleVec & toData) const;

	/*! Convert motor currents in ADC counts to mA.  It is indexed by
	 the string ADCToMotorCurrents in the QualifiedReadMap. */
	// type is short to double
	void ADCToMotorCurrents(const mtsShortVec & fromData, mtsDoubleVec & toData) const;

	/*! Convert pot feedback in ADC counts to V.  It is indexed by the
	 string ADCToPotFeedbacks in the QualifiedReadMap. */
	// type is short to double
	void ADCToPotFeedbacks(const mtsShortVec & fromData, mtsVector <double> & toData) const;

	/*! Convert motor voltages in V to DAC counts.  It is indexed by
	 the string MotorVoltagesToDAC in the QualifiedReadMap. */
	// type is DOUBLE TO SHORT
	void MotorVoltagesToDAC(const mtsDoubleVec & fromData, mtsShortVec & toData) const;

	/*! Convert currents limits in mA to DAC counts.  It is indexed by
	 the string CurrentLimitsToDAC in the QualifiedReadMap. */
	// type is double to short
	void CurrentLimitsToDAC(const mtsDoubleVec & fromData, mtsShortVec & toData) const;

	/*! Enable the specified axis
	 */
	void Enable(const cmnShort & axisIndex);

	/*! Disable the specified axis
	 */
	void Disable(const cmnShort & axisIndex);

	/*! Reset Encoder
	 */
	void ResetEncoders(const cmnShort & axisIndex);

	/* set digital output */
	void SetDigitalOutput(const mtsIntVec & DigitalOutput);

	/* get digital input */
	void GetDigitalInput(mtsIntVec & DigitalInput) const;

    /* get latched index:
        might only be available in Version 0xCCDD FPGA (MR-Robot) */
    void GetLatchedIndex(mtsShortVec & latchedIndex) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(devLoPoMoCo);

#endif // _devLoPoMoCo_h
