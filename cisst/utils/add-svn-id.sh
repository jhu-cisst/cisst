#!/bin/sh

# $Id$

# Author(s): Anton Deguet
# Created on: 2010-11-29

# (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
# Reserved.

# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

# find files in regular directories (i.e. not ".svn")
for FILE in `find . -type d -name ".svn" -prune -o -type f -print`
do
    # list svn keywords for that file 
    RESULT=`svn propget svn:keywords "$FILE"`
    # check if the list contains "Id"
    echo $RESULT | grep --silent "Id" 
    if [ "$?" -ne "0" ]
    then
	# otherwise add the "Id" keyword
	echo Adding Id svn keyword to "$FILE"
        svn propset svn:keywords "Id" "$FILE"
    fi
done
