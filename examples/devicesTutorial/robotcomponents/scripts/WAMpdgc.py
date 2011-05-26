from RobotComponents import *

cmnLogger.SetMask( CMN_LOG_ALLOW_ALL )
cmnLogger.SetMaskFunction( CMN_LOG_ALLOW_ALL )
cmnLogger.SetMaskDefaultLog( CMN_LOG_ALLOW_ALL )

taskManager = mtsManagerLocal.GetInstance()

rtcan = devRTSocketCAN( 'rtcan0', devCAN.RATE_1000 )

WAMqinit = array( [ 0.0, -pi/2, 0.0, pi, 0.0, 0.0, 0.0 ] )
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

StartTasks()

print 'ENTER to enable'
stdin.readline()
PDGC.EnableCommand()
WAMtrajectory.EnableCommand()

print 'ENTER to move'
stdin.readline()
WAMdq = array( [ pi/4, pi/4, pi/4, -pi/4, pi/4, pi/4, 0.0 ] )
WAMq = WAMqinit + WAMdq
WAMsetpoints.Insert( WAMq )
WAMsetpoints.Latch()

print 'ENTER to home'
stdin.readline()
WAMsetpoints.Insert( WAMqinit )
WAMsetpoints.Latch()

print 'ENTER to exit'
stdin.readline()

StopTasks()
