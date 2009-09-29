/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: Offsets.h,v 1.1 2008/09/29 21:44:42 tian Exp $

  Author(s):  Ankur Kapoor
  Created on: 2004

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _devLoPoMoCoOffsets_h
#define _devLoPoMoCoOffsets_h
/*
 * Register addresses
 */
#define VERSION_REGISTER			0
#define CMD_REGISTER				2
#define STATUS_REGISTER				2
#define DATA_REGISTER				4
#define AD_FIFO_REGISTER			6
#define DA_MOTOR_VOLTAGE_REGISTER		8
#define DA_CURRENT_LIMIT_REGISTER		10
#define ENCODER_LOW_REGISTER			12
#define ENCODER_HIGH_REGISTER			14
#define ENCODER_PERIOD				16
#define ENCODER_FREQUENCY			18
#define ENCODER_INDEX			20
#define DIGITAL_IO			22
#define UNUSED_0				20

/*
 * Functions 
 */
/* Set functions */
#define IOCMD_SET_MOTOR_STATE			0x0000
#define IOCMD_SET_ALL_INDICES			0x0400
#define IOCMD_SET_MOTOR_VOLTAGE_INDICES		0x0800
#define IOCMD_SET_CURRENT_LIMIT_INDICES		0x0C00
#define IOCMD_SET_ENCODER_INDICES		0x1000
#define IOCMD_SET_CURRENT_FEEDBACK_MAX_INDEX	0x1400
#define IOCMD_SET_POT_FEEDBACK_MAX_INDEX	0x1800
#define IOCMD_SET_AD_INTERRUPT_NUMBER		0x1C00
#define IOCMD_SET_TIMER_INTERRUPT_NUMBER	0x2000
#define IOCMD_SET_AD_INTERRUPT			0x2400
#define IOCMD_SET_TIMER_INTERRUPT		0x2800
#define IOCMD_SET_WDT_PERIOD			0x2C00
#define IOCMD_SET_TIMER_PERIOD			0x3000
//#define Unused				0x3400
//#define Unused				0x3800
//#define Unused				0x3C00

/* Get functions */
#define IOCMD_GET_MOTOR_STATE			0x4000
//#define Unused				0x4400
#define IOCMD_GET_MOTOR_VOLTAGE_INDICES		0x4800
#define IOCMD_GET_CURRENT_LIMIT_INDICES		0x4C00
#define IOCMD_GET_ENCODER_INDICES		0x5000
#define IOCMD_GET_CURRENT_FEEDBACK_MAX_INDEX	0x5400
#define IOCMD_GET_POT_FEEDBACK_MAX_INDEX	0x5800
#define IOCMD_GET_AD_INTERRUPT_NUMBER		0x5C00
#define IOCMD_GET_TIMER_INTERRUPT_NUMBER	0x6000
#define IOCMD_GET_AD_INTERRUPT			0x6400
#define IOCMD_GET_TIMER_INTERRUPT		0x6800
#define IOCMD_GET_WDT_PERIOD			0x6C00
#define IOCMD_GET_TIMER_PERIOD			0x7000
#define IOCMD_GET_CURRENT_TIME			0x7400
//#define Unused				0x7800
//#define Unused				0x7C00

/* Pulse functions */
#define IOCMD_LOAD_MOTOR_VOLTAGES		0x8000
#define IOCMD_LOAD_CURRENT_LIMITS		0x8400
#define IOCMD_LOAD_MOTOR_VOLTAGES_CURRENT_LIM	0x8800
#define IOCMD_LATCH_ENCODERS			0x8C00
#define IOCMD_PRELOAD_ENCODERS			0x9000
#define IOCMD_START_CONV_CURRENT_FEEDBACK	0x9400
#define IOCMD_START_CONV_POT_FEEDBACK		0x9800
#define IOCMD_CLEAR_AD_INTERRUPT		0x9C00
#define IOCMD_CLEAR_TIMER_INTERRUPT		0xA000
#define IOCMD_RESET_CURRENT_FEEDBACK		0xA400
#define IOCMD_RESET_POT_FEEDBACK		0xA800
//#define Unused				0xAC00
//#define Unused				0xB000
//#define Unused				0xB400
//#define Unused				0xB800
//#define Unused				0xBC00

#endif // _devLoPoMoCoOffsets_h
