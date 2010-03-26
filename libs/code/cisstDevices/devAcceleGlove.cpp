#include <string.h>
#include <numeric>

#include <cisstVector/vctMatrixRotation3.h>
#include <cisstVector/vctAxisAngleRotation3.h>
#include <cisstDevices/devAcceleGlove.h>
#include <cisstCommon/cmnLogger.h>
using namespace std;

CMN_IMPLEMENT_SERVICES(devAcceleGlove);

const std::string devAcceleGlove::PalmInterface   = "PalmInterface";
const std::string devAcceleGlove::ThumbInterface  = "ThumbInterface";
const std::string devAcceleGlove::FirstInterface  = "FirstInterface";
const std::string devAcceleGlove::MiddleInterface = "MiddleInterface";
const std::string devAcceleGlove::RingInterface   = "RingInterface";
//const std::string devAcceleGlove::RingInterfaceName   = "RingInterface";

const std::string devAcceleGlove::GetPalmPosition = "GetPalmPosition";
const std::string devAcceleGlove::GetThumbAngles  = "GetThumbAngles";
const std::string devAcceleGlove::GetFirstAngles  = "GetFirstAngles";
const std::string devAcceleGlove::GetMiddleAngles = "GetMiddleAngles";
const std::string devAcceleGlove::GetRingAngles   = "GetRingAngles";
//const std::string devAcceleGlove::SetRingPositionCmd   = "SetRingPosition";

devAcceleGlove::devAcceleGlove( const std::string& taskname,
				double period ) : 

  // configure the task
  mtsTaskPeriodic( taskname, period, true ) {

  pthumb[0][0] = -0.0774; pthumb[0][1] = 26.2460;
  pthumb[1][0] = -0.0793; pthumb[1][1] = 24.8530;
  pthumb[2][0] = -0.0775; pthumb[2][1] = 26.5338;

  pfirst[0][0] = -0.0772; pfirst[0][1] = 24.2037;
  pfirst[1][0] = -0.0765; pfirst[1][1] = 24.9017;
  pfirst[2][0] = -0.0755; pfirst[2][1] = 24.9794;

  pmiddle[0][0] = -0.0563; pmiddle[0][1] = 18.7767;
  pmiddle[1][0] = -0.0589; pmiddle[1][1] = 18.9264;
  pmiddle[2][0] = -0.0543; pmiddle[2][1] = 18.4945;

  pring[0][0] = -0.0788; pring[0][1] = 27.3828;
  pring[1][0] = -0.0798; pring[1][1] = 27.0540;
  pring[2][0] = -0.0777; pring[2][1] = 26.1236;

  ppinky[0][0] = -0.0709; ppinky[0][1] = 24.1443;
  ppinky[1][0] = -0.0770; ppinky[1][1] = 23.0401;
  ppinky[2][0] = -0.0767; ppinky[2][1] = 45.8832;

  ppalm[0][0] =  0.0780; ppalm[0][1] = -27.5727;
  ppalm[1][0] = -0.0771; ppalm[1][1] =  25.6719;
  ppalm[2][0] =  0.0781; ppalm[2][1] = -26.3727;



  mtsProvidedInterface* interface;

  // Create and configure the palm interface
  interface = AddProvidedInterface( devAcceleGlove::PalmInterface );
  if( interface ){
    palmposition.SetSize( 3 );
    palmposition.SetAll( 0.0 );
    StateTable.AddData( palmposition, "PalmPosition" );
    interface->AddCommandReadState( StateTable, 
				    palmposition,
				    devAcceleGlove::GetPalmPosition );
  }
  else{
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << ": Failed to create the provided interface for "
		       << devAcceleGlove::PalmInterface
		       << std::endl;
    exit(-1);
  }

  // Create and configure the thumb interface
  interface = AddProvidedInterface( devAcceleGlove::ThumbInterface );
  if( interface ){
    thumbangles.SetSize( 4 );
    thumbangles.SetAll( 0.0 );
    StateTable.AddData( thumbangles, "ThumbAngles" );
    interface->AddCommandReadState( StateTable, 
				    thumbangles,
				    devAcceleGlove::GetThumbAngles );
  }
  else{
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << ": Failed to create the provided interface for "
		       << devAcceleGlove::ThumbInterface
		       << std::endl;
    exit(-1);
  }

  // Create and configure the first finger interface
  interface = AddProvidedInterface( devAcceleGlove::FirstInterface );
  if( interface ){
    firstangles.SetSize( 4 );
    firstangles.SetAll( 0.0 );
    StateTable.AddData( firstangles, "FirstAngles" );
    interface->AddCommandReadState( StateTable, 
				    firstangles,
				    devAcceleGlove::GetFirstAngles );
  }
  else{
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << ": Failed to create the provided interface for "
		       << devAcceleGlove::FirstInterface
		       << std::endl;
    exit(-1);
  }

  // Create and configure the middle finger interface
  interface = AddProvidedInterface( devAcceleGlove::MiddleInterface );
  if( interface ){
    middleangles.SetSize( 4 );
    middleangles.SetAll( 0.0 );
    StateTable.AddData( middleangles, "MiddleAngles" );
    interface->AddCommandReadState( StateTable, 
				    middleangles,
				    devAcceleGlove::GetMiddleAngles );
  }
  else{
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << ": Failed to create the provided interface for "
		       << devAcceleGlove::MiddleInterface
		       << std::endl;
    exit(-1);
  }

  // Create and configure the ring finger interface
  interface = AddProvidedInterface( devAcceleGlove::RingInterface );
  if( interface ){
    ringangles.SetSize( 4 );
    ringangles.SetAll( 0.0 );
    StateTable.AddData( ringangles, "RingAngles" );
    interface->AddCommandReadState( StateTable, 
				    ringangles,
				    devAcceleGlove::GetRingAngles );
  }
  else{
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << ": Failed to create the provided interface for "
		       << devAcceleGlove::RingInterface
		       << std::endl;
    exit(-1);
  }

}

