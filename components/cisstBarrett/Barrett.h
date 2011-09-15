/*

  Author(s): Simon Leonard
  Created on: Dec 02 2009

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _Barrett_h
#define _Barrett_h

#include <stdint.h>

struct Barrett{

  enum Command{ GET, SET };

  //! Define the value of a property (4-6 bytes)
  typedef int64_t Value;

  static const unsigned char GET_CODE = 0x00;
  static const unsigned char SET_CODE = 0x80;

  //! The number of properties available
  static const size_t NUM_PROPERTIES = 128;

  enum ID { 
    VERSION           =   0,
    ROLE              =   1,
    SERIALNUMBER      =   2,
    CANBUSID          =   3,
    ERROR             =   4,
    STATUS            =   5,
    ADDRESS           =   6,
    VALUE             =   7,
    MODE              =   8,
    TEMPERATURE       =   9,
    PEAKTEMPERATURE   =  10,
    OVERHEAT          =  11,
    BAUDRATE          =  12,
    LOCK              =  13,
    DIGITALIO0        =  14,
    DIGITALIO1        =  15,
    TENSIONER         =  16,
    BRAKE             =  17,
    ANALOGIN0         =  18,
    ANALOGIN1         =  19,
    THERMISTOR        =  20,
    VBUS              =  21,
    IMOTOR            =  22,
    VLOGIC            =  23,
    ILOGIC            =  24,
    STRAINGAGE        =  25,
    GROUPA            =  26,
    GROUPB            =  27,
    GROUPC            =  28,
    COMMAND           =  29,
    SAVE              =  30,
    LOAD              =  31,
    DEFAULTCOMMAND    =  32,
    FINDCOMMAND       =  33,
    X0                =  34,
    X1                =  35,
    X2                =  36,
    X3                =  37,
    X4                =  38,
    X5                =  39,
    X6                =  40,
    X7                =  41,
    
    // Motor
    TRQ               =  42,
    MAXTRQ            =  43,
    VEL               =  44,
    MAXVEL            =  45,
    MCV               =  46,
    MOV               =  47,
    POS               =  48,
    P2                =  49,
    DEFAULTPOS        =  50,
    DP2               =  51,
    ENDPOINT          =  52,
    E2                =  53,
    OPENTARGET        =  54,
    OT2               =  55,
    CLOSETARGET       =  56,
    CT2               =  57,
    MECHANGLE_ENC     =  58,
    M2                =  59,
    DS                =  60,
    MECHOFFSET        =  61,
    CURRENTOFFSET     =  62,
    UPTIME            =  63,
    OD                =  64,
    MAXDUTYSUM        =  65,
    MECHANGLE         =  66,
    MECH2             =  67,
    COUNTSPERREV      =  68,
    CTS2              =  69,
    PUCKINDEX         =  70,
    HSG               =  71,
    LSG               =  72,
    IVEL              =  73,
    INITOFFSET        =  74,
    IOFF2             =  75,
    MPE               =  76,
    EN                =  77,
    TIME2STOP         =  78,
    KP                =  79,
    KD                =  80,
    KI                =  81,
    ACCEL             =  82,
    TENST             =  83,
    TENSO             =  84,
    JIDX              =  85,
    IPNM              =  86,
    HALLS             =  87,
    HALLH             =  88,
    HALLH2            =  89,
    POLES             =  90,
    IKP               =  91,
    IKI               =  92,
    IKCOR             =  93,
    HOLD              =  94,
    TIE               =  95,
    ECMAX             =  96,
    ECMIN             =  97,
    LFLAGS            =  98,
    LCTC              =  99,
    LCVC              = 100,
    LASTPROP          = 101,
    
    // Safety Module
    ZERO              = 42,
    PEN               = 43,
    SAFE              = 44,
    VELWARNING        = 45,
    VELFAULT          = 46,
    TRQWARNING        = 47,
    TRQFAULT          = 48,
    VOLTWARNINGL      = 49,
    VOLTFAULTL        = 50,
    VOLTWARNINGH      = 51,
    VOLTFAULTH        = 52,
    POWER             = 53,
    MAXPOWER          = 54,
    IGNOREFAULT       = 55,
    VNOM              = 56,
  };

};


#endif
