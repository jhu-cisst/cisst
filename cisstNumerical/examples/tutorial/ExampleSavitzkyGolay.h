
void ExampleSavitzkyGolay() {

  int N = 1024;

  vctDynamicVector<double> data(N, 0.0 );     // y(t)
  vctDynamicVector<double> datad(N, 0.0 );    // y'(t)
  vctDynamicVector<double> datadd(N, 0.0 );   // y''(t)
  vctDynamicVector<double> dataddd(N, 0.0 );  // y'''(t)

  // these are the 5 polynomials coefficients
  double a4 = 0.1324, a3 = -0.4025, a2 = -0.2215, a1 = 0.0469, a0 = 0.4575;

  // time
  double t = 0.0;
  double dt = 0.01;

  // generate the data and the derivatis
  for( int i=0; i<N; i++, t+=dt ){
    data[i]     = a0 + a1*t + 1.0*a2*t*t + 1.0*a3*t*t*t +  1.0*a4*t*t*t*t;
    datad[i]    =      a1   + 2.0*a2*t   + 3.0*a3*t*t   +  4.0*a4*t*t*t;
    datadd[i]   =             2.0*a2     + 6.0*a3*t     + 12.0*a4*t*t;
    dataddd[i]  =                        + 6.0*a3       + 24.0*a4*t;
  }


  // generate a causal SG mask.
  // Polynomial degree: 4
  // derivative: 0 (no derivative)
  // left samples: 6
  // right samples: 0 (causal filter)
  {
    int NL = 6;
    int NR = 0;
    vctDynamicVector<double> c = nmrSavitzkyGolay( 4, 0, NL, NR );
    vctDynamicVector<double> y(N, 0.0 );
    // convolution
    for( int i=NL; i<N-NR; i++ ){
      y[i] = ( data[i-6]*c[0] +
               data[i-5]*c[1] +
               data[i-4]*c[2] +
               data[i-3]*c[3] +
               data[i-2]*c[4] +
               data[i-1]*c[5] +
               data[ i ]*c[6] );
    }

    {
      double avg=0.0;
      for( int i=NL; i<N-NR; i++ )
        { avg+= fabs(data[i] - y[i]) / fabs( data[i] ); }
      std::cout << "Avg. relative error: " << avg / (N-6) << std::endl;
    }

  }

  // generate a causal SG mask.
  // Polynomial degree: 4
  // derivative: 1st
  // left samples: 3
  // right samples: 3 (symmetric filter)
  {
    int NL = 3;
    int NR = 3;
    vctDynamicVector<double> c = nmrSavitzkyGolay( 4, 1, NL, NR );
    vctDynamicVector<double> yd(N, 0.0 );
    // convolution
    for( int i=NL; i<N-NR; i++ ){
      yd[i] = ( data[i-3]*c[0] +
                data[i-2]*c[1] +
                data[i-1]*c[2] +
                data[ i ]*c[3] +
                data[i+1]*c[4] +
                data[i+2]*c[5] +
                data[i+3]*c[6] )/dt;
    }
    {
      double avg=0.0;
      for( int i=NL; i<N-NR; i++ )
        { avg+= fabs(datad[i] - yd[i]) / fabs( datad[i] ); }
      std::cout << "Avg. relative error: " << avg / (N-6) << std::endl;
    }

  }

  // generate a causal SG mask.
  // Polynomial degree: 4
  // derivative: 2nd
  // left samples: 6
  // right samples: 0 (causal filter)
  {
    int NL = 6;
    int NR = 0;
    vctDynamicVector<double> c = nmrSavitzkyGolay( 4, 2, NL, NR );
    vctDynamicVector<double> ydd(N, 0.0);
    // convolution
    for( int i=NL; i<N-NR; i++ ){
      ydd[i] = ( data[i-6]*c[0] +
                 data[i-5]*c[1] +
                 data[i-4]*c[2] +
                 data[i-3]*c[3] +
                 data[i-2]*c[4] +
                 data[i-1]*c[5] +
                 data[i-0]*c[6] )/(dt*dt);
    }
    {
      double avg=0.0;
      for( int i=NL; i<N-NR; i++ )
        { avg+= fabs(datadd[i] - ydd[i]) / fabs( datadd[i] ); }
      std::cout << "Avg. relative error: " << avg / (N-6) << std::endl;
    }

  }

  // generate a causal SG mask.
  // Polynomial degree: 4
  // derivative: 3rd
  // left samples: 3
  // right samples: 3 (causal filter)
  {
    int NL = 3;
    int NR = 3;
    vctDynamicVector<double> c = nmrSavitzkyGolay( 4, 3, NL, NR );
    vctDynamicVector<double> yddd(N, 0.0);
    // convolution
    for( int i=NL; i<N-NR; i++ ){
      yddd[i] = ( data[i-3]*c[0] +
                  data[i-2]*c[1] +
                  data[i-1]*c[2] +
                  data[ i ]*c[3] +
                  data[i+1]*c[4] +
                  data[i+2]*c[5] +
                  data[i+3]*c[6] )/(dt*dt*dt);
    }
    {
      double avg=0.0;
      for( int i=NL; i<N-NR; i++ )
        { avg+= fabs(dataddd[i] - yddd[i]) / fabs( dataddd[i] ); }
      std::cout << "Avg. relative error: " << avg / (N-6) << std::endl;
    }
  }

}
