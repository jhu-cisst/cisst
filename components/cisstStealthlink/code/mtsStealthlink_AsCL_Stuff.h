/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Peter Kazanzides, Anton Deguet
  Created on: 2006

  (C) Copyright 2007-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsStealthlink_AsCL_Stuff_h
#define _mtsStealthlink_AsCL_Stuff_h

#include <AsCL/AsCL_Base_Stuff.h>

class osaStopwatch;

//------------------------------------------------------------------------------
class mtsStealthlink_AsCL_IO_Watch: public AsCL_IO_Watch
{
public:

  mtsStealthlink_AsCL_IO_Watch();
  virtual ~mtsStealthlink_AsCL_IO_Watch();

  virtual int AddWatch(int, void *, void *);
  virtual void RemoveWatch(void);
  void CheckWatch(void);

private:

  typedef void (*mtsStealthlink_CallBack)(int, void *);
  typedef void (*Watch_Callback)(void *);

  int fd;
  Watch_Callback Callback;
  void *ClientPtr;
};


//------------------------------------------------------------------------------
class mtsStealthlink_AsCL_Timeout: public AsCL_Timeout
{
public:
  mtsStealthlink_AsCL_Timeout();
  ~mtsStealthlink_AsCL_Timeout();

  // Units of tmo_val?  This implementation assumes milliseconds.
  virtual int  AddTimeout(int tmo_val, void * func, void * obj);
  virtual void RemoveTimeout(void);
  void CheckTimeout(void);

private:
  typedef int (*Timeout_Callback) (void * data);

  osaStopwatch *StealthlinkTimer;

  double Timeout;   // seconds
  Timeout_Callback Callback;
  void *DataObj;
};


//------------------------------------------------------------------------------
class mtsStealthlink_AsCL_Utils: public AsCL_Utils
{
  mtsStealthlink_AsCL_IO_Watch *curWatch;
  mtsStealthlink_AsCL_Timeout *curTimeout;

public:
  mtsStealthlink_AsCL_Utils();
  virtual ~mtsStealthlink_AsCL_Utils();

  virtual AsCL_IO_Watch * new_IO_Watch(void);
  virtual AsCL_Timeout * new_Timeout(void);

  void CheckCallbacks(void);
};

#endif // _mtsStealthlink_AsCL_Stuff_h
