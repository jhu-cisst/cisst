Tutorial
========

In this tutorial we will create two simple components. The first component is a ``counter``. Its main function is to periodically increment an internal counter. When the counter overflows it should send an event. One should be able to query the current value and set the increment. If the user requests an incorrect value for the increment, the counter component will throw an event with an informative message.

The second component ``user`` is designed to be connected to the ``counter`` component. In this example, we're avoiding using a GUI toolkit to make the code as simple as possible. The user interface is text based.

The latest version of the code for this tutorial can be compiled along the *cisst* using the CMake options ``CISST_BUILD_EXAMPLES`` and ``CISST_cisstMultiTask_EXAMPLES``. The code itself can be found in ```cisst/cisstMultiTask/examples/tutorial`` </jhu-cisst/cisst/tree/main/cisstMultiTask/examples/tutorial>`__

Counter component
-----------------

Periodic task
~~~~~~~~~~~~~

For this component, we're using a the base class ``mtsTaskPeriodic``, i.e. the ``Run`` method will be called periodically to perform all user defined computations and the library will attempt to maintain a constant frequency between calls to the ``Run`` method. The amount of jitter depends on the services provided by the Operating System (the *cisstOSAbstraction* library provides an abstraction layer to the different OS features).

One of the parameters provided to the constructor is the desired periodicity provided as a ``double`` representing the time in seconds. To make sure the code is readable, *cisst* has a set of constants used to indicate the units used, e.g. ``5.0 * cmm_ms`` indicates 5 milliseconds.

It is possible to use different types of components: continuous, event based, triggered by an external event, ... See all component and task types in :doc:`cisstMultiTask concepts </libraries/cisstMultiTask/concepts>`.

In the header file:

.. literalinclude:: ../../../cisstMultiTask/examples/tutorial/counter.h
   :language: c++
   :start-after: // [doc-task-header-start]
   :end-before: // [doc-task-header-end]

In the implementation file:

.. literalinclude:: ../../../cisstMultiTask/examples/tutorial/counter.cpp
   :language: c++
   :start-after: // [doc-constructor-start]
   :end-before: // [doc-constructor-end]

The ``Run`` method of a task contains the user computations, in our example:

.. literalinclude:: ../../../cisstMultiTask/examples/tutorial/counter.cpp
   :language: c++
   :start-after: // [doc-run-method-start]
   :end-before: // [doc-run-method-end]

Since most components use queued commands to be thread safe, one must remember to empty the queues of commands using the method ``ProcessQueuedCommands``.

State table
~~~~~~~~~~~

All *cisstMultiTask* components own at least one state table. A state table can be seen as a matrix where the columns represent a state data object and the rows the values of that data object over time. It is implemented as a circular buffer and is used to provide a thread safe mechanism to publish the component's data (one writer, multiple readers). For historical reasons, all components have a default state table member (``StateTable``) but users can add as many state tables as they need (using ``AddStateTable``). One can add more state tables to handle:

-  different refresh rates, i.e. some data doesn't change over time. In this example, the increment changes only when set by the user so we don't need to save the data at the same refresh rate as the counter value.
-  similar groups of data, i.e. a component can maintain multiple similar interfaces corresponding to similar devices (e.g. left and right arms of a robot). In this case, one can use two tables containing the exact same data.

In the header file, declare data to be added to state table(s) and optionally user defined state tables:

.. literalinclude:: ../../../cisstMultiTask/examples/tutorial/counter.h
   :language: c++
   :start-after: // [doc-state-table-members-start]
   :end-before: // [doc-state-table-members-end]

In implementation file, configuring the state tables:

.. literalinclude:: ../../../cisstMultiTask/examples/tutorial/counter.cpp
   :language: c++
   :start-after: // [doc-state-table-start]
   :end-before: // [doc-state-table-end]

For the user defined state table, since we turned off *Automatic Advance*, we need to *Start* and *Advance* manually:

