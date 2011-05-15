from RobotComponents import *
from ODE             import *
from OSG             import *

world = ODEWorld()

cameara = OSGMono( 'camera', world )

Rtw0 = vctFrm3()
[lwr,grp]=ODELWRGripper( 'LWR', 'GRP', world, Rtw0 )

# create the trajectory + setpoint generator
lwrqinit = array( [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00] )
lwrqdmax = array( [0.10, 0.10, 0.10, 0.10, 0.10, 0.10, 0.10] )
lwrsetpoints = SetPoints( 'LWRsetpoints', lwrqinit );
lwrtrajectory = TrajectoryJoints( 'LWRtrajectory', lwrqinit, lwrqdmax )

taskManager = mtsManagerLocal.GetInstance();

taskManager.Connect( lwrsetpoints.GetName(),  devSetPoints.Output,
                     lwrtrajectory.GetName(), devTrajectory.Input );
taskManager.Connect( lwrtrajectory.GetName(), devTrajectory.Output,
                     lwr.GetName(),           devManipulator.Input );

# create the trajectory + setpoint generator
grpqinit = array( [0.00 ] )
grpqdmax = array( [0.005 ] )
grpsetpoints = SetPoints( 'GRPsetpoints', grpqinit );
grptrajectory = TrajectoryJoints( 'GRPtrajectory', grpqinit, grpqdmax )

taskManager = mtsManagerLocal.GetInstance();

taskManager.Connect( grpsetpoints.GetName(),  devSetPoints.Output,
                     grptrajectory.GetName(), devTrajectory.Input );
taskManager.Connect( grptrajectory.GetName(), devTrajectory.Output,
                     grp.GetName(),           devManipulator.Input );

StartTasks()

print 'ENTER to move'
stdin.readline()

lwrq = array( [1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00] )
lwrsetpoints.Insert( lwrq )
lwrsetpoints.Latch()

grpq = array( [ 1.00 ] )
grpsetpoints.Insert( grpq )
grpsetpoints.Latch()

print 'ENTER to exit.'
stdin.readline()

StopTasks()

