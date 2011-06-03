from RobotComponents import *

cmnLogger.SetMask( CMN_LOG_ALLOW_ALL )
cmnLogger.SetMaskFunction( CMN_LOG_ALLOW_ALL )
cmnLogger.SetMaskDefaultLog( CMN_LOG_ALLOW_ALL )

taskManager = mtsManagerLocal.GetInstance()

rtcan = devRTSocketCAN( 'rtcan0', devCAN.RATE_1000 )


# WAM stuff
WAMqinit = array( [ 0.0, -pi/2, 0.0, pi, 0.0, -pi/2, 0.0 ] )
WAM = CreateWAM( 'WAM', rtcan, WAMqinit )
WAM.Configure()
taskManager.AddComponent( WAM )

GC  = CreateGC( 'GC', '/home/sleonard/src/cisst/src/libs/etc/cisstRobot/WAM/wam7bh.rob' )

taskManager.Connect( GC.GetName(), devController.Output,
                     WAM.GetName(), devManipulator.Input )

taskManager.Connect( GC.GetName(), devController.Feedback,
                     WAM.GetName(), devManipulator.Output )


# BH stuff
BH = CreateBH( 'BH', rtcan )
BH.Configure()
taskManager.AddComponent( BH )


BHqinit = array( [ 0.0, 0.0, 0.0, 0.0] )
BHsetpoints = CreateSetPoints( 'BHsetpoints', BHqinit )
BHqdmax = array( [ 1.5, 1.5, 1.5, 0.1 ] )
BHtrajectory = CreateTrajectory( 'BHtrajectory', 0.05, BHqinit, BHqdmax )

taskManager.Connect( BHsetpoints.GetName(),  devSetPoints.Output,
                     BHtrajectory.GetName(), devTrajectory.Input )
taskManager.Connect( BHtrajectory.GetName(), devTrajectory.Output,
                     BH.GetName(),           devManipulator.Input )

StartTasks()

print 'ENTER to enable GC'
stdin.readline()
GC.EnableCommand()

BHq = array( [ 1.0, 1.0, 1.0, 1.0 ] )
BHsetpoints.Insert( BHq )
BHq = array( [ 0.0, 0.0, 0.0, 0.0 ] )
BHsetpoints.Insert( BHq )

print 'ENTER to close'
stdin.readline()
BHsetpoints.Latch()

print 'ENTER to open'
stdin.readline()
BHsetpoints.Latch()

print 'ENTER to exit'
stdin.readline()

StopTasks()
