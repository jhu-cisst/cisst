/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Praneeth Sadda, Anton Deguet
  Created on: 2011-11-11

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _mtsQtWidgetFactory_h
#define _mtsQtWidgetFactory_h

#include <typeinfo>
#include <map>

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

// Qt forward declarations
class QWidget;

// mts Qt forward declaration
class mtsQtWidgetGenericObjectRead;
class mtsQtWidgetGenericObjectWrite;


class mtsQtTypeInfoComparator
{
public:
    inline bool operator()(const std::type_info* a, const std::type_info* b) const {
        // Visual Studio (some versions) have "int before()"
        return (a->before(*b)) == 0 ? false : true;
    }
};

class CISST_EXPORT mtsQtWidgetFactory
{
    typedef mtsQtWidgetGenericObjectRead * (*WidgetReadCreatorFunction)(void);
    typedef mtsQtWidgetGenericObjectWrite * (*WidgetWriteCreatorFunction)(void);

    friend class WidgetReadCreator;
    friend class WidgetWriteCreator;

 public:
    class WidgetReadCreator
    {
    private:
        WidgetReadCreator(void);
    public:
        WidgetReadCreator(const std::type_info * type, WidgetReadCreatorFunction function);
    };

    class WidgetWriteCreator
    {
    private:
        WidgetWriteCreator(void);
    public:
        WidgetWriteCreator(const std::type_info * type, WidgetWriteCreatorFunction function);
    };

    typedef std::map<const std::type_info *, WidgetReadCreatorFunction, mtsQtTypeInfoComparator> WidgetReadCreatorMap;
    typedef std::map<const std::type_info *, WidgetWriteCreatorFunction, mtsQtTypeInfoComparator> WidgetWriteCreatorMap;

    static mtsQtWidgetGenericObjectRead * CreateWidgetRead(const std::type_info * type);
    static mtsQtWidgetGenericObjectWrite * CreateWidgetWrite(const std::type_info * type);


 protected:
    static WidgetReadCreatorMap & GetWidgetReadCreators(void);
    static WidgetWriteCreatorMap & GetWidgetWriteCreators(void);
};

#endif // _mtsQtWidgetFactory_h
