/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2007-04-08

  (C) Copyright 2007-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


// include what is needed from cisst
#include <cisstCommon/cmnSerializer.h>
#include <cisstCommon/cmnDeSerializer.h>
#include <cisstCommon/cmnGenericObjectProxy.h>
#include <cisstCommon/cmnConstants.h>

#include "classes.h"

// system includes
#include <iostream>


// main function
int main(void) {
    // create the output stream in binary mode
    std::ofstream outputStream("out.dat", std::ofstream::binary);
    cmnSerializer serialization(outputStream);

    // create objects to serialize
    classA objectA1, objectA2, objectA3;
    objectA1.Value = 1.0 / 3.0;
    serialization.Serialize(objectA1);

    objectA2.Value = 2.0 / 3.0;
    serialization.Serialize(objectA2);

    objectA3.Value = 5.0 / 3.0;
    serialization.Serialize(objectA3);

    // serialize two "string" objects
    classB objectB1, objectB2;
    objectB1.Value = "Hello ";
    objectB2.Value = "World!";

    serialization.Serialize(objectB1);
    serialization.Serialize(objectA1); // again, just to mix them!
    serialization.Serialize(objectB2);

    // serialize two simple objects based on double and int
    cmnDouble pi = cmnPI;
    cmnInt twoTen = 1024;
    serialization.Serialize(pi);
    serialization.Serialize(twoTen);

    // close the stream
    outputStream.close();

    // now try to read within the same execution
    std::ifstream inputStream("out.dat", std::ifstream::binary);
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
    deSerialization.DeSerialize(objectB1);

    // re-use our object pointer for the useless classA object
    delete objectA2pointer;
    objectA2pointer = dynamic_cast<classA *>(deSerialization.DeSerialize());

    // read last "string" object
    deSerialization.DeSerialize(objectB2);
    std::cout << objectB1.Value << objectB2.Value << std::endl;

    // deserialize the double and int and play with the conversions
    // between cmnSimpleObject<_elementType> and _elementType
    cmnDouble piAgain;
    double twoPi;
    cmnInt twoTenAgain;
    int twoEleven;
    deSerialization.DeSerialize(piAgain);
    twoPi = piAgain * 2.0;
    piAgain -= cmnPI;
    deSerialization.DeSerialize(twoTenAgain);
    twoEleven = twoTenAgain * 2;
    std::cout << "2*PI: " << twoPi
              << "\n0: " << piAgain
              << "\n2^11: " << twoEleven
              << std::endl;

    inputStream.close();

    return 0;
}
