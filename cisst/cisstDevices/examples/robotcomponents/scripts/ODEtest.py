from RobotComponents import *
from ODE             import *
from OSG             import *
from IPython import embed 

def PrintContactInfo(bodyname): 
    contacts = world.QueryContacts(bodyname)

    print "N contacts: ", contacts.size()
    
    while 0 < contacts.size() : 
        c = contacts.front()
        print "normal: ", c.normal, " depth: ", c.depth 
        contacts.pop_front()

cmnLogger.SetMask( CMN_LOG_ALLOW_ALL )
cmnLogger.SetMaskFunction( CMN_LOG_ALLOW_ALL )
cmnLogger.SetMaskDefaultLog( CMN_LOG_ALLOW_ALL )

# create the world
world = ODEWorld()



# create the camera
cameara = OSGMono( 'camera', world )

# Create a rigid body. Make up some mass + com + moit
mass = 1.0
com = array([0.0, 0.5, 0.5]) 
moit = eye(3) 

# create a ODEBody 
Rtw0 = vctFrm3()
Rw0  = Rtw0.Rotation();
Rw0[0][0] = 0.0; Rw0[0][1] = 0.0; Rw0[0][2] = -1.0;
Rw0[1][0] = 0.0; Rw0[1][1] = 1.0; Rw0[1][2] =  0.0;
Rw0[2][0] = 1.0; Rw0[2][1] = 0.0; Rw0[2][2] =  0.0;
tw0  = Rtw0.Translation();
tw0[0] = 0.75;
tw0[2] = 1.0;

cisst_source_root = os.environ['CISST_SOURCE_ROOT']
data   =   cisst_source_root+'/libs/etc/cisstRobot/objects/'

background = devODEBody('background', vctFrm3(), data+'background.3ds', world)

hubble = devODEBody( 'hubble',                              
                     Rtw0,                                
                     data+'cube.3ds', 
                     world,
                     mass, com, moit )

StartTasks()

print 'Press ENTER for interactive mode' 
stdin.readline()

embed()

StopTasks()
        
