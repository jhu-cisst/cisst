#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Created on Mon Aug 29 12:27:27 2011

@author: wen
"""

# Python standard library
import ctypes
import platform
import sys

# cisst
if platform.system() == 'Linux':
    sys.setdlopenflags(sys.getdlopenflags() | ctypes.RTLD_GLOBAL)
import time
import os    
import cisstMultiTaskPython as mts
import cisstParameterTypesPython as prm

print 'starting cisstStealthlinkClientComponent.py...'
manager = mts.mtsManagerLocal.GetInstance('localhost', 'TREK')
stealth = mts.mtsComponentWithManagement('stealthProxy')
print 'Created stealthProxy'
manager.AddComponent(stealth) 
print 'added stealthProxy Component to local manager'

time.sleep(1.0)

# print 'Before adding Controller Interface'
# stealth.AddInterfaceRequiredAndConnect(('ProcessServer', 'Stealthlink', 'Controller'))
# print 'After adding Controller Interface'
# time.sleep(1.0)
print 'Before adding Pointer Interface'
#import rpdb2; rpdb2.start_embedded_debugger('password')
stealth.AddInterfaceRequiredAndConnect(('ProcessServer', 'Stealthlink', 'Pointer'))
print 'After adding Pointer Interface'
time.sleep(1.0)
print 'Before adding Frame Interface'
stealth.AddInterfaceRequiredAndConnect(('ProcessServer', 'Stealthlink', 'Frame'))
print 'After adding Frame Interface'
time.sleep(1.0)
print 'Before adding Registration Interface'
stealth.AddInterfaceRequiredAndConnect(('ProcessServer', 'Stealthlink', 'Registration'))
print 'After adding Registration Interface'
time.sleep(1.0)
# print 'Before adding Endoscope Interface'
# stealth.AddInterfaceRequiredAndConnect(('ProcessServer', 'Stealthlink', 'Endoscope'))
# print 'After adding Endoscope Interface'
# time.sleep(1.0)
#manager = mts.mtsManagerLocal.GetInstance()
manager.CreateAll()
manager.StartAll()

time.sleep(10.0)
stealth.RequiredForPointer.UpdateFromC()
stealth.RequiredForFrame.UpdateFromC()
stealth.RequiredForRegistration.UpdateFromC()
# stealth.RequiredForEndoscope.UpdateFromC()
time.sleep(5.0)
# pose = prm.prmPositionCartesianGet() 


while(True):   
    time.sleep(0.5) # half second
    result = stealth.RequiredForPointer.GetPositionCartesian.IsValid()
    print 'function valid: ', result
    pose = stealth.RequiredForPointer.GetPositionCartesian()
    print 'Pointer: ', pose.Position()
    pose = stealth.RequiredForFrame.GetMarkerCartesian()
    print 'Frame: ', pose.Position()
    transform = stealth.RequiredForRegistration.GetTransformation()
    print 'Registration: ', transform
    # pose = stealth.RequiredForEndoscope.GetPositionCartesian()
    # print 'Endoscope: ', pose.Position()
    
#stealth.RequiredForPointer.UpdateFromC()
#stealth.RequiredForPointer.GetPositionCartesian(pose)
#print pos.Position().Rotation()
#print pos.Position().Translation()
#stealth.RequiredForRegistration.UpdateFromC()
#stealth.RequiredForRegistration.GetPositionCartesian(pose)
#print pos.Position().Rotation()
#print pos.Position().Translation()
