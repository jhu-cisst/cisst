/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Anton Deguet
  Created on: 2003-07-28
  
  (C) Copyright 2003-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "cmnClassRegisterTest.h"
#include "cmnClassRegisterTestStatic.h"
#include "cmnClassRegisterTestDynamic.h"


CMN_IMPLEMENT_SERVICES(TestA);
CMN_IMPLEMENT_SERVICES(TestB);
CMN_IMPLEMENT_SERVICES(TestC);
CMN_IMPLEMENT_SERVICES(TestC2);
CMN_IMPLEMENT_SERVICES_TEMPLATED(TestD12);
CMN_IMPLEMENT_SERVICES_TEMPLATED(TestD34);


void cmnClassRegisterTest::setUp(void)
{
    cmnLogger::AddChannel(OutputStream, CMN_LOG_LOD_VERY_VERBOSE);
}


void cmnClassRegisterTest::tearDown(void)
{
    /* restore the class LoD for multiple iterations of this tests*/
    cmnClassRegister::SetLoD("TestA", CMN_LOG_LOD_RUN_ERROR);
    cmnLogger::GetMultiplexer()->RemoveChannel(OutputStream);
}


void cmnClassRegisterTest::TestRegistration(void) {

    const cmnClassServicesBase* classAServices = NULL;
    const cmnClassServicesBase* classBServices = NULL;
    const cmnClassServicesBase* classCServices = NULL;
    const cmnClassServicesBase* classC1Services = NULL;
    const cmnClassServicesBase* classC2Services = NULL;
    const cmnClassServicesBase* classD12Services = NULL;
    const cmnClassServicesBase* classD34Services = NULL;

    classAServices = cmnClassRegister::FindClassServices("TestA");
    CPPUNIT_ASSERT(classAServices);
    CPPUNIT_ASSERT(TestA::ClassServices()->GetName() == "TestA");

    classBServices = cmnClassRegister::FindClassServices("TestB");
    CPPUNIT_ASSERT(classBServices);
    CPPUNIT_ASSERT(TestB::ClassServices()->GetName() == "TestB");

    classCServices = cmnClassRegister::FindClassServices("TestC");
    CPPUNIT_ASSERT(classCServices);
    CPPUNIT_ASSERT(TestC::ClassServices()->GetName() == "TestC");

    /* class TestC1 is not registered */
    classC1Services = cmnClassRegister::FindClassServices("TestC1");
    CPPUNIT_ASSERT(classC1Services == NULL);
    /* but the base class is, we should be able to get some
       services */
    TestC1 objectC1;
    CPPUNIT_ASSERT(objectC1.Services()->GetName() == "TestC");

    /* class TestC2 is registered */
    classC2Services = cmnClassRegister::FindClassServices("TestC2");
    CPPUNIT_ASSERT(classC2Services);
    CPPUNIT_ASSERT(TestC2::ClassServices()->GetName() == "TestC2");

    /* class TestD12 is registered */
    classD12Services = cmnClassRegister::FindClassServices("TestD12");
    CPPUNIT_ASSERT(classD12Services);
    CPPUNIT_ASSERT(TestD12::ClassServices()->GetName() == "TestD12");

    /* class TestD34 is registered */
    classD34Services = cmnClassRegister::FindClassServices("TestD34");
    CPPUNIT_ASSERT(classD34Services);
    CPPUNIT_ASSERT(TestD34::ClassServices()->GetName() == "TestD34");
}



void cmnClassRegisterTest::TestRegistrationStaticAllInline(void) {

    const cmnClassServicesBase* staticAllInlineServices = NULL;
    staticAllInlineServices = cmnClassRegister::FindClassServices("staticAllInline");
    CPPUNIT_ASSERT(staticAllInlineServices);
    CPPUNIT_ASSERT(staticAllInline::ClassServices()->GetName() == "staticAllInline");
    const staticAllInline staticAllInlineObject;
    CPPUNIT_ASSERT(staticAllInlineObject.Name() == "staticAllInline");
}



