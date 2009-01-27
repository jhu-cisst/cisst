/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: BoardIO.h,v 1.1 2008/09/29 21:44:42 tian Exp $

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

#ifndef _BOARDIO_H_
#define _BOARDIO_H_
#include <cisstCommon/cmnPortability.h>

#include <sys/io.h>
#include <unistd.h>

#include "Offsets.h"

/*
 * This class is the lowest level interface to the Low Power Motor Controller Board.
 * All methods in this class are d and issue one or more low level IO commands
 * Since it has outw/inw MACROS, -O or -O2 optimization must be turned on especially
 * under Linux/GCC.
 * This class is main intention to provide a clearner looking upper level code, and
 * also to deal with DOS vs Linux differences in low level codes
 */

#if (defined _BORLANDC_)
#define outl(data, address)	outpl(address, data)
#define inl(address)        inpl(address)
#define outw(data, address) outpw(address, data)
#define inw(address)        inpw(address)
#define outb(data, address) outpb(address, data)
#define inb(address)        inpb(address)
#endif

class BoardIO {
	private:
		unsigned int numTries;
		unsigned short BaseAddress;
	public:
	BoardIO (unsigned int baseAddress = 0x220) {BaseAddress = baseAddress;numTries = 0;}
	/* Set functions */
	 void SetMotorState (unsigned short mask, unsigned short state)
		{outw (IOCMD_SET_MOTOR_STATE | ((mask & 0x0F) << 4) | (state & 0x0F), BaseAddress + CMD_REGISTER);}
	 void SetAllIndices (bool autoIncrement, unsigned short maxIndex, unsigned short initIndex)
		{outw (IOCMD_SET_ALL_INDICES | (autoIncrement?0x0010:0x0000) | ((maxIndex & 0x03) << 2) | (initIndex & 0x03), BaseAddress + CMD_REGISTER);}
	 void SetMotorVoltageIndices (bool autoIncrement, unsigned short maxIndex, unsigned short initIndex)
		{outw (IOCMD_SET_MOTOR_VOLTAGE_INDICES | (autoIncrement?0x0010:0x0000) | ((maxIndex & 0x03) << 2) | (initIndex & 0x03), BaseAddress + CMD_REGISTER);}
	 void SetCurrentLimitIndices (bool autoIncrement, unsigned short maxIndex, unsigned short initIndex)
		{outw (IOCMD_SET_CURRENT_LIMIT_INDICES | (autoIncrement?0x0010:0x0000) | ((maxIndex & 0x03) << 2) | (initIndex & 0x03), BaseAddress + CMD_REGISTER);}
	 void SetEncoderIndices (bool autoIncrement, unsigned short maxIndex, unsigned short initIndex)
		{outw (IOCMD_SET_ENCODER_INDICES | (autoIncrement?0x0010:0x0000) | ((maxIndex & 0x03) << 2) | (initIndex & 0x03), BaseAddress + CMD_REGISTER);}
	 void SetCurrentFeedbackMaxIndex (unsigned short maxIndex)
		{outw (IOCMD_SET_CURRENT_FEEDBACK_MAX_INDEX | ((maxIndex & 0x03) << 2), BaseAddress + CMD_REGISTER);}
	 void SetPotFeedbackMaxIndex (unsigned short maxIndex)
		{outw (IOCMD_SET_POT_FEEDBACK_MAX_INDEX | ((maxIndex & 0x03) << 2), BaseAddress + CMD_REGISTER);}
	 void SetADInterruptNumber (unsigned short interruptNumber)
		{outw (IOCMD_SET_AD_INTERRUPT_NUMBER | (interruptNumber & 0x07), BaseAddress + CMD_REGISTER);}
	 void SetTimerInterruptNumber (unsigned short interruptNumber)
		{outw (IOCMD_SET_TIMER_INTERRUPT_NUMBER | (interruptNumber & 0x07), BaseAddress + CMD_REGISTER);}
	 void SetADInterrupt (bool enable)
		{outw (IOCMD_SET_AD_INTERRUPT | (enable?0x0001:0x0000), BaseAddress + CMD_REGISTER);}
	 void SetTimerInterrupt (bool enable)
		{outw (IOCMD_SET_TIMER_INTERRUPT | (enable?0x0001:0x0000), BaseAddress + CMD_REGISTER);}
	 void SetWDTPeriod (unsigned short period)
		{outw (IOCMD_SET_WDT_PERIOD | (period & 0x03FF), BaseAddress + CMD_REGISTER);}
	 void SetTimerPeriod (unsigned short period)
		{outw (IOCMD_SET_TIMER_PERIOD | (period & 0x03FF), BaseAddress + CMD_REGISTER);}

