/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/*

  Author(s):  Anton Deguet
  Created on: 2008-02-10

  (C) Copyright 2008-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <iostream>
#include <cisstRevision.h>
#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnLogger.h>

#include <demoData.h>

// demonstrate some features and how to use them in templated code
template <class _elementType>
void show(const _elementType & value, const std::string & name)
{
    typedef cmnData<_elementType> DataType;
    std::cout << "--- " << name << std::endl
              << "description:    " << DataType::SerializeDescription(value, ',', name) << std::endl
              << "serialized:     ";
    DataType::SerializeText(value, std::cout, ',');
    std::cout << std::endl
              << "human readable: " << DataType::HumanReadable(value) << std::endl;
    size_t numberOfScalars = DataType::ScalarNumber(value);
    std::cout << "number of scalars: " << numberOfScalars << std::endl;
    for (size_t index = 0; index < numberOfScalars; ++index) {
        std::cout << "scalar[" << index << "]"
                  << " - description: " << DataType::ScalarDescription(value, index, name) << std::endl
                  << " - value      : " << DataType::Scalar(value, index) << std::endl;
    }
    std::cout << std::endl;
}


int main(void)
{
    std::cout << CISST_FULL_REVISION << std::endl;
    demoData d1;
    // all other elements of demoData have default values
    d1.FixedVector = demoData::FixedVectorType(9.99, 9.99);
    TestNamespace::demoData2 d2;
    show(d1, "d1");
    show(d2, "d2");
    return 0;
}
