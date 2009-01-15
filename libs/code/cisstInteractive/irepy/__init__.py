# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#  $Id: __init__.py,v 1.6 2007/04/26 19:33:57 anton Exp $
#
#  Author(s):	Andrew LaMora
#  Created on: 2004-04-30
#
#  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
#  Reserved.

# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

#------------------------------------------
# Import the IRE libraries
#------------------------------------------
import ireMain

import threading

class IreThread(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
    def run(self):
        ireMain.launchIrePython()


#------------------------------------------
# function:  launch
#
# command-line command to launch the ire
#------------------------------------------
def launch():
	ireMain.launchIrePython()


#------------------------------------------
# Global statements
#------------------------------------------

#from pkgutil import extend_path
#__path__ = extend_path(__path__, __name__)

print "Welcome to the IRE!"
print "Use irepy.launch() to launch the IRE GUI"
#ireMain.launchIrePython()


##############################################################################
# $Log: __init__.py,v $
# Revision 1.6  2007/04/26 19:33:57  anton
# All files in libraries: Applied new license text, separate copyright and
# updated dates, added standard header where missing.
#
# Revision 1.5  2006/11/20 20:33:19  anton
# Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
# cisstInteractive, cisstImage and cisstOSAbstraction.
#
# Revision 1.4  2005/12/21 08:21:10  pkaz
# IRE: added support for running the IRE in a separate thread (uses Python threading module).
#
# Revision 1.3  2005/12/20 21:01:23  pkaz
# __init__.py:  removed some dead code.
#
# Revision 1.2  2005/09/26 16:36:05  anton
# cisst: Added modelines for emacs and vi (python), see #164.
#
# Revision 1.1  2005/09/06 22:43:08  alamora
# irepy module: init revision 0.1.0 checkin
#
# Revision 1.2  2005/02/01 22:15:00  alamora
# IRE now launches explicitly from irepy.launch()
# ireSimpleDialogs no longer loaded into def ns by default
#
# Revision 1.1  2005/01/25 00:52:29  alamora
# First addition of IRE to the (practical) world
#