	/* Get functions */
	 unsigned short GetMotorState ()
		{outw (IOCMD_GET_MOTOR_STATE, BaseAddress + CMD_REGISTER); return (inw (BaseAddress + DATA_REGISTER) & 0x000F);}
	 unsigned short GetMotorVoltageIndices ()
		{outw (IOCMD_GET_MOTOR_VOLTAGE_INDICES, BaseAddress + CMD_REGISTER); return (inw (BaseAddress + DATA_REGISTER) & 0x001F);}
	 unsigned short GetCurrentLimitIndices ()
		{outw (IOCMD_GET_CURRENT_LIMIT_INDICES, BaseAddress + CMD_REGISTER); return (inw (BaseAddress + DATA_REGISTER) & 0x001F);}
	 unsigned short GetEncoderIndices ()
		{outw (IOCMD_GET_ENCODER_INDICES, BaseAddress + CMD_REGISTER); return (inw (BaseAddress + DATA_REGISTER) & 0x001F);}
	 unsigned short GetCurrentFeedbackMaxIndex ()
		{outw (IOCMD_GET_CURRENT_FEEDBACK_MAX_INDEX, BaseAddress + CMD_REGISTER); return (inw (BaseAddress + DATA_REGISTER) & 0x000C);}
	 unsigned short GetPotFeedbackMaxIndex ()
		{outw (IOCMD_GET_POT_FEEDBACK_MAX_INDEX, BaseAddress + CMD_REGISTER); return (inw (BaseAddress + DATA_REGISTER) & 0x000C);}
	 unsigned short GetADInterruptNumber ()
		{outw (IOCMD_GET_AD_INTERRUPT_NUMBER, BaseAddress + CMD_REGISTER); return (inw (BaseAddress + DATA_REGISTER) & 0x0007);}
	 unsigned short GetTimerInterruptNumber ()
		{outw (IOCMD_GET_TIMER_INTERRUPT_NUMBER, BaseAddress + CMD_REGISTER); return (inw (BaseAddress + DATA_REGISTER) & 0x0007);}
	 bool GetADInterrupt ()
		{outw (IOCMD_GET_AD_INTERRUPT, BaseAddress + CMD_REGISTER); return (((inw (BaseAddress + DATA_REGISTER) & 0x0001)?true:false));}
	 bool GetTimerInterrupt ()
		{outw (IOCMD_GET_TIMER_INTERRUPT, BaseAddress + CMD_REGISTER); return (((inw (BaseAddress + DATA_REGISTER) & 0x0001))?true:false);}
	 unsigned short GetWDTPeriod ()
		{outw (IOCMD_GET_WDT_PERIOD, BaseAddress + CMD_REGISTER); return (inw (BaseAddress + DATA_REGISTER) & 0x03FF);}
	 unsigned short GetTimerPeriod ()
		{outw (IOCMD_GET_TIMER_PERIOD, BaseAddress + CMD_REGISTER); return (inw (BaseAddress + DATA_REGISTER) & 0x03FF);}
	 unsigned short GetCurrentTime ()
		{outw (IOCMD_GET_CURRENT_TIME, BaseAddress + CMD_REGISTER); return (inw (BaseAddress + DATA_REGISTER) & 0x03FF);}

	/* Pulse Functions */

