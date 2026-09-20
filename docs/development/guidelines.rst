Naming conventions
==================

-  All libraries names start with the ``cisst`` prefix.

   -  All global symbols of a library start with the library specific symbol:

      -  ``cisstCommon`` uses ``cmn``
      -  ``cisstVector`` uses ``vct``
      -  ``cisstOSAbstraction`` uses ``osa``
      -  ``cisstNumerical`` uses ``nmr``
      -  ``cisstInteractive`` uses ``ire``
      -  ``cisstMultiTask`` uses ``mts``
      -  ``cisstParameterTypes`` uses ``prm``
      -  ``cisstStereoVision`` uses ``svl``

   -  Global classes and functions use the lower case library prefix (e.g. ``vctFixedSizeVector``) while constants, enums and macros use the upper case prefix (e.g. ``VCT_ROW_MAJOR``).

      -  One significant exception to that rule is for STL compatibility. Many methods and typedefs in ``cisstVector`` use lower case and underscores (e.g. typedef ``value_type``, method ``size()``).

   -  Within reason, there should be one class per file, one file per class.
   -  If a class or type (typedef, enum) is specific to a class, define it within the class scope (e.g. ``vctDynamicVector::index_type``). This reduces the number of symbols defined at the global level.
   -  All names should be as explicit as possible and avoid abbreviations as much as possible (e.g. ``mtsInterfaceRequired``, not ``mtsIntReq``)
   -  Class names use FirstLetterUpperCase as do methods and data members
   -  Parameters use firstLetterLowerCase
   -  We DO NOT use underscore ``_`` in names

      -  We have some rare exceptions, e.g. ``cmn_ms`` because ``ms`` as in milliseconds has to be lower case
      -  Underscore are used to prefix template parameters. For example ``template <class _elementType> ...``.

   -  For sub-templating, we use multiple underscores, i.e. the number of underscores represents the level of sub-templating. We have yet to see more than two levels.

Header files
============

