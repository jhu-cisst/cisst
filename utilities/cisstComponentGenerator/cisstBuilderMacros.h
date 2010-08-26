/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: cisstBuilderMacros.h 1030 2010-06-09 jkriss1 $ */

#ifndef _cisstBuilderMacros_h
#define _cisstBuilderMacros_h

#define MTS_DECLARE_COMPONENT(A,B) void A::InitComponent(void);
#define MTS_STATE_TABLE_BEGIN
#define MTS_STATE_TABLE_DATA(A,B) A B;
#define MTS_STATE_TABLE_END
#define MTS_INTERFACE_PROVIDED_BEGIN(A)
#define MTS_INTERFACE_PROVIDED_END(A)
#define MTS_COMMAND_VOID(A,B)
#define MTS_COMMAND_WRITE(A,B,C)
#define MTS_COMMAND_READ(A,B,C)
#define MTS_COMMAND_STATEREAD(A,B)
#define MTS_COMMAND_QUALIFIEDREAD(A,B,C,D)
#define MTS_EVENT_VOID(A,B)
#define MTS_EVENT_WRITE(A,B,C)
#define MTS_INTERFACE_REQUIRED_BEGIN(A)
#define MTS_FUNCTION_VOID(A,B) mtsFunctionVoid A;
#define MTS_FUNCTION_WRITE(A,B) mtsFunctionWrite A;
#define MTS_FUNCTION_READ(A,B) mtsFunctionRead A;
#define MTS_FUNCTION_QUALIFIEDREAD(A,B) mtsFunctionQualifiedRead A;
#define MTS_INTERFACE_REQUIRED_END(A)

#endif