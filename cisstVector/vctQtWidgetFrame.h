/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2013-04-20

  (C) Copyright 2013-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _vctQtWidgetFrame_h
#define _vctQtWidgetFrame_h

// cisst include
#include <cisstVector/vctForwardDeclarations.h>
#include <cisstVector/vctForwardDeclarationsQt.h>
#include <cisstVector/vctTransformationTypes.h>
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstVector/vctQtWidgetRotation.h>
#include <cisstVector/vctQtWidgetDynamicVector.h>

#include <QWidget>

class QVBoxLayout;


// Always include last
#include <cisstVector/vctExportQt.h>

/*! Qt Widget to display a 3D frame using cisstVector. */
class CISST_EXPORT vctQtWidgetFrameDoubleRead: public QWidget
{
    Q_OBJECT;

 public:

    /*! Define the display mode type based on the rotation display
      mode. */
    typedef vctQtWidgetRotationDoubleRead RotationWidgetType;
    typedef RotationWidgetType::DisplayModeType DisplayModeType;

    /*! Constructor.  Default display mode is rotation matrix.  See
      also SetDisplayMode. */
    vctQtWidgetFrameDoubleRead(const DisplayModeType displayMode = RotationWidgetType::MATRIX_WIDGET);

    inline ~vctQtWidgetFrameDoubleRead(void) {};

    /*! Set the rotation value to be displayed.  This method assumes
      the rotation matrix is valid, i.e. normalized and will nor
      perform any check nor normalization. */
    template <class _rotationType>
    void SetValue(const vctFrameBase<_rotationType> & frame) {
        vctMatRot3 rotationMatrix;
        rotationMatrix.FromNormalized(frame.Rotation());
        this->RotationWidget->SetValue(rotationMatrix);
        vctDoubleVec translation(frame.Translation());
        translation.Multiply(mPrismaticFactor);
        this->TranslationWidget->SetValue(translation);
    }

    /*! Set the display mode.  This method applies the display mode to
      the rotation widget. */
    inline void SetDisplayMode(const DisplayModeType displayMode) {
        this->RotationWidget->SetDisplayMode(displayMode);
    }

    inline void SetPrismaticRevoluteFactors(const double & prismatic, const double & revolute) {
        mPrismaticFactor = prismatic;
        RotationWidget->SetPrismaticRevoluteFactors(prismatic, revolute);
    }

 protected:
    // widgets
    vctQtWidgetRotationDoubleRead * RotationWidget;
    vctQtWidgetDynamicVectorDoubleRead * TranslationWidget;
    // layout
    QVBoxLayout * Layout;
    // conversion factors
    double mPrismaticFactor;
};

#endif // _vctQtWidgetFrame_h
