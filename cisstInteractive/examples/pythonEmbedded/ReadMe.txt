

Author(s): Anton Deguet


The goal of this example is to demonstrate the use of the object
register in conjunction with the class register to implement an
application using both C++ and Python.  The execution starts in C++,
the programmer registers a couple of objects of different classes and
then start an interactive shell in Python.  Once in the shell, the
user can get a handle on any registered object and modify them.  He
can also create new objects and register them to share them with the
C++ code.

This example uses the cisst object register (cmnObjectRegister) to share
C++ objects with Python. The C++ classes are exposed with pybind11, including
their inheritance from cmnGenericObject, so FindObject can return the concrete
dynamic type (e.g. myDerivedClass) to Python.

