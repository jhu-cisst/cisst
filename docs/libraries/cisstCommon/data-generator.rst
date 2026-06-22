Introduction
============

In the *cisst* libraries, as in many other libraries, we often need to handle different data objects in a transparent manner. For example:

-  Serialize and de-serialize to send data between processes
-  Save data in text files for post processing
-  Display current value in a human readable format
-  Extract numbers from a data object for plotting

There are multiple ways to provide a similar interface to multiple objects but the most common are **class inheritance** and **type traits**.

Class inheritance
-----------------

Class inheritance is one possible solution. Amongst its advantages:

-  Allows heterogeneous containers of objects
-  Possibility to perform run-time type check using `Run-Time Type Information (RTTI) <http://en.wikipedia.org/wiki/Run-time_type_information>`__.

Amongst the drawbacks:

-  All objects must be derived from a *cisst* base type (let's call it ``cmnGenericObject``). If one wants to use an object from another library (of type ``X``) within the *cisst* "framework" one can either:

   -  Create a new class derived from ``cmnGenericObject`` and ``X``. Multiple inheritance can be tricky and can lead to strange problems. For example, Qt ``QObject`` need to be derived from ``QObject`` first as Qt tend to cast (``reinterpret_cast`` objects to ``QObject`` and this only works if the "vtable" for the ``QObject`` comes first). Keep in mind that constructors and assignment operators have to be redefined for all derived classes.
   -  Create a new object derived from ``cmnGenericObject`` that has a data member of type ``X``. To access the object ``data`` the user will now have to write ``object.data``. The new type is often referred as a proxy object. Keep in mind that all constructors of class ``X`` still need to be duplicated.

-  Run-time type checks are fine but in many cases the type is or should be known at compilation time. A compilation error might be preferable to a run-time error ...

Type traits
-----------

An alternative solution is to use type traits. Type traits can be implemented using an overloaded global function or templates static methods. Let's consider a simple functionality: ``std::string HumanReadable(void)``. This can be implemented using a base class and a virtual method ``virtual std::string cmnGenericObject::HumanReadable(void) const = 0``. Using the type traits approach one can use a global function:

.. code:: c++

   std::string cmnDataHumanReadable(const double & data);

Or even better a templated class with a static method:

.. code:: c++

   template <>
   class cmnData<double> {
     public:
       static std::string HumanReadable(double & data) {
         return someStringBasedOnDataValue;
       }
       // ... all other methods required ...
    };

The later is preferred because it doesn't leave any room for interpretation or implicit cast(s) for the compiler. For the end user, this allows to use the type ``X`` as he or she is used to, all the methods, constructors and assignment operators remain available. For the *cisst* libraries, all objects can now be handled using the same set of features. Within the *cisst* libraries, we can still use heterogenous containers using the proxy pattern but these proxies don't interfere with the default API of ``X``.

**``cmnData``**
---------------

For all generic operations, *cisst* uses the templated class ``cmnData<>``. For operations requiring external dependencies a separate class should be created. For example methods to read and write in `JSON <http://en.wikipedia.org/wiki/JSON>`__ format are defined within ``cmnDataJSON``.

The main methods are define in ``cisstCommon/cmnDataFunctions``, in class ``cmnData<>``:

-  ``std::string HumanReadable(const DataType & data)``: human readable for user interfaces
-  ``void Copy(DataType & data, const DataType & source)``: efficient copy, can rely on copy constructor or user implementation to reduce memory allocations
-  ``std::string SerializeDescription(const DataType & data, const char delimiter, const std::string & userDescription = "")``: string used to create a description of data being serialized, the output can be processed
-  ``void SerializeText(const DataType & data, std::ostream & outputStream, const char delimiter = ',') throw (std::runtime_error)``: text serialization to a C++ stream
-  ``void DeSerializeText(DataType & data, std::istream & inputStream, const char delimiter = ',') throw (std::runtime_error)``: text de-serialization from a C++ stream
-  ``void SerializeBinary(const DataType & data, std::ostream & outputStream) throw (std::runtime_error)``: binary serialization to a C++ stream
-  ``void DeSerializeBinary(DataType & data, std::istream & inputStream, const cmnDataFormat & localFormat, const cmnDataFormat & remoteFormat) throw (std::runtime_error)``: binary de-serialization from a C++ stream. The application needs to know what the remote format was (32 vs 64 bits, little vs big endian, long integers treated as 32 vs 64 bits) as well as the local format to handle conversions
-  ``size_t SerializeBinaryByteSize(const DataType & data)``: size in bytes required to store a binary version of the object. Serialization to stream doesn't use the size, serialization to buffer requires it
-  ``size_t SerializeBinary(const DataType & data, char * buffer, size_t bufferSize)``: binary serialization to a buffer
-  ``size_t DeSerializeBinary(DataType & data, const char * buffer, size_t bufferSize, const cmnDataFormat & localFormat, const cmnDataFormat & remoteFormat)``: binary de-serialization from a buffer
-  ``std::string ScalarDescription(const DataType & data, const size_t index, const std::string & userDescription = "") throw (std::out_of_range)``: assuming the data object contains number(s), i.e. scalar(s), human readable description of one of the scalars (e.g. "Translation[0]"). Can be used for user interfaces
-  ``double Scalar(const DataType & data, const size_t index) throw (std::out_of_range)``: assuming the data object contains scalar(s), value of one of the scalar(s)
-  ``size_t ScalarNumber(const DataType & data)``: number of scalars contain in the data object (e.g. 3 for a ``vct3``)
-  ``bool ScalarNumberIsFixed(const DataType & data)``: helper function to optimize search of ith scalar in a compound data object

For the JSON, the following two methods are defined within ``cmnDataJSON<>``:

-  ``void SerializeText(const DataType & data, Json::Value & jsonValue)``: serialize to JSON
-  ``void DeSerializeText(DataType & data, const Json::Value & jsonValue) throw (std::runtime_error)``: de-serialize from JSON

``cmnData`` is already instantiated for the C/C++ native types, some of the STL containers (``std::string``, ``std::vector`` of any type supported) and all the ``cisstVector`` types. Some existing helper methods and macros can be used to support additional data types.

*cisst* implementations
-----------------------

There are different approaches implemented within the *cisst* libraries, mostly reflecting the history of the libraries.

The oldest code relies on inheritance from a generic base type (``cmnGenericObject`` for *cisstCommon* and ``mtsGenericObject`` for *cisstMultiTask*). The *cisstMultiTask* library also uses type traits and internal proxies. For example, there is a class ``mtsDouble3`` that inherits from ``mtsGenericObject`` and ``vctFixedSizeVector<double,3>`` (this is one of the few places in the cisst libraries where multiple inheritance is used). But, you can also directly use ``vctDouble3`` (and its typedef ``vct3``) because cisstMultiTask defines all the "helper" functions that are needed by the wrapper object, ``mtsGenericObjectProxy<vct3>`` (which is not the same as ``mtsDouble3``).

Ultimately, *cisstMultiTask* data types should all be using the type traits approach with ``cmnData`` and internal proxies.

Data generator
==============

Whichever approach is chosen, there is a fair amount of code to write for each data type:

-  This is a tedious task
-  It's fairly easy to introduce some bugs in the code by omitting one or more data members or base class in the serialize/de-serialize methods
-  Each and every data type class has to be updated when a new feature is introduced

To avoid these issues, many libraries rely on a high level description (see for example Corba IDL, ROS messages, ...) and a code generator to produce the appropriate code in C, C++, ObjectiveC, Python, ... For the *cisst* libraries, we developed yet another data description format. One of the decisions made is to allow inline C/C++ code in the data description and therefore restrict the target language to C++. On the other hand, this allows us to:

-  Create customized API for our data types while the code generator handles the common and repetitive part of the code.
-  Use any C/C++ class within your data structures. The only requirement is that a few global functions need to be overloaded to handle non standard data types (e.g. a VTK mesh). See Hand Written data types.

File format
-----------

The syntax is as follows:

-  The file contains a list of scopes, a scope is defined by a keyword followed by ``{``, the scope's content and a closing ``}``
-  Each scope can contain other scopes or fields
-  Each field is defined by a keyword followed by ``=``, the field's content and a closing ``;``
-  One can use C++ style line comments, i.e. ``//``

The file format (supported scopes and fields) can be retrieved using the command line option ``-s`` or ``--syntax-only``:

::

   cisstDataGenerator --syntax-only
   File syntax:
     class {
       base-class {
         is-data = <value must be one of 'true' 'false': default is 'true'>; // (optional) - indicates if the base class is a cisst data type itself
         type = <user defined string>; // (required) - C++ type for the base class, e.g. cmnGenericObject
         visibility = <value must be one of 'public' 'private' 'protected': default is 'public'>; // (optional) - determines if the base class should be public, ...
       }
       typedef {
         name = <user defined string>; // (required) - name of the new type defined
         type = <user defined string>; // (required) - C/C++ type used to define the new type
       }
       member {
         accessors = <value must be one of 'none' 'references' 'set-get' 'all': default is 'all'>; // (optional) - indicates which types of accessors should be generated for the data member
         default = <user defined string>; // (optional) - default value that should be assigned to the data member in the class constructor
         description = <user defined string>; // (optional) - user provided description of the data member
         is-data = <value must be one of 'true' 'false': default is 'true'>; // (optional) - indicates if the data member is a cisst data type itself
         is-size_t = <value must be one of 'true' 'false': default is 'false'>; // (optional) - indicates if the data member is a typedef of size_t or size_t
         name = <user defined string>; // (required) - name of the data member, will also be used to generate accessors
         type = <user defined string>; // (required) - C++ type of the data member (e.g. double, std::string, ...)
         visibility = <value must be one of 'public' 'protected' 'private': default is 'protected'>; // (optional) - indicates if the data member should be public, ...
       }
       inline-header {
         C++ code snippet - code that will be placed as-is in the generated header file
       }
       inline-code {
         C++ code snippet - code that will be placed as-is in the generated source file
       }
       attribute = <user defined string>; // (optional) - string place between 'class' and the class name (e.g. CISST_EXPORT)
       name = <user defined string>; // (required) - name of the generated C++ class
     }
     inline-header {
       C++ code snippet - code that will be placed as-is in the generated header file
     }
     inline-code {
       C++ code snippet - code that will be placed as-is in the generated source file
     }

The ``cisstDataGenerator`` provides error messages when it encounters syntax errors along with the line number corresponding to the error. Error messages are formatted to match gcc errors and can be parsed by most IDEs.

Examples
--------

It is important to note that the cisst data generator doesn't force any specific implementation related to cisstMultiTask, i.e. it is possible to use either inheritance from ``mtsGenericObject`` or the proxy approach.

See examples with inlined comments:

-  Classes not used with cisstMultiTask: `demoData.cdg <https://github.com/jhu-cisst/cisst/blob/master/cisstCommon/examples/dataGenerator/demoData.cdg>`__
-  Classes used with cisstMultiTask:
-  `mtsComponentState.cdg </jhu-cisst/cisst/blob/master/cisstMultiTask/mtsComponentState.cdg>`__ and user implementation for extra code `mtsComponentState.cpp </jhu-cisst/cisst/blob/master/cisstMultiTask/code/mtsComponentState.cpp>`__. This class uses macros to implement the proxy pattern (see the ``mtsGenericObjectProxy`` definition at the end of the ``cdg`` file).
-  `prmPositionCartesianGet.cdg </jhu-cisst/cisst/blob/master/cisstParameterTypes/prmPositionCartesianGet.cdg>`__. This class is derived from ``mtsGenericObject``
-  `prmPositionJointGet.cdg </jhu-cisst/cisst/blob/master/cisstParameterTypes/prmPositionJointGet.cdg>`__. This class is derived from ``mtsGenericObject``

CMake
-----

We provide a CMake macro that simplifies the build process. This macro manages the dependencies as well as build rules between the description file, generated header and source files and object files. The macro is defined in the file ``cisstMacros.cmake`` which is automatically included when you ``include (${CISST_USE_FILE})`` in your ``CMakeLists.txt``. Here are two examples of use:

.. code:: cmake

     # create data type using the data generator
     cisst_data_generator (cmnExDataGenerator    # prefix for the CMake variables that will contain the lists of headers/sources
                           ${CMAKE_CURRENT_BINARY_DIR}    # destination directory where do you want the generated files to go
                           ""    # subdirectory for the header file, see next example.  This will be appended to the destination directory.
                           demoData.cdg)    # one or more cisst data description files

     # to compile cisst generated code, need to find header file
     include_directories (${CMAKE_CURRENT_BINARY_DIR})

     add_executable (cmnExDataGenerator
                     ${cmnExDataGenerator_CISST_DG_SRCS}   # variable automatically created and populated by cisst_data_generator macro using the provided prefix
                     main.cpp)

Another example using the include subdirectory and the list of header files generated:

.. code:: cmake

   # create data type using the data generator
   cisst_data_generator (cisstParameterTypes
                         "${cisst_BINARY_DIR}/include" # where to save the files
                         "cisstParameterTypes/"           # sub directory for include, header files will in "include/cisstParameterTypes"
                                                                       # and can be included using #include <cisstParameterTypes/prmPositionCartesianGet.h>
                         prmPositionCartesianGet.cdg  # using multiple data description files
                         prmPositionJointGet.cdg)

   # to compile cisst generated code, need to find header file
   include_directories (${CMAKE_CURRENT_BINARY_DIR})

   # ${cisstParameterTypes_CISST_DG_SRCS} contains the list of generated source files (absolute paths)
   # ${cisstParameterTypes_CISST_DG_HDRS} contains the list of generated header files (absolute paths)

Hand written code
=================

The only reason to instantiate ``cmnData`` for a new data type is to include an existing type from an external library (stl, VTK, OpenCV, ...). If you are creating a new data type, please use the data generator.

Code
----

For all new existing type to support (``externalType``), you will need to declare and instantiate a specialized version of ``cmnData``. Your code should look like:

.. code:: c++

   template <>
   class cmnData<externalType>
   {
   public:
       typedef externalType DataType;

       CMN_DATA_IS_SPECIALIZED_TRUE(externalType)
       CMN_DATA_COPY_USING_ASSIGN(externalType); // assuming the most efficient to copy is to use "="
       CMN_DATA_SERIALIZE_DESCRIPTION(externalType, exType); // for human description, short name

       static std::string HumanReadable(const std::string & data) {
           ...
       }

       static size_t SerializeBinaryByteSize(const DataType & data) {
           ...
       }

       static size_t SerializeBinary(const DataType & data,
                                     char * buffer, size_t bufferSize) {
           ...
       }

       static size_t DeSerializeBinary(DataType & data, const char * buffer, size_t bufferSize,
                                       const cmnDataFormat & localFormat,
                                       const cmnDataFormat & remoteFormat)
       {
           ...
       }

       static void SerializeBinary(const DataType & data,
                                   std::ostream & outputStream)
           throw (std::runtime_error)
       {
           ...
       }

       static void DeSerializeBinary(DataType & data,
                                     std::istream & inputStream,
                                     const cmnDataFormat & localFormat,
                                     const cmnDataFormat & remoteFormat)
           throw (std::runtime_error)
       {
           ...
       }

       static void SerializeText(const DataType & data,
                                 std::ostream & outputStream,
                                 const char delimiter)
           throw (std::runtime_error)
       {
           ...
       }

       static void DeSerializeText(DataType & data,
                                   std::istream & inputStream,
                                   const char delimiter)
           throw (std::runtime_error)
       {
           ...
       }

       static bool ScalarNumberIsFixed(const std::string & CMN_UNUSED(data)) {
           return true; // true is size never changes
       }

       static size_t ScalarNumber(const std::string & data) {
           ... // could vary, for std::vector<double>, should return size()
       }

       static std::string ScalarDescription(const std::string & data, const size_t CMN_UNUSED(index),
                                            const std::string & userDescription) throw (std::out_of_range) {
           ... ideally return a string for the given scalar ...
           ... other wise throw exception.  The return value is just to avoid compiler warnings ...
           cmnThrow(std::out_of_range("cmnDataScalarDescription: std::string has no scalar"));
           return "n/a";
       }

       static double Scalar(const std::string & data, const size_t index) throw (std::out_of_range) {
           ... ideally return a value for the given scalar ...
           ... other wise throw exception.  The return value is just to avoid compiler warnings ...
           cmnThrow(std::out_of_range("cmnDataScalar: std::string has no scalar"));
           return 1.234;
       }
   };

.. _examples-1:

Examples
--------

The best examples are existing instantiations for the C/C++ native types, STL types and *cisstVector* data types.

-  Native types: these heavily rely on a set of `macros <https://github.com/jhu-cisst/cisst/blob/master/cisstCommon/cmnDataFunctionsMacros.h>`__, see `cmnDataFunctions.h <https://github.com/jhu-cisst/cisst/blob/master/cisstCommon/cmnDataFunctions.h>`__ and `cmnDataFunctions.cpp <https://github.com/jhu-cisst/cisst/blob/master/cisstCommon/code/cmnDataFunctions.cpp>`__
-  ``std::string``: `cmnDataFunctionsString.h <https://github.com/jhu-cisst/cisst/blob/master/cisstCommon/cmnDataFunctionsString.h>`__ and `cmnDataFunctionsStrings.cpp <https://github.com/jhu-cisst/cisst/blob/master/cisstCommon/code/cmnDataFunctionsString.cpp>`__
