/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s):  Jonathan Kriss
  Created on: 2010-08-25

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsComponentGeneratorMacros_h
#define _mtsComponentGeneratorMacros_h

#define MTS_DECLARE_COMPONENT(A, B) void InitComponent(void);
#define MTS_STATE_TABLE_BEGIN
#define MTS_STATE_TABLE_DATA(A, B) A B;
#define MTS_STATE_TABLE_END
#define MTS_INTERFACE_PROVIDED_BEGIN(A)
#define MTS_INTERFACE_PROVIDED_END(A)
#define MTS_COMMAND_VOID(A, B)
#define MTS_COMMAND_WRITE(A, B, C)
#define MTS_COMMAND_READ(A, B, C)
#define MTS_COMMAND_STATEREAD(A, B)
#define MTS_COMMAND_QUALIFIEDREAD(A, B, C, D)
#define MTS_EVENT_VOID(A, B)
#define MTS_EVENT_WRITE(A, B, C)
#define MTS_INTERFACE_REQUIRED_BEGIN(A) struct A {
#define MTS_FUNCTION_VOID(A, B) mtsFunctionVoid B;
#define MTS_FUNCTION_WRITE(A, B) mtsFunctionWrite B;
#define MTS_FUNCTION_READ(A, B) mtsFunctionRead B;
#define MTS_FUNCTION_QUALIFIEDREAD(A, B) mtsFunctionQualifiedRead B;
#define MTS_EVENT_HANDLER_WRITE(A, B, C)
#define MTS_EVENT_HANDLER_VOID(A, B, C)
#define MTS_INTERFACE_REQUIRED_END(A) } A;

#endif // _mtsComponentGeneratorMacros_h