void cmnClassRegisterTest::TestRegistrationStaticNoInline(void) {

    const cmnClassServicesBase* staticNoInlineServices = NULL;
    staticNoInlineServices = cmnClassRegister::FindClassServices("staticNoInline");
    CPPUNIT_ASSERT(staticNoInlineServices);
    CPPUNIT_ASSERT(staticNoInline::ClassServices()->GetName() == "staticNoInline");
    const staticNoInline staticNoInlineObject;
    CPPUNIT_ASSERT(staticNoInlineObject.Name() == "staticNoInline");
}


#if USE_DYNAMIC_LIB_CLASS_REGISTRATION_TESTS
void cmnClassRegisterTest::TestRegistrationDynamicAllInline(void) {

    const cmnClassServicesBase* dynamicAllInlineServices = NULL;
    dynamicAllInlineServices = cmnClassRegister::FindClassServices("dynamicAllInline");
    CPPUNIT_ASSERT(dynamicAllInlineServices);
    CPPUNIT_ASSERT(dynamicAllInline::ClassServices()->GetName() == "dynamicAllInline");
    const dynamicAllInline dynamicAllInlineObject;
    CPPUNIT_ASSERT(dynamicAllInlineObject.Name() == "dynamicAllInline");
}



void cmnClassRegisterTest::TestRegistrationDynamicNoInline(void) {
    const cmnClassServicesBase* dynamicNoInlineServices = NULL;
    dynamicNoInlineServices = cmnClassRegister::FindClassServices("dynamicNoInline");
    CPPUNIT_ASSERT(dynamicNoInlineServices);
    CPPUNIT_ASSERT(dynamicNoInline::ClassServices()->GetName() == "dynamicNoInline");
    const dynamicNoInline dynamicNoInlineObject;
    CPPUNIT_ASSERT(dynamicNoInlineObject.Name() == "dynamicNoInline");
}
#endif // USE_DYNAMIC_LIB_CLASS_REGISTRATION_TESTS


void cmnClassRegisterTest::TestLoD(void) {
    cmnLogLoD lod;
    const cmnClassServicesBase* classAServices = NULL;
    TestA objectA;

    /* access the class lod via the class itself, therefore should
       find the default LoD, i.e. 5.  this will force the
       registration */
    lod = objectA.ClassServices()->GetLoD();
    CPPUNIT_ASSERT(lod == 5);

    /* try again via the class register, but first check that the
       class is known */
    classAServices = cmnClassRegister::FindClassServices("TestA");
    CPPUNIT_ASSERT(classAServices != NULL);
    /* the class info known by the object should be the same as the
       one known by the class register */
    CPPUNIT_ASSERT(classAServices == objectA.ClassServices());
    CPPUNIT_ASSERT(classAServices == TestA::ClassServices());
    CPPUNIT_ASSERT(classAServices == objectA.Services());

    /* set the LoD via the class and check via the class and class
       register */
    objectA.ClassServices()->SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    lod = objectA.ClassServices()->GetLoD();
    CPPUNIT_ASSERT(lod == CMN_LOG_LOD_VERY_VERBOSE);
    lod = cmnClassRegister::FindClassServices("TestA")->GetLoD();
    CPPUNIT_ASSERT(lod == CMN_LOG_LOD_VERY_VERBOSE);

    /* set the LoD via the class register and check via the class and
       the class register (with macro) */
    cmnClassRegister::SetLoD("TestA", CMN_LOG_LOD_INIT_VERBOSE);
    lod = objectA.ClassServices()->GetLoD();
    CPPUNIT_ASSERT(lod == CMN_LOG_LOD_INIT_VERBOSE);
    lod = cmnClassRegister::FindClassServices("TestA")->GetLoD();
    CPPUNIT_ASSERT(lod == CMN_LOG_LOD_INIT_VERBOSE);

    /* set the global LoD */
    cmnLogger::SetLoD(CMN_LOG_LOD_RUN_ERROR);
    CPPUNIT_ASSERT(cmnLogger::GetLoD() == CMN_LOG_LOD_RUN_ERROR);
    cmnLogger::SetLoD(CMN_LOG_LOD_RUN_DEBUG);
    CPPUNIT_ASSERT(cmnLogger::GetLoD() == CMN_LOG_LOD_RUN_DEBUG);

    /* restore the class LoD for multiple iterations of this tests*/
    cmnClassRegister::SetLoD("TestA", CMN_LOG_LOD_RUN_ERROR);
}



