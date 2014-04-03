/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2007-04-08
  
  (C) Copyright 2007-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


// include what is needed from cisst
#include <cisstCommon/cmnDeSerializer.h>
#include <cisstCommon/cmnGenericObjectProxy.h>
#include "classes.h"

// system includes
#include <iostream>


// main function
int main() {

    // add std::cout to see human readable log
    cmnLogger::AddChannel(std::cout);

    // create an input stream in binary mode and read the content as
    // defined in write.cpp
    std::ifstream inputStream("out.dat", std::ifstream::binary);
    if (!inputStream.good()) {
        CMN_LOG_INIT_ERROR << "Can't open \"out.dat\", please run the serialization \"write\" program first."
                           << std::endl;
        return -1;
    }
    cmnDeSerializer deSerialization(inputStream);

    // get object A1 using its base class
    cmnGenericObject * objectA1base = 0;
    objectA1base = deSerialization.DeSerialize();
    std::cout << objectA1base->Services()->GetName() << std::endl;

    // use dynamic cast to manipulate the object, this assumes that
    // the reader knows what to expect
    classA * objectA2pointer = 0;
    objectA2pointer = dynamic_cast<classA *>(deSerialization.DeSerialize());
    std::cout << objectA2pointer->Value << std::endl;

    // use an existing object of the correct type, this assumes that
    // the reader knows what to expect
    classA objectA3copy;
    deSerialization.DeSerialize(objectA3copy);
    std::cout << objectA3copy.Value << std::endl;

    // deserialize the "string" objects 
    classB objectB1, objectB2;
    deSerialization.DeSerialize(objectB1);
    
    // re-use our object pointer for the useless classA object
    delete objectA2pointer;
    objectA2pointer = dynamic_cast<classA *>(deSerialization.DeSerialize());

    // read last "string" object
    deSerialization.DeSerialize(objectB2);
    std::cout << objectB1.Value << objectB2.Value << std::endl;

    // deserialize the double and int
    cmnDouble pi;
    cmnInt twoTen;
    deSerialization.DeSerialize(pi);
    deSerialization.DeSerialize(twoTen);
    std::cout << "PI: " << pi
              << "\n2^10: " << twoTen
              << std::endl;

    return 0;
}