	/* DAC Functions */
	 void SetMotorVoltage (short data) {
		/*
		 * Make note of the un-expected behaviour here
		 * the positive range is upto 2^n and NOT 2^n-1
		 * whereas the negative range is upto 2^n-1.
		 * 4096 == +5V 
		 * -4095 == -5V
		 */
		//shift for DAC
		data += 0x1000;
		data = (data > 0x1FFF)?0x1FFF:data;
		data = (data < 0)?0:data;
		outw (data, BaseAddress + DA_MOTOR_VOLTAGE_REGISTER);
		}
	 void SetCurrentLimit (short data) {
		/*
		 * The second dac is set to be unipolar
		 * 0 == 0V
		 * 8192 = +5V
		 */
		data = (data > 0x1FFF)?0x1FFF:data;
		data = (data < 0)?0:data;
		outw (data, BaseAddress + DA_CURRENT_LIMIT_REGISTER);
		}
	 void LoadMotorVoltages ()
		{outw (IOCMD_LOAD_MOTOR_VOLTAGES, BaseAddress + CMD_REGISTER);}
	 void LoadCurrentLimits ()
		{outw (IOCMD_LOAD_CURRENT_LIMITS, BaseAddress + CMD_REGISTER);}
	 void LoadMotorVoltagesCurrentLimits ()
		{outw (IOCMD_LOAD_MOTOR_VOLTAGES_CURRENT_LIM, BaseAddress + CMD_REGISTER);}

	/* ADC Functions */
	 short GetADFIFO () {
		/*
		 * 5V == 8191 
		 * -5V == -8192
		 */
		short data = inw (BaseAddress + AD_FIFO_REGISTER) & 0x3FFF;
		/*
		 * extend the sign bit to 16bits of data.
		 * upper 3 bits are ones for -ve numbers
		 * upper 3 bits are zeros for +ve numbers
		 */
		data = (data & 0x2000)?(data | 0xE000):(data & 0x1FFF);
		return data;
	}

	/*
	 * A read of the FIFO puts the ADC in the reset (aka Interrupt Deactivated)
	 * state. This is somewhat un-expected behaviour, I was expecting that a
	 * #CONVST pulse would also De-activate the #INT pin. Keeping this in mind it
	 * is a good idea to read the FIFO before closing application, irrespective
	 * of the interrupt state to enusre ADC is in valid state. Also it might be a
	 * good idea to do a read just after progamming initializtion to make sure
	 * ADC is in valid state again irrespective of the interrupt state.
	 *
	 * The cleanest solution is to fix the MAXIM bug in hardware, I leave this
	 * for the future. A quicker more cleaner solution is to have the #INT as part
	 * of the status register, check this bit before each conversion, and reset
	 * the ADC if the interrupt is activated. For now, this has been implemented.
	 *
	 * Ankur
	 */

	 unsigned int StartConvCurrentFeedback () {
		bool devReady = (inw (BaseAddress + STATUS_REGISTER) & 0x0008)?true:false;
		if (devReady == true) {
			inw (BaseAddress + AD_FIFO_REGISTER);
			numTries++;
		}
		bool devInt = (inw (BaseAddress + STATUS_REGISTER) & 0x0002)?true:false;
		if (devInt == true) {
			outw (IOCMD_CLEAR_AD_INTERRUPT, BaseAddress + CMD_REGISTER);
		}
		outw (IOCMD_START_CONV_CURRENT_FEEDBACK, BaseAddress + CMD_REGISTER);
		return numTries;
	}
	 unsigned int StartConvPotFeedback () {
		bool devReady = (inw (BaseAddress + STATUS_REGISTER) & 0x0008)?true:false;
		if (devReady == true) {
			inw (BaseAddress + AD_FIFO_REGISTER);
			numTries++;
		}
		bool devInt = (inw (BaseAddress + STATUS_REGISTER) & 0x0002)?true:false;
		if (devInt == true) {
			outw (IOCMD_CLEAR_AD_INTERRUPT, BaseAddress + CMD_REGISTER);
		}
		outw (IOCMD_START_CONV_POT_FEEDBACK, BaseAddress + CMD_REGISTER);
		return numTries;
	}
	 bool ADInterruptPending ()
		{return ((inw (BaseAddress + STATUS_REGISTER) & 0x0002)?true:false);}
	 bool ADInterruptFlag ()
		{return  ((inw (BaseAddress + STATUS_REGISTER) & 0x0008)?true:false);}
	 void ClearADInterrupt ()
		{outw (IOCMD_CLEAR_AD_INTERRUPT, BaseAddress + CMD_REGISTER);}
	 bool PollADInterruptPending (unsigned int timeout) {
		bool devInt = false;
		unsigned int timeCount = 0;
			while (1) {
				devInt = (inw (BaseAddress + STATUS_REGISTER) & 0x0002)?true:false;
				if (devInt) return true;
				if (timeCount > timeout) return false;
				usleep (1);
				timeCount++;
			}
	}

