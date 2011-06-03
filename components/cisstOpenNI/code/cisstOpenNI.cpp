#include <cisstOpenNI/cisstOpenNI.h>

cisstOpenNI::cisstOpenNI( const std::string& fname  ){

  xn::EnumerationErrors errors;
  XnStatus status = context.InitFromXmlFile( fname.data(), &errors );

  if( status == XN_STATUS_NO_NODE_PRESENT ){
    XnChar strError[1024];
    errors.ToString( strError, 1024 );
    std::cout << strError << std::endl;
    return;
  }

  else if( status != XN_STATUS_OK ){
    std::cout << "Failed to open " << fname << " " 
	      << xnGetStatusString( status ) << std::endl;
    return;
  }
  
  status = context.FindExistingNode( XN_NODE_TYPE_DEPTH, depthgenerator );
  status = context.FindExistingNode( XN_NODE_TYPE_IMAGE, rgbgenerator );
  
  status =depthgenerator.GetAlternativeViewPointCap().SetViewPoint(rgbgenerator);
  
  if( status != XN_STATUS_OK ){
    std::cout << "Failed to set viewpoint: "
	      << xnGetStatusString(status) << std::endl;
    return;
  }

  context.StartGeneratingAll();

}

vctDynamicMatrix<double> cisstOpenNI::GetDepthImage(){
  
  // Query the context
  context.WaitAndUpdateAll();

  // Get data
  xn::DepthMetaData depthMD;
  depthgenerator.GetMetaData( depthMD );
  const XnDepthPixel* pDepth = depthMD.Data();

  vctDynamicMatrix<double> depthimage( depthMD.YRes(), depthMD.XRes() );
  double* ptr = depthimage.Pointer();
  for( size_t i=0; i<depthMD.YRes()*depthMD.XRes(); i++ )
    { ptr[i] =  pDepth[i] / 1000.0; }

  return depthimage;

}

vctDynamicMatrix<double> cisstOpenNI::GetRangeData(){

  // Query the context
  context.WaitAndUpdateAll();

  // Get data
  xn::DepthMetaData depthMD;
  depthgenerator.GetMetaData( depthMD );

  // create arrays
  XnUInt32 cnt = depthMD.XRes()*depthMD.YRes();
  XnPoint3D* proj = NULL;
  XnPoint3D* wrld = NULL;
  try{ 
    proj = new XnPoint3D[ cnt ];
    wrld = new XnPoint3D[ cnt ];
  }
  catch( std::bad_alloc& ){}

  CMN_ASSERT( proj != NULL );
  CMN_ASSERT( wrld != NULL );

  // Create projective coordinates
  for( size_t i=0, x=0; x<depthMD.XRes(); x++ ){
    for( size_t y=0; y<depthMD.YRes(); i++, y++ ){
      proj[i].X = (XnFloat)x;
      proj[i].Y = (XnFloat)y;
      proj[i].Z = depthMD(x,y);
    }
  }

  // Convert projective to 3D
  XnStatus status = depthgenerator.ConvertProjectiveToRealWorld(cnt, proj, wrld);
  if( status != XN_STATUS_OK ){
    std::cout << "Failed to convert projective to world: "
	      << xnGetStatusString( status ) << std::endl;
  }

  // create matrix
  vctDynamicMatrix<double> rangedata( 3, cnt );
  for( size_t i=0; i<cnt; i++ ){
    rangedata[0][i] = -wrld[i].X/1000.0;
    rangedata[1][i] = wrld[i].Y/1000.0;
    rangedata[2][i] = wrld[i].Z/1000.0;
  }

  delete[] proj;
  delete[] wrld;

  return rangedata;
}

vctDynamicMatrix<unsigned char> cisstOpenNI::GetRGBImage(){

  // Query the context
  context.WaitAndUpdateAll();

  // Get data
  xn::ImageMetaData rgbMD;
  rgbgenerator.GetMetaData( rgbMD );

  // create image
  vctDynamicMatrix<unsigned char>  rgbimage( rgbMD.YRes(), rgbMD.XRes()*3 );
  memcpy( rgbimage.Pointer(), 
	  rgbMD.Data(), 
	  rgbMD.YRes()*rgbMD.XRes()*3*sizeof(unsigned char) );

  return rgbimage;
}

vctDynamicNArray<unsigned char,3> cisstOpenNI::GetRGBPlanarImage(){

  // Query the context
  context.WaitAndUpdateAll();

  // Get data
  context.WaitOneUpdateAll( rgbgenerator );
  xn::ImageMetaData rgbMD;
  rgbgenerator.GetMetaData( rgbMD );

  vctDynamicNArray<unsigned char, 3> rgbimage;
  rgbimage.SetSize( vctDynamicNArray<unsigned char, 3>::nsize_type( rgbMD.YRes(),
								    rgbMD.XRes(),
								    3 ) );
  memcpy( rgbimage.Pointer(), rgbMD.Data(), rgbMD.YRes()*rgbMD.XRes()*3 );

  return rgbimage;

}