devAcceleGlove::~devAcceleGlove()
{  serialport.Close();  }

void devAcceleGlove::Configure( const std::string& devname ){

  serialport.SetPortName( devname );
  serialport.SetBaudRate( osaSerialPort::BaudRate38400 );
  serialport.SetCharacterSize( osaSerialPort::CharacterSize8 );
  serialport.SetParityChecking( osaSerialPort::ParityCheckingNone );
  serialport.SetStopBits( osaSerialPort::StopBitsOne );
  serialport.SetFlowControl( osaSerialPort::FlowControlNone );

  serialport.Open();

}

void devAcceleGlove::Startup(){}
void devAcceleGlove::Cleanup(){}

void devAcceleGlove::Run(){

  // send a binary raaw query
  char w[] = "w";
  serialport.Write( w, 1 );
  
  // collect the 44 bytes answer
  char buffer[128];
  memset((void*)buffer, 0, 128);
  usleep(80000);
  serialport.Read( (char*)buffer, 128 );

  int thumbsensor[3];
  int firstsensor[3];
  int middlesensor[3];
  int ringsensor[3];
  int pinkysensor[3];
  int palmsensor[3];

  if( sscanf( buffer, 
	      "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
	      &thumbsensor[0],  &thumbsensor[1],  &thumbsensor[2], 
	      &firstsensor[0],  &firstsensor[1],  &firstsensor[2], 
	      &middlesensor[0], &middlesensor[1], &middlesensor[2], 
	      &ringsensor[0],   &ringsensor[1],   &ringsensor[2], 
	      &pinkysensor[0],  &pinkysensor[1],  &pinkysensor[2], 
	      &palmsensor[0],   &palmsensor[1],   &palmsensor[2] )
      != 18 ){

    std::cout << "ERROR" << std::endl;
  }

  vctFixedSizeVector<double,3> palmaccel;
  palmaccel[0] = palmsensor[0]*ppalm[0][0] + ppalm[0][1];
  palmaccel[1] = palmsensor[1]*ppalm[1][0] + ppalm[1][1];
  palmaccel[2] = palmsensor[2]*ppalm[2][0] + ppalm[2][1];
 
  vctFixedSizeVector<double,3> thumbaccel;
  vctFixedSizeVector<double,3> firstaccel;
  vctFixedSizeVector<double,3> middleaccel;
  vctFixedSizeVector<double,3> ringaccel;
  vctFixedSizeVector<double,3> pinkyaccel;

  // get some calibrated meaurements

  thumbaccel[0] = thumbsensor[0]*pthumb[0][0] + pthumb[0][1];
  thumbaccel[1] = thumbsensor[1]*pthumb[1][0] + pthumb[1][1];
  thumbaccel[2] = thumbsensor[2]*pthumb[2][0] + pthumb[2][1];

  firstaccel[0] = firstsensor[0]*pfirst[0][0] + pfirst[0][1];
  firstaccel[1] = firstsensor[1]*pfirst[1][0] + pfirst[1][1];
  firstaccel[2] = firstsensor[2]*pfirst[2][0] + pfirst[2][1];

  middleaccel[0] = middlesensor[0]*pmiddle[0][0] + pmiddle[0][1];
  middleaccel[1] = middlesensor[1]*pmiddle[1][0] + pmiddle[1][1];
  middleaccel[2] = middlesensor[2]*pmiddle[2][0] + pmiddle[2][1];

  ringaccel[0] = ringsensor[0]*pring[0][0] + pring[0][1];
  ringaccel[1] = ringsensor[1]*pring[1][0] + pring[1][1];
  ringaccel[2] = ringsensor[2]*pring[2][0] + pring[2][1];

  pinkyaccel[0] = pinkysensor[0]*ppinky[0][0] + ppinky[0][1];
  pinkyaccel[1] = pinkysensor[1]*ppinky[1][0] + ppinky[1][1];
  pinkyaccel[2] = pinkysensor[2]*ppinky[2][0] + ppinky[2][1];

  // normalize all the vectors (we only need orientation not magnitudes)
  palmaccel.NormalizedSelf();
  thumbaccel.NormalizedSelf();
  firstaccel.NormalizedSelf();
  middleaccel.NormalizedSelf();
  ringaccel.NormalizedSelf();
  pinkyaccel.NormalizedSelf();

  // this is to average out readings
  tangles.push_back( acos( thumbaccel.DotProduct(palmaccel) ) );
  fangles.push_back( acos( firstaccel.DotProduct(palmaccel) ) );
  mangles.push_back( acos( middleaccel.DotProduct(palmaccel) ) );
  rangles.push_back( acos( ringaccel.DotProduct(palmaccel) ) );
  pangles.push_back( acos( pinkyaccel.DotProduct(palmaccel) ) );
  
  // average the last 6 measurements
  double tangle, fangle, mangle, rangle, pangle;
  tangle = std::accumulate( tangles.begin(),tangles.end(),0.0 )/tangles.size();
  fangle = std::accumulate( fangles.begin(),fangles.end(),0.0 )/fangles.size();
  mangle = std::accumulate( mangles.begin(),mangles.end(),0.0 )/mangles.size();
  rangle = std::accumulate( rangles.begin(),rangles.end(),0.0 )/rangles.size();
  pangle = std::accumulate( pangles.begin(),pangles.end(),0.0 )/pangles.size();

  // only keep 5 measurements in the queue
  if( 5 < tangles.size() ) tangles.pop_front();
  if( 5 < fangles.size() ) fangles.pop_front();
  if( 5 < mangles.size() ) mangles.pop_front();
  if( 5 < rangles.size() ) rangles.pop_front();
  if( 5 < pangles.size() ) pangles.pop_front();

  // the cross product between the thumb and the palm
  vctFixedSizeVector<double,3> tpaxis;
  vctFixedSizeVector<double,3> fpaxis;
  vctFixedSizeVector<double,3> mpaxis;
  vctFixedSizeVector<double,3> rpaxis;
  vctFixedSizeVector<double,3> ppaxis;

  tpaxis.CrossProductOf( thumbaccel,  palmaccel );
  fpaxis.CrossProductOf( firstaccel,  palmaccel );
  mpaxis.CrossProductOf( middleaccel,  palmaccel );
  rpaxis.CrossProductOf( ringaccel,  palmaccel );
  ppaxis.CrossProductOf( pinkyaccel,  palmaccel );

  double tx, tz, fx, fz, mx, mz, rx, rz, px, pz;

  RxRz( tpaxis, tangle, tx, tz );
  RxRz( fpaxis, fangle, fx, fz );
  RxRz( mpaxis, mangle, mx, mz );
  RxRz( rpaxis, rangle, rx, rz );
  RxRz( ppaxis, pangle, px, pz );

  
}

void devAcceleGlove::RxRz( const vctFixedSizeVector<double,3>& axis, 
			   double angle,
			   double& rx, 
			   double& rz ){
  
  vctAxisAngleRotation3<double> r( axis, angle,  VCT_NORMALIZE );
  vctMatrixRotation3<double> R(r);
  rx = acos( R[2][2] ) * 180.0 / M_PI;
  rz = acos( R[0][0] ) * 180.0 / M_PI;

}
