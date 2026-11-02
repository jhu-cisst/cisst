%module samplePython

#ifndef TEST_SWIG_TARGET_OPTIONS
#error Target SWIG compile options were not applied
#endif

%{
#include "answer.h"
%}

%include "answer.h"
