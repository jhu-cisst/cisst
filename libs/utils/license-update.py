#!/usr/bin/python
# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
# $Id: license-update.py 8 2009-01-04 21:13:48Z adeguet1 $
#
# Author(s): Anton Deguet
# Created on: 2005-05-19

# (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
# Reserved.

# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---


import sys
import os
import stat
import types
import shutil
import fnmatch
import re

# Process a single directory, recursive calls are here

def processDirectory(directory):
    print "Processing directory: ", directory
    # list the directory
    try:
        files = os.listdir(directory)
    except os.error:
        print "Can't list directory", directory
        sys.exit()
    # check each file
    for file in files:
        if file != "CVS":
            fullFile = os.path.join(directory, file)
            try:
                fileStat = os.lstat(fullFile)
            except os.error:
                print "Error: Can't stat file", fullFile
                sys.exit()
            if stat.S_ISDIR(fileStat.st_mode):
                processDirectory(fullFile)
            else:
                processFile(fullFile)


# Process a file: make a backup, try to find the license delimiters
# and replace with the new license

def processFile(file):
    toProcess = 0
    
    # identify the comment character
    if fnmatch.fnmatch(file, '*.h') \
    or fnmatch.fnmatch(file, '*.cpp') \
    or fnmatch.fnmatch(file, '*.h.in') \
    or fnmatch.fnmatch(file, '*.cpp.in') \
    or fnmatch.fnmatch(file, '*.i') \
    or fnmatch.fnmatch(file, '*.i.in'):
        license = licenseCpp
        pattern = patternCpp
        delimiters = delimitersCpp
        toProcess = 1
    elif fnmatch.fnmatch(file, '*.txt') \
    or fnmatch.fnmatch(file, '*.cmake') \
    or fnmatch.fnmatch(file, '*.cmake.in') \
    or fnmatch.fnmatch(file, '*.py') \
    or fnmatch.fnmatch(file, '*.py.in') \
    or fnmatch.fnmatch(file, '*.csh') \
    or fnmatch.fnmatch(file, '*.csh.in') \
    or fnmatch.fnmatch(file, '*.sh') \
    or fnmatch.fnmatch(file, '*.sh.in'):
        license = licenseSharp
        pattern = patternSharp
        delimiters = delimitersSharp
        toProcess = 1
    elif fnmatch.fnmatch(file, '*.bat') \
    or fnmatch.fnmatch(file, '*.bat.in'):
        license = licenseAt
        pattern = patternAt
        delimiters = delimitersAt
        toProcess = 1

    # process it if required
    if toProcess == 1:
        # load file content
        sourceFile = open(file)
        fileContent = sourceFile.read()
        sourceFile.close()
        # try to locate and replace old license
        match = pattern.search(fileContent)
        if match:
            newContent = match.group(1) + license + match.group(3)
            if newContent == fileContent:
                print "Info: File", file, "already has the updated license"
            else:
                # backup
                backup = file + ".before-license"
                shutil.copy(file, backup)
                # write new content back to file
                sourceFile = open(file, "w")
                sourceFile.write(newContent)
                sourceFile.close()
                print "Info: File", file, "updated with new license"
        else:
            print "Warning: File", file, "doesn't contain the required delimiters:\n", delimiters

    # file not processed, warning
    else:
        print "Warning: File", file, "not processed, unknown type"

        


# "main" 

if len(sys.argv) <= 2:
    print "Parameters are: license-file file-or-directory [file-or-directory ...]"
    sys.exit()

# load the content of the license file
licenseFile = open(sys.argv[1])
licenseContent = licenseFile.read()
licenseFile.close()

# license delimiters (use + to be able to use the script on itself)
licenseStart = "--- begin cisst license" + " - do not edit ---"
licenseEnd = "--- end cisst" + " license ---"

# create a full license with delimiters
licenseContent = licenseStart + licenseContent + licenseEnd

# create a string for c++, etc
licenseCpp = licenseContent
patternCpp = re.compile("(.*)" + licenseStart + "(.*)" + licenseEnd + "(.*)", re.DOTALL)
delimitersCpp = licenseStart + "\n" + licenseEnd

# create a string with everyline preceeded with # for shell
licenseSharp = "# " + re.sub(r'\n', "\n# ", licenseContent)
patternSharp = re.compile("(.*)# " + licenseStart + "(.*)" + licenseEnd + "(.*)", re.DOTALL)
delimitersSharp = "# " + licenseStart + "\n# " + licenseEnd

# create a string with everyline preceeded with @REM for DOS Batch
licenseAt = "@rem " + re.sub(r'\n', "\n@rem ", licenseContent)
patternAt = re.compile("(.*)@rem " + licenseStart + "(.*)" + licenseEnd + "(.*)", re.DOTALL)
delimitersAt = "@rem " + licenseStart + "\n@rem " + licenseEnd

# directories or files to process
files = sys.argv[2:]
    
# process parameters
for file in files:
    if file != "CVS":
        try:
            fileStat = os.lstat(file)
        except os.error:
            print "Error: Can't stat file", file
            sys.exit()
        if stat.S_ISDIR(fileStat.st_mode):
            processDirectory(file)
        else:
            processFile(file)

#
# $Log: license-update.py,v $
# Revision 1.9  2007/04/26 19:33:58  anton
# All files in libraries: Applied new license text, separate copyright and
# updated dates, added standard header where missing.
#
# Revision 1.8  2006/11/20 20:33:20  anton
# Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
# cisstInteractive, cisstImage and cisstOSAbstraction.
#
# Revision 1.7  2006/05/02 19:18:19  anton
# license-update.py: Added support for .sh, .csh and .bat files.
#
# Revision 1.6  2005/09/26 16:36:05  anton
# cisst: Added modelines for emacs and vi (python), see #164.
#
# Revision 1.5  2005/09/07 03:31:00  anton
# license-update.py: Added license in script as well as support for *.py and
# *.py.in files.
#
# Revision 1.4  2005/08/19 05:27:40  anton
# license-update.py: Added suffix .i for SWIG interface files.
#
# Revision 1.3  2005/06/03 18:57:02  anton
# license-update script: Added support for files *.cmake and *.cmake.in
#
# Revision 1.2  2005/06/03 18:24:36  anton
# license update script: Added support for *.cpp.in and *.h.in
#
# Revision 1.1  2005/05/19 15:20:45  anton
# License: Added the text and a python utility to update the license in every
# file.
#
#
