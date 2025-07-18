# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
#  Author(s):	Peter Kazanzides
#  Created on: 2006-01-06
#
#  (C) Copyright 2006-2025 Johns Hopkins University (JHU), All Rights Reserved.

# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---


# This file contains image data used for IRE bitmaps/icons:
#
#   - SmallUpArrow:  used when sorting list columns
#   - SmallDnArrow:  used when sorting list columns
#   - NewItem:       create a new item (e.g., file, notebook tab, etc.)
#   - OpenItem:      open an existing item (e.g., file, folder, etc.)
#   - SaveItem:      save item to a file
#
# SmallUpArrow and SmallDnArrow were copied from the wxPython
# demo code.
#
# NewItem, OpenItem, SaveItem were generated from BMP files by img2py.py
# (in wxPython tools).

import sys
import wx
import zlib
if sys.version_info.major == 2:
    from cStringIO import StringIO as BytesIO
else:
    from io import BytesIO

#----------------------------------------------------------------------
def getSmallUpArrowData():
    return \
b"\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00<IDAT8\x8dcddbf\xa0\x040Q\xa4{h\x18\xf0\xff\xdf\xdf\xffd\x1b\x00\xd3\
\x8c\xcf\x10\x9c\x06\xa0k\xc2e\x08m\xc2\x00\x97m\xd8\xc41\x0c \x14h\xe8\xf2\
\x8c\xa3)q\x10\x18\x00\x00R\xd8#\xec\xb2\xcd\xc1Y\x00\x00\x00\x00IEND\xaeB`\
\x82"

def getSmallUpArrowBitmap():
    return wx.Bitmap(getSmallUpArrowImage())

def getSmallUpArrowImage():
    try:
        stream = BytesIO(getSmallUpArrowData())
    except TypeError as e:
        print('getSmallUpArrowImage: ', e)
    return wx.Image(stream)

#----------------------------------------------------------------------
def getSmallDnArrowData():
    return \
b"\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00HIDAT8\x8dcddbf\xa0\x040Q\xa4{\xd4\x00\x06\x06\x06\x06\x06\x16t\x81\
\xff\xff\xfe\xfe'\xa4\x89\x91\x89\x99\x11\xa7\x0b\x90%\ti\xc6j\x00>C\xb0\x89\
\xd3.\x10\xd1m\xc3\xe5*\xbc.\x80i\xc2\x17.\x8c\xa3y\x81\x01\x00\xa1\x0e\x04e\
?\x84B\xef\x00\x00\x00\x00IEND\xaeB`\x82" 

def getSmallDnArrowBitmap():
    return wx.Bitmap(getSmallDnArrowImage())

def getSmallDnArrowImage():
    try:
        stream = BytesIO(getSmallDnArrowData())
    except TypeError as e:
        print('getSmallDnArrowImage: ', e)
    return wx.Image(stream)

#----------------------------------------------------------------------
def getNewItemData():
    return zlib.decompress(
b"x\xda\xeb\x0c\xf0s\xe7\xe5\x92\xe2b``\xe0\xf5\xf4p\t\x02\xd2\x02@\xcc\xcf\
\xc1\x04$\x93\x04oT\x00)\xe6b'\xcf\x10\x0e\x0e\x8e\xdb\x0f\xfd\x1f\x00\xb9~\
\x9e.\x8e!\x1a\x13\x93m\n\x1e00k6\xb0\x9e+*(8\xb0\x8a\xe9\xac\x8bZ\xca\xb9z\
\x9e\x86L\x81.\xb65\x0f\xfe\xff\xff\x7f\xa2\x8a#;!%\xc5e[;\xeb\xfc(6\xb6\x8e\
\xd5\x0fnulb\xe4\xed\xc9R`[p\x84\xd9f\xc3G\x06\xb7\xa0`m\x86)\x13\x14\xafp[M\
u\x03\x9a\xcc\xe0\xe9\xea\xe7\xb2\xce)\xa1\t\x000\x8c/\xbb" )

