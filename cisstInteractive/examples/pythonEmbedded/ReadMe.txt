

Author(s): Anton Deguet


The goal of this example is to demonstrate the use of the object
register in conjunction with the class register to implement an
application using both C++ and Python.  The execution starts in C++,
the programmer registers a couple of objects of different classes and
then start an interactive shell in Python.  Once in the shell, the
user can get a handle on any registered object and modify them.  He
can also create new objects and register them to share them with the
C++ code.

This example relies on the cisst object register (cmnObjectRegister)
as well as the class register (cmnClassRegister) to find the class
type to return.  The wrapping of the C++ code is performed by SWIG.
An interesting feature of this example is the use of the method
cmnObjectRegisterGet added in cisstCommon.i which cast the pointer on
the base class (cmnGenericObject) to whatever is the actual class of
the object (e.g. myDerivedClass).

