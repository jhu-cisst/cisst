#include <cisstDevices/manipulators/devManipulator.h>

#include <cisstMultiTask/mtsInterfaceProvided.h>


const std::string devManipulator::InputInterface  = "InputInterface";
const std::string devManipulator::Input           = "Input";

const std::string devManipulator::OutputInterface = "OutputInterface";
const std::string devManipulator::Output          = "Output";


devManipulator::devManipulator( const std::string& taskname, 
				double period,
				size_t N ) : 
  
  mtsTaskPeriodic( taskname, period, true ) {

  {
    // The manipulator provides an input interface
    mtsInterfaceProvided * interface;
    interface = AddInterfaceProvided( devManipulator::InputInterface );
    if( interface ){
      input.SetSize( N );
      input.SetAll( 0.0 );
      StateTable.AddData( input, "Input" );
      interface->AddCommandWriteState
	( StateTable, input, devManipulator::Input );
    }
  }
  
  {
    // The manipulator provides an output interface
    mtsInterfaceProvided * interface;
    interface = AddInterfaceProvided( devManipulator::OutputInterface );
    if( interface ){
      output.SetSize( N );
      output.SetAll( 0.0 );
      StateTable.AddData( output, "Output" );
      interface->AddCommandReadState
	( StateTable, output, devManipulator::Output );
    }
  }
  
}

void devManipulator::Run(){
  ProcessQueuedCommands();
  output = Read();
  Write( input );  
}
