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

StartTasks()

stdin.readline()

StopTasks()
