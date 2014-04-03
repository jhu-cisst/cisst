#!/usr/bin/python

# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
#  Author(s):  Andrew LaMora
#  Created on: 2005-09-06
#
#  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
#  Reserved.

# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

# Python script file to compile Python source files (*.py) and create
# Python binary files (*.pyc).  This script file requires two
# command line arguments:
# 
#    src-dir    Directory for the input source files (*.py)
#    dest-dir   Directory for the output binary files (*.py)
#
# If an error occurs (e.g., could not delete file, create directory, etc.)
# the script exits with a non-zero return code.


"""makeIrepy src-dir  dest-dir

This module compiles the source files of the IRE IDE into the
appropriate destination (release) directory.
"""
import os, sys, glob, py_compile
#import shutil

def main():
    if len(sys.argv)<3:
        print "Syntax:  python %s src-dir dest-dir" % sys.argv[0]
        sys.exit(1)

    src = sys.argv[1]
    dest = sys.argv[2]

    # remove all .pyc files in destination directory
    # (this won't fail even if the directory does not exist)
    try:
       map(os.remove, glob.glob(os.path.join(dest,'*.pyc')))
    except OSError, e:
       print 'makeIrepy: could not delete ' + e.filename + ', ' + e.strerror
       sys.exit(1)

    # create the directory (if it does not already exist)
    if not os.path.isdir(dest):
        try:
           os.makedirs(dest)
        except OSError, e:
           print 'makeIrepy: could not create destination directory: ' + dest
           print e
           sys.exit(1)

    # compile the source files
    for f in glob.glob1(src,'*.py'):
        py_compile.compile(os.path.join(src,f), os.path.join(dest,f+'c'))
    
    #Now copy the images over.
    #This can be directly handled by CMake
    #shutil.copytree(os.path.join(src,'images', os.path.join(dest, 'images')))
    
if __name__ == "__main__":
    main()
    
    
