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

//------------------------------------------------------------------------------
class mtsStealthlink_AsCL_IO_Watch: public AsCL_IO_Watch
{
 public:

  mtsStealthlink_AsCL_IO_Watch();
  virtual ~mtsStealthlink_AsCL_IO_Watch();

  virtual int AddWatch(int, void *, void *);
  virtual void RemoveWatch();

 private:
  int _handler;
  int fd;

  typedef void (*mtsStealthlink_CallBack)(int, void *);
  typedef void (*GTK_CallBack)(void *);
  static GTK_CallBack callback;
  static void WatchCallback(int, void *);   // mtsStealthlink_CallBack
};


//------------------------------------------------------------------------------
class mtsStealthlink_AsCL_Timeout: public AsCL_Timeout
{
 public:
  mtsStealthlink_AsCL_Timeout();
  ~mtsStealthlink_AsCL_Timeout();

  virtual int  AddTimeout(int tmo_val, void * func, void * obj);
  virtual void RemoveTimeout(void);

 private:
  int _handler;
  void * dataobj;

  typedef int (*StealthFunction) (void * data);
  static StealthFunction callback;
  static double timeout;
  static void MyCallback(void * data);

};


//------------------------------------------------------------------------------
class mtsStealthlink_AsCL_Utils: public AsCL_Utils
{
 public:
  mtsStealthlink_AsCL_Utils();
  virtual ~mtsStealthlink_AsCL_Utils();

  virtual AsCL_IO_Watch * new_IO_Watch(void);
  virtual AsCL_Timeout * new_Timeout(void);
};

#endif // _mtsStealthlink_AsCL_Stuff_h
