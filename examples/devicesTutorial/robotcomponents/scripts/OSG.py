from   numpy import *
import numpy.linalg
import numpy.random

from cv import *;

from cisstCommonPython    import *;
from cisstMultiTaskPython import *;
from cisstVectorPython    import *;
from cisstDevicesPython   import *;

from sys import *

def OSGWorld():
    # Create the world (OSG node)
    osgworld = devOSGWorld()
    return osgworld

def OSGMono(camname, world, w=640, h=480, fov=55.0, Zn=0.1, Zf=10.0, x=0, y=0):
    aspect = double( w ) / double( h )
    # Create the camera thread
    osgmono = devOSGMono( camname, world, x, y, w, h, fov, aspect, Zn, Zf )
    # Get the task manager
    taskManager = mtsManagerLocal.GetInstance()
    # Add he camera task
    taskManager.AddComponent( osgmono )
    return osgmono


