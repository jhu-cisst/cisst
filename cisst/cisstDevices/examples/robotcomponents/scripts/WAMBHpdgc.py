from RobotComponents import *

cmnLogger.SetMask( CMN_LOG_ALLOW_ALL )
cmnLogger.SetMaskFunction( CMN_LOG_ALLOW_ALL )
cmnLogger.SetMaskDefaultLog( CMN_LOG_ALLOW_ALL )

taskManager = mtsManagerLocal.GetInstance()

rtcan = devRTSocketCAN( 'rtcan0', devCAN.RATE_1000 )


WAMqinit = array( [ 0.0, -pi/2-0.1, 0.0, pi, 0.0, -pi/2, 0.0 ] )
WAM = CreateWAM( 'WAM', rtcan, WAMqinit )
WAM.Configure()
taskManager.AddComponent( WAM )

PDGC  = CreatePDGC( 'PDGC', '/home/sleonard/src/cisst/src/libs/etc/cisstRobot/WAM/wam7bh.rob' )

WAMsetpoints = CreateSetPoints( 'WAMsetpoints', WAMqinit )
WAMqdmax = array( [ 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 ] )
WAMtrajectory = CreateTrajectory( 'WAMtrajectory', 0.01, WAMqinit, WAMqdmax )

taskManager.Connect( WAMsetpoints.GetName(),  devSetPoints.Output,
                     WAMtrajectory.GetName(), devTrajectory.Input )

taskManager.Connect( WAMtrajectory.GetName(), devTrajectory.Output,
                     PDGC.GetName(),          devManipulator.Input )

taskManager.Connect( PDGC.GetName(), devController.Output,
                     WAM.GetName(), devManipulator.Input )

taskManager.Connect( PDGC.GetName(), devController.Feedback,
                     WAM.GetName(), devManipulator.Output )

# BH stuff
BH = CreateBH( 'BH', rtcan )
BH.Configure()
taskManager.AddComponent( BH )


BHqinit = array( [ 0.0, 0.0, 0.0, 0.0] )
BHsetpoints = CreateSetPoints( 'BHsetpoints', BHqinit )
BHqdmax = array( [ 1.5, 1.5, 1.5, 0.1 ] )
BHtrajectory = CreateTrajectory( 'BHtrajectory', 0.1, BHqinit, BHqdmax )

taskManager.Connect( BHsetpoints.GetName(),  devSetPoints.Output,
                     BHtrajectory.GetName(), devTrajectory.Input )
taskManager.Connect( BHtrajectory.GetName(), devTrajectory.Output,
                     BH.GetName(),           devManipulator.Input )

StartTasks()

print 'ENTER to enable PDGC'
stdin.readline()
PDGC.EnableCommand()

BHq = array( [ 1.25, 1.25, 1.25, 0.0 ] )
BHsetpoints.Insert( BHq )
BHq = array( [ 0.0, 0.0, 0.0, 0.0 ] )
BHsetpoints.Insert( BHq )

print 'ENTER to approach'
stdin.readline()
WAMqa = array( [ 0.63, -1.00, 1.30, 1.54, -0.72, -1.08, -1.29 ] )
WAMsetpoints.Insert( WAMqa )
WAMsetpoints.Latch()

print 'ENTER to grip'
stdin.readline()
WAMqg = array( [ 0.46, -1.05, 1.40, 1.13, -0.76, -0.86, -1.08 ] )
WAMsetpoints.Insert( WAMqg )
WAMsetpoints.Latch()
BHsetpoints.Latch()

print 'ENTER to lift'
stdin.readline()
WAMsetpoints.Insert( WAMqa )
WAMsetpoints.Latch()

print 'ENTER to home'
stdin.readline()
WAMsetpoints.Insert( WAMqinit )
WAMsetpoints.Latch()

print 'ENTER to exit'
stdin.readline()

StopTasks()
