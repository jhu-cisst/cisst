/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s):  Anton Deguet
  Created on: 2010-09-06

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _cdgData_h
#define _cdgData_h

#include <iostream>
#include <vector>
#include <cisstCommon/cmnTokenizer.h>

#include "cdgDataMember.h"
#include "cdgTypedef.h"

/* example:

include <cisstVector/vctFixedSizeTypes.h>
include <iostream>

typename newType

typedef vctFixedSizeVector<vctFixedSizeVector<3, double>, 3> MatrixType;
typedef IndexType Index

member double Sine Sine
member vctDouble3 Position End effector
member std::string Name Human readable name
member IndexType Index

 */

class cdgData {

    /*! Name of the data type to be created. */
    std::string Name;

    /*! Default Log LoD for the class generated.  See also
      cmnGenericObject.  Corresponds to "default-log-lod" in cisst
      data description file.  If not found, the default is
      CMN_LOG_LOD_RUN_ERROR. */ 
    std::string DefaultLogLoD;

    /*! List of header files to include.  Corresponds to keyword
      "include" in cisst data description file. */
    typedef std::vector<std::string> IncludesType;
    IncludesType Includes;

    /*! List of typedefs.  Corresponds to keyword "typedef" in cisst
      data description file.  Typedef are reproduced in the
      generated code and can be used to defined types with spaced in
      their names (e.g "unsigned int", "std::vector<std::vector<int>
      >"). */
    //@{
    typedef std::vector<cdgTypedef> TypedefsType;
    TypedefsType Typedefs;
    //@}
    
    /*! List of data members.  Corresponds to keyword "member" in
      cisst data description file. */
    typedef std::vector<cdgDataMember> MembersType;
    MembersType DataMembers;

public:
    void ParseFile(std::ifstream & input, const std::string & filename);

    void GenerateHeader(std::ostream & output) const;

    void GenerateCode(std::ostream & output, const std::string & header) const;

protected:
    bool StripComments(std::istream & input, std::ostream & content,
                       size_t & errorLine, std::string & errorMessage);

    void GenerateStandardMethodsHeader(std::ostream & output) const;

    void GenerateSerializeRawCode(std::ostream & output) const;

    void GenerateDeSerializeRawCode(std::ostream & output) const;

    void GenerateToStreamCode(std::ostream & output) const;

    void GenerateToStreamRawCode(std::ostream & output) const;
};

#endif // _cdgData_h
