/*

  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _robMassBase_h
#define _robMassBase_h

#include <iostream>

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctFrame4x4.h>

class robMassBase{
protected:

  //! Parallel Axis Theorem
  /**
     Finds the moment of inertia wrt to a parallel axis
  */
  vctFixedSizeMatrix<double,3,3> 
  ParallelAxis( double m, 
		const vctFixedSizeVector<double,3>& t, 
		const vctFixedSizeMatrix<double,3,3,VCT_ROW_MAJOR>& I ) const {

    // inner product
    double tTt = t[0]*t[0] + t[1]*t[1] + t[2]*t[2];

    // outer product;
    vctFixedSizeMatrix<double,3,3> ttT( t[0]*t[0], t[0]*t[1], t[0]*t[2],
					t[1]*t[0], t[1]*t[1], t[1]*t[2],
					t[2]*t[0], t[2]*t[1], t[2]*t[2] );

    // compute the offset It
    vctFixedSizeMatrix<double,3,3,VCT_ROW_MAJOR> It;
    It = m* ( tTt * vctFixedSizeMatrix<double,3,3,VCT_ROW_MAJOR>::Eye() - ttT );

    return I + It;
  }

public:

  robMassBase(){}
  ~robMassBase(){}

  //! Return the mass
  virtual double Mass() const = 0;
  
  //! Return the center of mass
  virtual vctFixedSizeVector<double,3> CenterOfMass() const = 0;

  //! Return the moment of inertia tensor
  virtual vctFixedSizeMatrix<double,3,3> MomentOfInertia() const = 0;

  //! Read the mass from a input stream
  virtual robError Read( std::istream& is ){
    double m;                                  // the mass
    double comx, comy, comz;                   // center of mass
    double d1, d2, d3;                         // principal moments of inertia
    double x1, x2, x3, y1, y2, y3, z1, z2, z3; // principal axes

    is >> m                                    // read everything
       >> comx >> comy >> comz 
       >> d1 >> d2 >> d3
       >> x1 >> x2 >> x3
       >> y1 >> y2 >> y3
       >> z1 >> z2 >> z3;

    if( d1 < 0.0 || d2 < 0.0 || d3 < 0.0 ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
			<< ": Principal moments of inertia must be non-negative"
			<< std::endl;
      return ERROR;
    }
    
    // build the structures
    vctFixedSizeVector<double,3>   com(comx, comy, comz);
    vctFixedSizeVector<double,3>   d(d1, d2, d3);
    vctDynamicVector<double> e1(3, x1, x2, x3);
    vctDynamicVector<double> e2(3, y1, y2, y3);
    vctDynamicVector<double> e3(3, z1, z2, z3);
    vctMatrixRotation3<double,VCT_ROW_MAJOR> V( e1, e2, e3, true, true );

    // set the masses
    SetDynamicsParameters( m, com, d, V );
    return SUCCESS;
  }
  
  //! Set the mass parameters
  /**
     \param mass The mass of the body
     \param COM The center of gravity
     \param MOIT The moment of inertia tensor
  */
  virtual void SetDynamicsParameters( double mass, 
				      const vctFixedSizeVector<double,3>& COM,
				      const vctFixedSizeVector<double,3>& d,
				      const vctMatrixRotation3<double>& V ) = 0;

  //! Write the mass to an output stream
  virtual void Write( std::ostream& os ) const {
    double m = Mass();
    vctFixedSizeVector<double,3> com = CenterOfMass();
    vctFixedSizeMatrix<double,3,3> moi = MomentOfInertia();
    os << std::setw(13) << m 
       << std::setw(13) << com[0] 
       << std::setw(13) << com[1] 
       << std::setw(13) << com[2] 
       << std::setw(13) << moi[0][0] 
       << std::setw(13) << moi[1][1] 
       << std::setw(13) << moi[2][2] 
       << std::setw(13) << moi[0][1] 
       << std::setw(13) << moi[1][2] 
       << std::setw(13) << moi[0][2];
  }

};

#endif