-  All header files should have "header guards". As each class definition corresponds to a class name, the header guards should be ``_<className>_h``. For example, the class ``cmnPath`` is declared in the file ``cmnPath.h`` and used the guards ``#ifndef _cmnPath_h``. The guard's end at the end of file should include a comment to easily identify it, i.e. ``#endif // _cmnPath_h``
-  All ``#include`` related to *cisst* should use ``<>``, not ``""``. For example, one should use ``#include <cisstCommon/cmnGenericObject.h>``, not ``#include "cmnGenericObject.h"``. The only exception is for private header files which are found in the ``code`` directory.
-  To include the headers of a given library, one can use the convenient ``cisstXyz.h`` header (e.g. ``#include <cisstCommon.h>``). Programmers must be aware that these library headers (automatically generated) include all the library headers and hence greatly increase the compilation time. Therefore, the libraries we distribute should always use targeted headers, e.g. if the only type needed is ``cmnPath``, one should have ``#include <cisstCommon/cmnPath.h>``, NOT ``#include <cisstCommon.h>``.
-  As a convention, if a library needs forward declarations, one should create a ``xyzForwardDeclarations.h`` file (e.g. ``mtsForwardDeclarations.h`` for ``cisstMultiTask``). Forward declarations might be needed for circular dependencies but they can also greatly reduce compilation time.

Formatting
==========

-  The *cisst* libraries use Unix end of line. If you are working on a Unix computer, you can run ``dos2unix`` to convert to the right format. On a Mac, you can install dos2unix via macports.org.
-  Configure your text editor so that tabulations are replaced by spaces and tabulations are 4 spaces long. This is very important for code versioning. If the number of spaces is modified for each line, cvs/svn/git will consider that the whole file has been modified. How to configure ...

   -  Emacs: All files contain a header which tells Emacs to use 4 spaces as tabs. For newly created files, you can do M-x customize, then pick Programming followed by Languages and C. Select the option C Basic Offset, show the current value and set it to 4. To save, select state and Save for future sessions. To indent a region, C-M-q (i.e. esc, ctrl+q).
   -  Visual Studio 6: In the pop-down menu Tools, select Options and go to Tabs. Make sure the size is 4 and check Insert spaces.

-  Use ``/`` (slash) as a directory separator, not\ ``\`` (backslash). This applies for the CMake configuration files, the ``#include statements`` as well as a path in your code.
-  To increase the readability and ease the debugging (setting breakpoints), avoid multiple instructions on a single line, e.g.:

   .. code:: c++

      int firstIndex = myArray->GetFirstIndex();
      double value = otherArray[firstIndex];
      std::cout << "My Value" << value << std::endl;

   is better than:

   .. code:: c++

      int firstIndex = myArray->GetFirstIndex(); double value = otherArray[firstIndex]; std::cout << "My Value" << value << std::endl;

-  For short inline methods, the code can be inserted right after the method declaration. For longer ones (more than 2 or 3 lines), the code should be placed in the same header file right after the class definition. This helps when one needs to quickly see all the available methods of a class.
-  For templated code, short inline methods code should be with the method declaration and all other methods code should be at the end of the header file.
-  Use a space between operands, it is sometime necessary and it always helps. E.g.: ``i += *j``.
-  Use a space after a comma (``,``) or a semi colon (``;``), e.g.: ``for (i = 0; i > 10; i++)``, not ``for(i=0;i>10;i++)``.
-  Use a single space between for, while, switch or if and the following parenthesis.
-  If a method prototype or call is too long (i.e. approx 80 characters), try to split on multiple lines, e.g.:

   .. code:: c++

      cmnMyClassReturned
      cmnMyVeryLongClassName::ALongMethodName(const cmnOtherClass& object1,
                                              const cmnAnOtherVeryLongClassName& object2) const

C++ guidelines
==============

cisst guidelines
----------------

-  The cisst libraries provide a logging mechanism. Please use it instead of custom ``#if debug  std::cerr << "Error message" << std::endl``. See the class ``cmnLogger`` and macros ``CMN_LOG_XXX`` and ``CMN_LOG_CLASS_XXX``, where the ``XXX`` indicates the log message type (e.g., ``INIT_WARNING``, ``RUN_ERROR``, ``RUN_VERBOSE``).
-  Do not use ``throw``, use the utility ``cmnThrow`` instead. This allows to log and optionally disable exception. Furthermore, one can use this function as a breakpoint and ease the debugging with exceptions.
-  Do not use ``assert``, use the utility ``CMN_ASSERT`` instead. As for ``cmnThrow``, this allows to customize the behaviors of assertions and provides a trace in the cisst log file.
-  Use the C++ standard dynamic arrays: ``std::vector`` and ``std::string`` when dynamic array allocation is needed. Limit the use of dynamically allocated arrays like ``char * myString = new char[theLength];`` or ``double * coordinates = new double[size];``. For vectors of numerical values, consider the cisstVector containers.

General guidelines
------------------

This section is a quick reminder of basic practices which should be applied.

-  Avoid public data members.
-  Never, never use ``using namespace std`` or ``using std::<xyz>`` in a cisst library file. Never!
-  For a data member which need to be accessed, add the methods ``MyDataMember(void) const`` and ``MyDataMember(void)`` or methods that return a reference (``const`` or not) on the data member.
-  Use ``const`` and references, systematically. This can apply to the return type, parameters and the methods themselves.
-  Do not pass by value, unless it is necessary. If you use ``const``, you should probably use a reference too.
-  Limit the use of the ``?:`` conditional evaluation operator to cases of expression evaluation only. For example, ``x = (y > 5) ? 0 : 5``; Do not use it for more general conditions, in which case if/then/else structures are better.
-  The use of macros to represent constant values and inline evaluations should be avoided. Use enumeration types, constant values, and inline functions instead. Macros should be used for preprocessing tasks, such as conditional compilation and expansion templates.
-  Limit the use of global variables and global functions as much as possible. Prefer static data members and static functions in classes instead.
-  Limit the use of multiple inheritance as much as possible.
-  All includes use <>, not "" (e.g. ``#include <iostream>``).
-  For the standard library headers, use the filename without the .h suffix, e.g: ``#include <vector>``, ``#include <string>`` or ``#include <iostream>``, not ``#include <iostream.h>``.
-  Your files should #include only those header files required for the code that is in them. If your implementation file (YourClass.cpp) uses a class or a function that is not needed for the declaration of YourClass, ``#include`` that header file only through ``YourClass.cpp`` and not through ``YourClass.h``.
-  Avoid the declaration of the iterator/index in the ``for`` statements since compilers have different ways to interpret them. Statement like for ``(int i=0; i < 10; i++)`` must be avoided.
-  For a constructor, prefer initialization of data members to assignment in the body, e.g: ``myClass::myClass(const int & a, const int & b): A(a), B(b) {};``. The order of initialization must match the order of declaration of the data members in the class.
-  Do not write a copy constructor or assignment operator unless it is necessary. If you have a copy constructor, the parameter's name should be ``other``, e.g. ``myClass::myClass(const myClass& other): X(other.X) {};``
-  All modern compilers have a internal boolean type, therefore one should use ``bool``, ``true`` and ``false``. Don't use ``int``, ``short`` or whatever.
-  When writing a class hierarchy using derivations and virtual methods, make it a habit to declare the destructor method of the base class virtual. This prevents future troubles at hardly any cost.
-  Declare binary operators, such as ``+``, ``-``, etc., as global functions. This allows for a symmetric and consistent code. If necessary declare them as ``friend`` of their operands.
-  Overloaded operators should have a corresponding function/method and use it.
-  Some people like to use the keyword void to declare a parameter-less method or function, some don't. This is encouraged for the cisst libraries. E.g. ``void AFunction(void);``

Licensing issues
================

Standard header for the cisst Libraries
---------------------------------------

All files in the cisst libraries should start with the following header. The first two lines are here mostly to help intelligent/advanced/real editors (vi and emacs) to select the right language as well as tab/spacing.

.. code:: c++

   /* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
   /* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

   /*
     Author(s):  FirstName1 LastName1, FirstName2 LastName2
     Created on: YYYY-MM-DD

     (C) Copyright YYYY-YYYY Johns Hopkins University (JHU), All Rights
     Reserved.

   --- begin cisst license - do not edit ---

   This software is provided "as is" under an open source license, with
   no warranty.  The complete license can be found in license.txt and
   http://www.cisst.org/cisst/license.txt.

   --- end cisst license ---

   */

Can I use GPL Source Code in CISST Development?
-----------------------------------------------

| Any of the open source cisst software should **NOT be based on GPL code.**
| Of course, it is perfectly fine for people to use GPL code in any of their projects -- we just won't add any GPL code to the public repository. It is fine, however, to put "hooks" to compile/link with this code.

For example, you can have a CISST_HAS_SOME_GPL_LIBRARY option in CMake and can use it for conditional compilation. You can even check in code that has the following:

.. code:: c++

    #ifdef CISST_HAS_SOME_GPL_LIBRARY 
    #include <some_library.h>
    // Some code that uses classes/methods in some_library.h, as long as it is original code
    // that is not a derivative work of GPL code (e.g., examples provided by SOME_GPL_LIBRARY)
    #endif

In this case, we would have to make it clear to potential users that if they use CISST with SOME_GPL_LIBRARY, all of their code is subject to the GPL license.

Note: SOME_GPL_LIBRARY, is any code that has GPL license.