void cmnClassRegisterTest::TestLog(void) {

    /* set the level of detail for class TestA */
    TestA objectA;
    cmnClassRegister::SetLoD("TestA", CMN_LOG_LOD_RUN_WARNING);
    cmnLogger::SetLoD(CMN_LOG_LOD_RUN_ERROR);

    OutputStream.str("");
    objectA.Message(CMN_LOG_LOD_INIT_DEBUG);
    std::string expectedLog =
        std::string(cmnLogLoDString[CMN_LOG_LOD_INIT_DEBUG])
        + " - Function " + objectA.Services()->GetName() + ": 4\n"
        + std::string(cmnLogLoDString[CMN_LOG_LOD_INIT_DEBUG])
        + " - Class " + objectA.Services()->GetName() + ": 4\n";
    CPPUNIT_ASSERT(OutputStream.str() == expectedLog);

    OutputStream.str("");
    objectA.Message(CMN_LOG_LOD_RUN_ERROR);
    expectedLog =
        std::string(cmnLogLoDString[CMN_LOG_LOD_RUN_ERROR])
        + " - Function " + objectA.Services()->GetName() + ": 5\n"
        + std::string(cmnLogLoDString[CMN_LOG_LOD_RUN_ERROR])
        + " - Class " + objectA.Services()->GetName() + ": 5\n";
    CPPUNIT_ASSERT(OutputStream.str() == expectedLog);

    /* global LoD prevails, nothing goes thru */
    OutputStream.str("");
    objectA.Message(CMN_LOG_LOD_RUN_WARNING);
    CPPUNIT_ASSERT(OutputStream.str() == "");

    /* set a higher global LoD */
    cmnLogger::SetLoD(CMN_LOG_LOD_RUN_DEBUG);
    OutputStream.str("");
    objectA.Message(CMN_LOG_LOD_RUN_WARNING);
    expectedLog =
        std::string(cmnLogLoDString[CMN_LOG_LOD_RUN_WARNING])
        + " - Function " + objectA.Services()->GetName() + ": 6\n"
        + std::string(cmnLogLoDString[CMN_LOG_LOD_RUN_WARNING])
        + " - Class " + objectA.Services()->GetName() + ": 6\n";
    CPPUNIT_ASSERT(OutputStream.str() == expectedLog);

    OutputStream.str("");
    objectA.Message(CMN_LOG_LOD_RUN_DEBUG);
    expectedLog =
        std::string(cmnLogLoDString[CMN_LOG_LOD_RUN_DEBUG])
        + " - Function " + objectA.Services()->GetName() + ": 8\n";
    CPPUNIT_ASSERT(OutputStream.str() == expectedLog);
}