.. literalinclude:: ../../../cisstMultiTask/examples/tutorial/counter.cpp
   :language: c++
   :start-after: // [doc-state-table-manual-start]
   :end-before: // [doc-state-table-manual-end]

There is no need to *Start* and *Advance* the default state table ``StateTable``; this is performed automatically before and after the calls to the ``Run`` method. This applies to all state tables with *Automatic Advance* turned on (default behavior).

Commands
~~~~~~~~

*cisstMultiTask* uses the *command pattern*, i.e. the user doesn't directly call the C++ methods of a component. All methods are encapsulated in command *objects* (e.g. ``mtsCommandVoid``). These command objects are grouped in *interfaces* and can be retrieved by name at runtime (see also :doc:`cisstMultiTask concepts </libraries/cisstMultiTask/concepts>`). In practice, the first step is to create an interface that will contain some of the provided features of the component (i.e. commands and events). To add an ``mtsInterfaceProvided``:

.. literalinclude:: ../../../cisstMultiTask/examples/tutorial/counter.cpp
   :language: c++
   :start-after: // [doc-provided-interface-start]
   :end-before: // [doc-provided-interface-end]

The next step is to add some commands to the newly created provided interface. For the counter component, we're going to use 3 different types of commands:

-  *Void*: command that doesn't require any payload, usually encapsulating a method with the signature ``void method(void)``. Void commands usually change the state of the component and therefore encapsulate non const methods. If the component owns its own thread, void commands are queued.
-  *Write*: command that requires a payload, usually encapsulating a method with the signature ``void method(const payloadType & payload)``. Write commands usually change the state of the component of the state and therefore encapsulate non const methods. If the component owns its own thread, write commands are queued.
-  *Read*: command used to get some information from the component, usually encapsulating a method with the signature ``void method(payloadType & placeHolder) const``. Read commands shouldn't change the state of the component and therefore encapsulate const methods. Read commands are never queued, so make sure the encapsulated method is thread safe. For this example, we're using a built-in mechanism to read from the state table which happens to be thread safe.

As for the ``AddInterfaceProvided``, one should test if the command has been added properly. This is unlikely to fail for a hard-coded list of commands.

In the header file, declaration of encapsulated methods:

.. literalinclude:: ../../../cisstMultiTask/examples/tutorial/counter.h
   :language: c++
   :start-after: // [doc-command-methods-start]
   :end-before: // [doc-command-methods-end]

In your implementation file, add the provided interface and commands:

.. literalinclude:: ../../../cisstMultiTask/examples/tutorial/counter.cpp
   :language: c++
   :start-after: // [doc-provided-and-commands-start]
   :end-before: // [doc-provided-and-commands-end]

Events
~~~~~~

Commands are always triggered by the *other* component, i.e. the component connected to the provided interface (in our example, *counter* component has the provided interface *User*). It is also possible to add events to a provided interface. Events are triggered by the component that owns the provided interface. As for the commands, it is possible to declare an event with or without payload (*write event* and *void event*).

In the header file:

.. literalinclude:: ../../../cisstMultiTask/examples/tutorial/counter.h
   :language: c++
   :start-after: // [doc-event-members-start]
   :end-before: // [doc-event-members-end]

In the implementation file, to add the events to the provided interface:

.. literalinclude:: ../../../cisstMultiTask/examples/tutorial/counter.cpp
   :language: c++
   :start-after: // [doc-events-setup-start]
   :end-before: // [doc-events-setup-end]

To trigger an event:

.. literalinclude:: ../../../cisstMultiTask/examples/tutorial/counter.cpp
   :language: c++
   :start-after: // [doc-trigger-event-start]
   :end-before: // [doc-trigger-event-end]

For events, the two possible errors are:

-  ``mtsFunction`` is not bound, i.e. the user forgot to use this function as an event trigger, i.e. it was not used with ``AddEvent``
-  Invalid data type for a *write* event, the payload doesn't correspond to the type declared with ``AddEventWrite``.

User component
--------------

