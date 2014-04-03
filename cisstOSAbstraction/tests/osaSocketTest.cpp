/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ali Uneri
  Created on: 2010-01-30

  (C) Copyright 2007-2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <string.h>

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstOSAbstraction/osaSocket.h>
#include <cisstOSAbstraction/osaSocketServer.h>

#include "osaSocketTest.h"


void osaSocketTest::TestUDP(void)
{
    osaSocket serverSocket(osaSocket::UDP);
    osaSocket clientSocket(osaSocket::UDP);

    bool success;
    int bytes;
    char buffer[512];

    success = serverSocket.AssignPort(1234);
    CPPUNIT_ASSERT(success);

    clientSocket.SetDestination("127.0.0.1", 1234);

    bytes = clientSocket.Send("testing");
    CPPUNIT_ASSERT_EQUAL(7, bytes);

    osaSleep(10.0 * cmn_ms);

    buffer[0] = '\0';
    bytes = serverSocket.Receive(buffer, sizeof(buffer));
    CPPUNIT_ASSERT_EQUAL(7, bytes);
    CPPUNIT_ASSERT(strcmp("testing", buffer) == 0);
}


void osaSocketTest::TestTCP(void)
{
    osaSocketServer server;
    osaSocket * serverSocket;
    osaSocket clientSocket;

    bool success;
    int bytes;
    char buffer[512];

    int trials = 10;
    do {
        trials--;
        success = server.AssignPort(1234);
    } while(!success && trials);

    if (trials == 0) {
        CPPUNIT_FAIL("could not assign port");
        return;
    }

    success = server.Listen();
    CPPUNIT_ASSERT(success);

    success = clientSocket.Connect("127.0.0.1", 1234);
    CPPUNIT_ASSERT(success);

    serverSocket = server.Accept();
    CPPUNIT_ASSERT(serverSocket);

    bytes = clientSocket.Send("testing");
    CPPUNIT_ASSERT_EQUAL(7, bytes);

    osaSleep(10.0 * cmn_ms);

    buffer[0] = '\0';
    bytes = serverSocket->Receive(buffer, sizeof(buffer));
    CPPUNIT_ASSERT_EQUAL(7, bytes);
    CPPUNIT_ASSERT(strcmp("testing", buffer) == 0);

//    clientSocket.Close();
//
//    osaSleep(1.0 * cmn_s);
//
//    bytes = serverSocket->Send("testing");
//    CPPUNIT_ASSERT_EQUAL(-1, bytes);
//
//    bytes = clientSocket.Send("testing");
//    CPPUNIT_ASSERT_EQUAL(-1, bytes);
//
//    success = clientSocket.Connect("127.0.0.1", 1234);
//    CPPUNIT_ASSERT(success);
//
//    delete serverSocket;
//    serverSocket = server.Accept();
//    CPPUNIT_ASSERT(serverSocket);
//
//    bytes = clientSocket.Send("testing");
//    CPPUNIT_ASSERT_EQUAL(7, bytes);
//
//    osaSleep(10.0 * cmn_ms);
//
//    buffer[0] = '\0';
//    bytes = serverSocket->Receive(buffer, sizeof(buffer));
//    CPPUNIT_ASSERT_EQUAL(7, bytes);
//    CPPUNIT_ASSERT(strcmp("testing", buffer) == 0);
}

CPPUNIT_TEST_SUITE_REGISTRATION(osaSocketTest);