	/* Encoder Functions */
	 void LatchEncoders () {
		outw (IOCMD_LATCH_ENCODERS, BaseAddress + CMD_REGISTER);
	}
	 void PreLoadEncoders (unsigned short listEncoder[]) {
		unsigned short mask = ((listEncoder[3] & 0x1) << 3) | ((listEncoder[2] & 0x1) << 2) |
			((listEncoder[1] & 0x1) << 1) | (listEncoder[0] & 0x1);
		outw (IOCMD_PRELOAD_ENCODERS | mask, BaseAddress + CMD_REGISTER);
	}
	 void SetEncoderPreloadRegister (unsigned int data) {
		//outl (data, BaseAddress + ENCODER_LOW_REGISTER);
         outw_p (data & 0xffff, BaseAddress + ENCODER_LOW_REGISTER);
         outw_p ((data & 0xff0000) >> 16, BaseAddress + ENCODER_HIGH_REGISTER);
	}
	 unsigned int GetEncoder () {
		//return (inl (BaseAddress + ENCODER_LOW_REGISTER) & 0x00FFFFFF);
         unsigned int lowbyte, highbyte;
         lowbyte = inw_p (BaseAddress + ENCODER_LOW_REGISTER);
         highbyte = inw_p (BaseAddress + ENCODER_HIGH_REGISTER);
         return ((highbyte << 16 | lowbyte) & 0x00FFFFFF);
	}
	 short GetEncoderPeriod () {
		unsigned short ret;
		ret = inw (BaseAddress + ENCODER_PERIOD) & 0x7FFF;
		// sign extend to 2 bytes
		ret = (ret&(0x01 <<13))?ret|0xC000:ret;
		return ret;
	}
	 short GetEncoderFrequency () {
		unsigned short ret;
		ret = inw (BaseAddress + ENCODER_FREQUENCY) & 0xFFFF;
		// sign extend to 2 bytes
		ret = (ret&(0x01 <<14))?ret|0xC000:ret;
		return ret;
	}

	/* Timer Functions */
	 bool TimerInterruptPending ()
		{return ((inw (BaseAddress + STATUS_REGISTER) & 0x0001)?true:false);}
	 void ClearTimerInterrupt ()
		{outw (IOCMD_CLEAR_TIMER_INTERRUPT, BaseAddress + CMD_REGISTER);}

	/* WDT Functions */
	 bool WDTStatus ()
		{return ((inw (BaseAddress + STATUS_REGISTER) & 0x0004)?true:false);}

	/* Misc. Functions */
	 unsigned short BoardVersion ()
		{return (inw (BaseAddress));}
	 void SetBaseAddress (unsigned short address)
		{BaseAddress = address;}
	 unsigned short GetBaseAddress ()
		{return BaseAddress;}
	 void EnableMotor (unsigned short motor)
		{SetMotorState (0x01<<motor, 0x01<<motor);}
	 void DisableMotor (unsigned short motor)
		{SetMotorState (0x01<<motor, ~(0x01<<motor));}
	 void EnableAllMotors ()
		{SetMotorState (0x0F, 0x0F);}
	 void DisableAllMotors ()
		{SetMotorState (0x0F, 0x00);}
	/* ***provided for testing only*** */
	 void Write (unsigned short data, unsigned short offset)
		{outw (data, BaseAddress + offset);}
	/* ***provided for testing only*** */
	 unsigned short Read (unsigned short offset)
		{return (inw (BaseAddress + offset));}
	/* ***provided for testing only*** */
	 void SetSimEncoder (bool enable, unsigned short dir, unsigned short divisor)
		{outw (((enable)?0x01:0x00)<<5|(dir & 0x01)<<4|divisor & 0x0F, BaseAddress + UNUSED_0);}

    // format is home, neg limit, pos limit
    inline short GetDigitalInput(void) {
        short data = inw (BaseAddress + DIGITAL_IO);
        return (~data);
    }

    inline void SetDigitalOutput(short data) {
        outw ((~data) & 0x000F, BaseAddress + DIGITAL_IO);
    }
};

#endif
