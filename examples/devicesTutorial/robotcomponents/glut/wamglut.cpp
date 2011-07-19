#include <cisstDevices/robotcomponents/trajectories/devSetPoints.h>
#include <cisstDevices/robotcomponents/trajectories/devQLQRn.h>
#include <cisstDevices/robotcomponents/glut/devGLUTManipulator.h>
#include <cisstDevices/robotcomponents/glut/devGLUT.h>
#include <cisstDevices/devKeyboard.h>
#include <cisstMultiTask/mtsTaskManager.h>


int main( int argc, char** argv ){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  devGLUT glut(argc, argv);

  devKeyboard keyboard;
  keyboard.SetQuitKey('q');
  keyboard.AddKeyWriteFunction( 'n', "next", devSetPoints::NextSetPoint, true );
  taskManager->AddComponent( &keyboard );

  vctDynamicVector<double> qinit( 7, 0.0 );
  vctDynamicVector<double> qfinal( 7, 1.0 );

  devSetPoints setpoints( "setpoints", 7 );
  setpoints.Insert( qfinal );
  setpoints.Insert( qinit );
  taskManager->AddComponent(&setpoints);
  
  vctDynamicVector<double> qdmax( 7, 0.1 );
  vctDynamicVector<double> qddmax( 7, 0.05 );
  devQLQRn trajectory( "trajectory", 
		       0.01, 
		       devTrajectory::ENABLED,
		       OSA_CPUANY,
		       devTrajectory::QUEUE,
		       devTrajectory::POSITION,
		       qinit, 
		       qdmax,
		       qddmax );
  taskManager->AddComponent(&trajectory);

  std::string path("libs/etc/cisstRobot/WAM/");
  std::vector<std::string> links;
  links.push_back( path + "l1.obj" );
  links.push_back( path + "l2.obj" );
  links.push_back( path + "l3.obj" );
  links.push_back( path + "l4.obj" );
  links.push_back( path + "l5.obj" );
  links.push_back( path + "l6.obj" );
  links.push_back( path + "l7.obj" );


  devGLUTManipulator WAM("WAM", 
			 0.03,
			 devManipulator::ENABLED,
			 OSA_CPUANY,
			 path + "wam7.rob",
			 vctFrame4x4<double>(),
			 vctDynamicVector<double>( 7, 0.0 ),
			 links,
			 path + "l0.obj" );
  taskManager->AddComponent(&WAM);


  if (!taskManager->Connect( keyboard.GetName(),  "next",
                             setpoints.GetName(), devSetPoints::Control ) )
    {
      std::cerr << "Connect failed: " 
		<< keyboard.GetName() << ":" << "next"
		<< " - "
		<< setpoints.GetName() << ":" << devManipulator::Input
		<< std::endl;
      return 1;
    }


  if (!taskManager->Connect( trajectory.GetName(), devTrajectory::Input,
                             setpoints.GetName(),  devSetPoints::OutputRn ) )
    {
      std::cerr << "Connect failed: " 
		<< trajectory.GetName() << ":" << devTrajectory::Input
		<< " - "
		<< setpoints.GetName() << ":" << devManipulator::Input
		<< std::endl;
      return 1;
    }

  if (!taskManager->Connect( trajectory.GetName(), devTrajectory::Output,
                             WAM.GetName(),    devManipulator::Input ))
    {
      std::cerr << "Connect failed: " 
		<< trajectory.GetName() << ":" << "Output"
		<< " - "
		<< WAM.GetName() << ":" << devManipulator::Input
		<< std::endl;
      return 1;
    }


  taskManager->CreateAll();
  taskManager->StartAll();

  std::cout << "Type 'n' to move to the final position" << std::endl;
  std::cout << "Type 'n' again to move to the initial position" << std::endl;

  glutMainLoop();

  return 0;

}
