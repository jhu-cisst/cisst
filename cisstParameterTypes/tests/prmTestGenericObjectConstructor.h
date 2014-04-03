/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2009-04-29
  
  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _prmTestGenericObjectConstructor_h
#define _prmTestGenericObjectConstructor_h

#include <cisstMultiTask/mtsGenericObject.h>

#include <sstream>

// utility function to test the data member inherited from mtsGenericObject
template <class _elementType>
void prmTestGenericObjectConstructor(const _elementType & initial)
{
    // check data inherited from mtsGenericObject
    CPPUNIT_ASSERT(initial.Timestamp() == 0.0);
    CPPUNIT_ASSERT(initial.AutomaticTimestamp() == true);
    CPPUNIT_ASSERT(initial.Valid() == false);
}


// utility to sawp the value of mtsGenericObject data members 
template <class _elementType>
void prmTestGenericObjectConstructorSwapValues(_elementType & initial)
{
    initial.Timestamp() += 3.1435;
    initial.AutomaticTimestamp() = !initial.AutomaticTimestamp();
    initial.Valid() = !initial.Valid();
}



// utility function to test the data member inherited from mtsGenericObject
template <class _elementType>
void prmTestGenericObjectCopyConstructor(const _elementType & initial, const _elementType & final)
{
    // check data inherited from mtsGenericObject
    CPPUNIT_ASSERT(initial.Timestamp() == final.Timestamp());
    CPPUNIT_ASSERT(initial.AutomaticTimestamp() == final.AutomaticTimestamp());
    CPPUNIT_ASSERT(initial.Valid() == final.Valid());
}


#endif // _prmTestGenericObjectConstructor_h