def getNewItemBitmap():
    return wx.Bitmap(getNewItemImage())

def getNewItemImage():
    try:
        stream = BytesIO(getNewItemData())
    except TypeError as e:
        print('getNewItemImage: ', e)
    return wx.Image(stream)

#----------------------------------------------------------------------
def getOpenItemData():
    return zlib.decompress(
b"x\xda\xeb\x0c\xf0s\xe7\xe5\x92\xe2b``\xe0\xf5\xf4p\t\x02\xd2\x02@\xcc\xcf\
\xc1\x04$\x93\x04oT\x00)\xe6b'\xcf\x10\x0e\x0e\x8e\xdb\x0f\xfd\x1f\x00\xb9\
\xed\x9e.\x8e!\x1a\x13\xa7\x06\x1d\xe4=\xa2\xc0S\xec\xb7]\\\xae\xf7\xe2\xa5\
\x93\x97\xdc\xf5\x14]m\xbd>;L\xdb\x90\x1c\xad\xb0\xaa\xcb\xfcsl\xbc\xf9\xa5\
\x83\x82\xd5\xe1Mnj\xe7\x0b\x95S\xed'\xed\xfe+#q\xed\xff\x97\x99\xbe!S\x97H2\
0\xbb<i\x8b\xdd\x9e\xc2\xce\xccb`\xec\xf9\xe6\xf9\x96\xc7\xc2\t\xcf\xa6~6\
\xda\xec\xbcd\xd2\x8d\xa2\xcc\x08\xd6\xe6\xbb\xbfss\x19^\xd4\xba\x181\xf4\
\xe7\x84\xca\xf2Y\xe7\xa4\xbc\xf4-{\xbbA\xe8Jy\xc5G\xd3\x1a\xbf\xa7\xf7\xed\
\xcd\x18}Y\xd7\x02\x9d\xc4\xe0\xe9\xea\xe7\xb2\xce)\xa1\t\x00Y\xfcLT" )

def getOpenItemBitmap():
    return wx.Bitmap(getOpenItemImage())

def getOpenItemImage():
    try:
        stream = BytesIO(getOpenItemData())
    except TypeError as e:
        print('getOpenItemImage: ', e)
    return wx.Image(stream)

#----------------------------------------------------------------------
def getSaveItemData():
    return zlib.decompress(
b"x\xda\xeb\x0c\xf0s\xe7\xe5\x92\xe2b``\xe0\xf5\xf4p\t\x02\xd2\x02 \xcc\xc1\
\x04$'L\xcc0\x03R\xcc\xc5N\x9e!\x1c\x1c\x1c\xb7\x1f\xfa?\x00r\x13=]\x1cC4&n\
\r:\xc9y\xc0\x80g\xb2\x17\xdb\xe7\x0c9\xdf\xd9[\xf9\x9d\xa5\x92n\x1d\x99\xf2\
j\xb2{\x89\xa4\x8b\xe7\x8a\x07\x1a\x1b\x19\xb7T\x9c\xc9\xf1\xec\xd5+o\xda\
\xcdl!l\xb55 \xf3\xcc}!\x97\nG\x83E\xbe~*\xf1o\xd2\x1d\xff\x9d\xfb\x11\xf6R3\
\xf1{\x86g\xa5\xc5S\xaf7r\xa7KY&\xef_\xff\x95ib\x88\xc1\xf5+\xdb\x16K\x03-c\
\xf0t\xf5sY\xe7\x94\xd0\x04\x00\xc3\xa3<:" )

def getSaveItemBitmap():
    return wx.Bitmap(getSaveItemImage())

def getSaveItemImage():
    try:
        stream = BytesIO(getSaveItemData())
    except TypeError as e:
        print('getSaveItemsImage: ', e)
    return wx.Image(stream)
