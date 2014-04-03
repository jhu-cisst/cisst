/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

#define MTS_DECLARE_COMPONENT(componentName, componentType) bool InitComponent(void);

#define MTS_STATE_TABLE_BEGIN
#define MTS_STATE_TABLE_DATA(dataType, dataName) dataType dataName;
#define MTS_STATE_TABLE_END

#define MTS_INTERFACE_PROVIDED_BEGIN(interfaceName) struct interfaceName {
#define MTS_INTERFACE_PROVIDED_END(interfaceName) } interfaceName;
#define MTS_COMMAND_VOID(method, commandNameWithDoubleQuotes)
#define MTS_COMMAND_WRITE(method, commandNameWithDoubleQuotes, argumentPrototype)
#define MTS_COMMAND_READ(A, B, C)
#define MTS_COMMAND_STATEREAD(dataName, commandNameWithDoubleQuotes)
#define MTS_COMMAND_QUALIFIEDREAD(A, B, C, D)
#define MTS_EVENT_VOID(functionName, eventNameWithDoubleQuotes) mtsFunctionVoid functionName;
#define MTS_EVENT_WRITE(functionName, eventNameWithDoubleQuotes, argumentPrototype) mtsFunctionWrite functionName; 

#define MTS_INTERFACE_REQUIRED_BEGIN(interfaceName) struct interfaceName {
#define MTS_INTERFACE_REQUIRED_END(interfaceName) } interfaceName;
#define MTS_FUNCTION_VOID(functionName, commandNameWithDoubleQuotes) mtsFunctionVoid functionName;
#define MTS_FUNCTION_WRITE(functionName, commandNameWithDoubleQuotes) mtsFunctionWrite functionName;
#define MTS_FUNCTION_READ(functionName, commandNameWithDoubleQuotes) mtsFunctionRead functionName;
#define MTS_FUNCTION_QUALIFIEDREAD(functionName, commandNameWithDoubleQuotes) mtsFunctionQualifiedRead functionName;
#define MTS_EVENT_HANDLER_WRITE(method, eventNameWithDoubleQuotes, argumentPrototype)
#define MTS_EVENT_HANDLER_VOID(method, eventNameWithDoubleQuotes)
#define MTS_EVENT_HANDLER_NONQUEUED_VOID(method, eventNameWithDoubleQuotes)

#endif // _mtsComponentGeneratorMacros_h