Our *user* component uses the terminal to display its state and key hits to trigger the different commands. The default commands are:

::

   Press ...
    [g] to get current counter value
    [r] to reset counter
    [i] to set a new counter increment
    [q] to quit

Since we want to continuously observe the user's key hits, the *user* component is derived from ``mtsTaskContinuous``. In the ``Run`` method, we use the *cisstCommon* functions ``cmnKbHit`` and ``cmnGetChar`` to capture the key hits in a non-blocking manner. We need to make sure listening to keyboard hits is not blocking because the ``Run`` method also calls ``ProcessQueuedEvents``. In general, programmers should make sure computations in the ``Run`` methods are non-blocking.

Functions
~~~~~~~~~

The *user* component needs a *required interface* to group all the features it requires. Each feature is declared as a *function* (object of type ``mtsFunction...``). Overall, when the two components are connected, the *user*'s required interface is connected to the *counter*'s provided interface. Each *function* declared in the *user*'s required interface is connected to the corresponding *command* in the *counter*'s provided interface. The two sets of features (commands and events of a provided interface vs. functions and event handlers of a required interface) can be subsets of each other (see also :doc:`cisstMultiTask concepts </libraries/cisstMultiTask/concepts>`).

In the header file:

.. literalinclude:: ../../../cisstMultiTask/examples/tutorial/user.h
   :language: c++
   :start-after: // [doc-required-functions-start]
   :end-before: // [doc-required-functions-end]

In the implementation file:

.. literalinclude:: ../../../cisstMultiTask/examples/tutorial/user.cpp
   :language: c++
   :start-after: // [doc-required-interface-start]
   :end-before: // [doc-required-interface-end]

Event handlers
~~~~~~~~~~~~~~

Event handlers are similar to commands, i.e. they also encapsulate C++ methods and come in two flavors, *void* and *write*. One must first declare the methods used to handle the events, most likely ``private`` or ``protected``.

In the header file:

.. literalinclude:: ../../../cisstMultiTask/examples/tutorial/user.h
   :language: c++
   :start-after: // [doc-event-handlers-start]
   :end-before: // [doc-event-handlers-end]

Event handlers must be added to an existing provided interface when the component is being constructed or configured:

.. literalinclude:: ../../../cisstMultiTask/examples/tutorial/user.cpp
   :language: c++
   :start-after: // [doc-event-handlers-setup-start]
   :end-before: // [doc-event-handlers-setup-end]

If the component owns its thread, events will most likely be queued on the required interface. To make sure the queues don't get filled, one has to process all the queued events. In the ``Run`` method:

.. literalinclude:: ../../../cisstMultiTask/examples/tutorial/user.cpp
   :language: c++
   :start-after: // [doc-process-queued-events-start]
   :end-before: // [doc-process-queued-events-end]

Connecting components
~~~~~~~~~~~~~~~~~~~~~

Once the two components have been implemented, they need to be connected together using their interfaces. A required interface can be connected to one and only one provided interface. On the other hand, it is possible to connect multiple required interfaces to a single provided interface.

When connecting a required interface to a provided interface:

-  The interface names don't have to match
-  Name and types of commands and events must match
-  The provided interface can have more commands and events than the required interface (unused features)
-  Functions and event handlers of a required interface can be tagged as *optional*. If so, the required interface can still be connected to a provided interface that doesn't provide said features

To manage all the components, use the ``mtsComponentManager`` object. The manager is implemented as a singleton so we need to call the static ``mtsComponentManager::GetInstance()`` method. The following steps are:

-  Add the components to the manager
-  Connect the components
-  Create the components; create the component threads and call their ``Startup`` method
-  Start the components; the ``Run`` method will be called
-  Kill the components; stop the threads and call the ``Cleanup`` method

In the main file:

.. literalinclude:: ../../../cisstMultiTask/examples/tutorial/main.cpp
   :language: c++
   :start-after: // [doc-main-start]
   :end-before: // [doc-main-end]
