from cisstCommonPython    import *;
from cisstMultiTaskPython import *;
from cisstVectorPython    import *;
from cisstDevicesPython   import *;

def StartTasks():
    # Get the task manager
    taskManager = mtsManagerLocal.GetInstance();
    # Create and start all the thread
    taskManager.CreateAll();
    taskManager.StartAll();

def StopTasks():
    # Get the task manager
    taskManager = mtsManagerLocal.GetInstance();
    # Stop everything
    taskManager.KillAll()
    taskManager.Cleanup()

def TrajectoryJoints( name, qinit, qdmax, period = 0.01 ):
    trajectory = devLinearRn( name,
                              period,
                              devRobotComponent.ENABLED,
                              OSA_CPU3,
                              devTrajectory.QUEUE,
                              devLinearRn.POSITION,
                              qinit,
                              qdmax );
    # Get the task manager
    taskManager = mtsManagerLocal.GetInstance()
    # Add he camera task
    taskManager.AddComponent( trajectory )
    return trajectory


def SetPoints( name, qinit ):
    # Create setpoints
    setpoints = devSetPoints( name, qinit )
    # Get the task manager
    taskManager = mtsManagerLocal.GetInstance()
    # Add he camera task
    taskManager.AddComponent( setpoints )
    return setpoints