void cmnClassRegisterTest::TestDynamicCreation(void) {
    cmnGenericObject* objectA = cmnClassRegister::Create("TestA");
    CPPUNIT_ASSERT(objectA == NULL);
    
    cmnGenericObject* objectB = cmnClassRegister::Create("TestB");
    CPPUNIT_ASSERT(objectB == NULL);
    
    cmnGenericObject* objectC = cmnClassRegister::Create("TestC");
    CPPUNIT_ASSERT(objectC);
    TestC realObjectC;
    CPPUNIT_ASSERT(typeid(*objectC) == typeid(realObjectC));

    /* if the class does not contain the needed macros, we should not be able to create */    
    cmnGenericObject* objectC1 = cmnClassRegister::Create("TestC1");
    CPPUNIT_ASSERT(objectC1 == NULL);

    cmnGenericObject* objectC2 = cmnClassRegister::Create("TestC2");
    CPPUNIT_ASSERT(objectC2);
    TestC2 realObjectC2;
    CPPUNIT_ASSERT(typeid(*objectC2) == typeid(realObjectC2));
    CPPUNIT_ASSERT(typeid(*objectC2) != typeid(realObjectC));
    /* test that default constructor has been used */
    TestC2 * createdOjectC2defautCtor = dynamic_cast<TestC2 *>(objectC2);
    CPPUNIT_ASSERT(createdOjectC2defautCtor != 0);
    CPPUNIT_ASSERT(createdOjectC2defautCtor->Size == 2);
    CPPUNIT_ASSERT(createdOjectC2defautCtor->Elements[0] == 1.0);
    CPPUNIT_ASSERT(createdOjectC2defautCtor->Elements[1] == 2.0);
    /* now test the copy constructor */
    free(createdOjectC2defautCtor->Elements);
    createdOjectC2defautCtor->Size = 3;
    createdOjectC2defautCtor->Elements = new double[3];
    createdOjectC2defautCtor->Elements[0] = 3;
    createdOjectC2defautCtor->Elements[1] = 2;
    createdOjectC2defautCtor->Elements[2] = 1;
    cmnGenericObject * createdOjectCopyCtor = createdOjectC2defautCtor->ClassServices()->Create(*createdOjectC2defautCtor);
    CPPUNIT_ASSERT(createdOjectCopyCtor != 0);
    TestC2 * createdOjectC2CopyCtor = dynamic_cast<TestC2 *>(createdOjectCopyCtor);
    CPPUNIT_ASSERT(createdOjectC2CopyCtor != 0);
    CPPUNIT_ASSERT(createdOjectC2CopyCtor->Size == createdOjectC2defautCtor->Size);
    CPPUNIT_ASSERT(createdOjectC2CopyCtor->Elements != createdOjectC2defautCtor->Elements); // pointer should change
    CPPUNIT_ASSERT(createdOjectC2CopyCtor->Elements[0] == createdOjectC2defautCtor->Elements[0]);
    CPPUNIT_ASSERT(createdOjectC2CopyCtor->Elements[1] == createdOjectC2defautCtor->Elements[1]);
    CPPUNIT_ASSERT(createdOjectC2CopyCtor->Elements[2] == createdOjectC2defautCtor->Elements[2]);

    /* test the copy constructor from an actual object, using the class register */
    createdOjectCopyCtor = cmnClassRegister::Create("TestC2", realObjectC2);
    CPPUNIT_ASSERT(createdOjectCopyCtor != 0);
    CPPUNIT_ASSERT(createdOjectC2CopyCtor->Elements != createdOjectC2defautCtor->Elements); // pointer should change

    /* test the copy constructor with another type */
    createdOjectCopyCtor = createdOjectC2defautCtor->ClassServices()->Create(*objectC);
    CPPUNIT_ASSERT(createdOjectCopyCtor == 0);

    /* test dynamic creation of templated type */
    cmnGenericObject* objectD34 = cmnClassRegister::Create("TestD34");
    CPPUNIT_ASSERT(objectD34);
    TestD34 realObjectD34a;
    CPPUNIT_ASSERT(typeid(*objectD34) == typeid(realObjectD34a));
    TestD<3, 4> realObjectD34b;
    CPPUNIT_ASSERT(typeid(*objectD34) == typeid(realObjectD34b));
}


void cmnClassRegisterTest::TestIterators(void)
{
    bool foundA = false;
    bool foundB = false;
    bool foundC = false;
    bool foundC2 = false;

    cmnClassRegister::const_iterator iter = cmnClassRegister::begin();
    const cmnClassRegister::const_iterator end = cmnClassRegister::end();
    for (; iter != end; ++iter) {
        if ((*iter).second->GetName() == "TestA") foundA = true;
        if ((*iter).second->GetName() == "TestB") foundB = true;
        if ((*iter).second->GetName() == "TestC") foundC = true;
        if ((*iter).second->GetName() == "TestC2") foundC2 = true;
    }
    CPPUNIT_ASSERT(foundA);
    CPPUNIT_ASSERT(foundB);
    CPPUNIT_ASSERT(foundC);
    CPPUNIT_ASSERT(foundC2);
}

