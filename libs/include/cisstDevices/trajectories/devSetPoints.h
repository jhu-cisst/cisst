

#include <cisstCommon.h>
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctQuaternionRotation3.h>
#include <cisstVector/vctMatrixRotation3.h>
#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstDevices/devExport.h>

#ifndef _devSetPoints_h
#define _devSetPoints_h

class CISST_EXPORT devSetPoints : public mtsTaskContinuous {

 private:

  std::vector< vctDynamicVector<double> > rnsetpoints;
  std::vector< vctDynamicVector<double> > r3setpoints;
  std::vector< vctDynamicVector<double> > so3setpoints;
  size_t i;

  mtsVector<double> rnoutput;
  mtsVector<double> r3output;
  mtsVector<double> so3output;

  
  struct EnableCommand{
    mtsFunctionWrite function;
    bool status;
  };

  std::vector<EnableCommand*> WriteEnable;
    

 public:

  devSetPoints( const std::string& taskname,
		const std::vector< vctDynamicVector<double> >& setpoints ) :
    mtsTaskContinuous( taskname ), 
    rnsetpoints( setpoints ),
    i(1){

    // The manipulator provides an input interface
    mtsProvidedInterface* interface;
    interface = AddProvidedInterface( devSetPoints::OutputInterface );

    if( interface && 0 < setpoints.size() ){
      rnoutput.SetSize( setpoints[0].size() );
      rnoutput = setpoints[0];
      StateTable.AddData( rnoutput, "RnOutput" );
      interface->AddCommandReadState( StateTable, rnoutput, "GetRnSetpoint" );
    }
    
  }

  void AddEnable( const std::string& interface, 
		  const std::string& command,
		  bool status ){
    EnableCommand* encmd = new EnableCommand;
    encmd->status = status;
    WriteEnable.push_back( encmd );
    mtsRequiredInterface* requiredInterface;
    requiredInterface = this->AddRequiredInterface( interface );
    requiredInterface->AddFunction( command, encmd->function );
  }

  devSetPoints( const std::string& taskname,
		const std::vector< vctFrame4x4<double> >& setpoints ) :
    mtsTaskContinuous( taskname ), 
    i(1){

    for( size_t i=0; i<setpoints.size(); i++ ){

     vctFrame4x4<double> Rt = setpoints[i];
      vctFixedSizeVector<double,3> t = Rt.Translation();
      vctMatrixRotation3<double> R( Rt[0][0], Rt[0][1], Rt[0][2],
				    Rt[1][0], Rt[1][1], Rt[1][2],
				    Rt[2][0], Rt[2][1], Rt[2][2],
				    VCT_NORMALIZE );
      vctQuaternionRotation3<double> q( R );
      r3setpoints.push_back( vctDynamicVector<double>( 3, t[0], t[1], t[2] ) );
      so3setpoints.push_back(vctDynamicVector<double>(4, q.X(),q.Y(),q.Z(),q.R()));

    }


    // The manipulator provides an input interface
    mtsProvidedInterface* interface;
    interface = AddProvidedInterface( devSetPoints::OutputInterface );

    if( interface && 0 < setpoints.size() ){
      r3output.SetSize( 3 );
      so3output.SetSize( 4 );
      r3output = r3setpoints[0];
      so3output = so3setpoints[0];
      StateTable.AddData( r3output, "R3Output" );
      StateTable.AddData( so3output, "SO3Output" );
      interface->AddCommandReadState( StateTable, r3output, "GetR3Setpoint" );
      interface->AddCommandReadState( StateTable, so3output, "GetSO3Setpoint" );
    }
    
  }

  ~devSetPoints(void) {};

  void Configure(const std::string & CMN_UNUSED(filename) = "") {};

  void Startup(void){};

  void Run(void){
    char c = cmnGetChar();
    if( c == 'n' ){
      if( i < rnsetpoints.size() )
	rnoutput = rnsetpoints[i];
      if( i < r3setpoints.size() )
	r3output = r3setpoints[i];
      if( i < so3setpoints.size() )
	so3output = so3setpoints[i];
      i++;
    }
    if( c == 'e' ){

      for( size_t i=0; i<WriteEnable.size(); i++ ){
	WriteEnable[i]->status = !WriteEnable[i]->status;
	mtsBool status = WriteEnable[i]->status;
	WriteEnable[i]->function( status );
      }

    }

  }

  void Cleanup(void) {};
  
  static const std::string OutputInterface;

};

const std::string devSetPoints::OutputInterface = "SetpointsOutput";

#endif
