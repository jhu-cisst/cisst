cisstRobot
==========

cisstRobot is a library that provides computational tools and algorithms for robotics. Among others, the cisstRobot library provides tools for evaluating kinematics and dynamics and for interpolating and blending trajectories. It is somewhat analogous to the Robotics Toolbox for Matlab.

cisstRobot is developed around the class `robManipulator </jhu-cisst/cisst/blob/main/cisstRobot/robManipulator.h>`__ that implements several algorithms for a kinematic chain. `robManipulator </jhu-cisst/cisst/blob/main/cisstRobot/robManipulator.h>`__ does not naturally extend to parallel or closed-loop manipulators. Each robot must have a configuration file that contains the parameters that are used by the various algorithms.

This tutorial will explain how you can define your robot and use it with cisstRobot.

`robManipulator </jhu-cisst/cisst/blob/main/cisstRobot/robManipulator.h>`__
-----------------------------------------------------------------------------

`robManipulator </jhu-cisst/cisst/blob/main/cisstRobot/robManipulator.h>`__ is the main class of cisstRobot. It provides the tools for computing forward/inverse kinematics, inverse dynamics and tools that can be useful for control such as joint/Cartesian space inertia matrices, spatial/body Jacobians, gravity loads, etc.

Before looking at `robManipulator </jhu-cisst/cisst/blob/main/cisstRobot/robManipulator.h>`__, we will look at how the kinematics/dynamics of a robot is defined. For your robot you need to create a text file that contains the kinematics and, optionally, the dynamics parameters of each link. By convention, these files have the .rob suffix. The CISST library provides .rob files of a few robots in the ``share/models`` sub-directory: `7DOF WAM </jhu-cisst/share/blob/main/models/WAM/wam7.rob>`__, `PUMA 560 </jhu-cisst/share/blob/main/models/PUMA560/puma560.rob>`__, `IRB 6600 </jhu-cisst/share/blob/main/models/IRB6600/irb6600.rob>`__, IRB 2400, CRS 260, FANUC and m16ib). All the robots have the mandatory kinematics parameters and only the 7DOF WAM has dynamics parameters. Note that you must provide the kinematics parameters of your robot. The dynamics parameters are optional and you cannot use "dynamics" methods unless you provide these parameters.

First, let's look at an example of the configuration file for a `7DOF WAM </jhu-cisst/share/blob/main/models/WAM/wam7.rob>`__. The first line is straightforward -- it says how many links the robot has. Since the 7DOF WAM has 7 links, the first line contains the number "7". A PUMA 560 has 6 links and, therefore, its first line has the number "6".

Then, on each of the subsequent lines, you enter the parameters of each link. Each parameter is separated by one or more spaces. The first parameter describes the kind of kinematics parameters. The word "standard" refers to standard Denavit-Hartenberg (DH) parameters. CISST robot supports the following kinematics conventions: `"standard" DH parameters </jhu-cisst/cisst/blob/main/cisstRobot/robDH.h>`__, `"modified" DH parameters </jhu-cisst/cisst/blob/main/cisstRobot/robModifiedDH.h>`__ and `Hayati parameters </jhu-cisst/cisst/blob/main/cisstRobot/robHayati.h>`__. Each of these conventions requires 4 numbers that represent the position and orientation of a distal coordinate frame with respect to the previous one. Reviewing these conventions is beyond the scope of this tutorial, as we will only describe the nature of each parameter.

Once the configuration file is completed, you can create a `robManipulator </jhu-cisst/cisst/blob/main/cisstRobot/robManipulator.h>`__ object and load the file. Likewise, you can derive a new class from robManipulator to implement control and trajectory algorithms.

Note: the rob file format is likely to change. It may be better to have a more structured format, such as "keyword=value" (as in an ``.ini`` file) or "keyword value" (as parsed by ``cmnStreamRawParser``) or XML format. There should be a field for the file format version number.

`Kinematics Parameters </jhu-cisst/cisst/blob/main/cisstRobot/robKinematics.h>`__
-----------------------------------------------------------------------------------

The first set of parameters represent the kinematics of each link. These parameters are mandatory and the order in which you specify them does matter.

`"Standard" DH Parameters </jhu-cisst/cisst/blob/main/cisstRobot/robDH.h>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Following the DH standard^1^ you must provide 4 numbers that define the orientation of the *ith* link with respect to the *i-1th* link. "Standard" DH convention assumes that the *ith* coordinate frame is at the *i+1* joint.

1. The first number represents the angle (in radians) between *zi-1* and *zi* about *xi*.

2. The second number represents the length (in meters) along *xi* of the common perpendicular between *zi-1* and *zi*.

3. The third number represents the angle (in radians) between *xi-1* and *xi* about *zi-1*.

4. The fourth number represents the distance (in meters) along axis *zi-1* between the origin of the *i-1th* coordinate frame and the point where the common perpendicular intersects axis *zi-1*

`"Modified" DH Parameters </jhu-cisst/cisst/blob/main/cisstRobot/robModifiedDH.h>`__ (also called Craig's convention)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Following the modified DH standard^2,3^, you must provide 4 numbers that define the orientation of the *ith* link with respect to the *i-1th* link. Unlike the "standard" DH convention, the "modified" DH convention assumes that the *ith* coordinate frame is at the *i* joint.

1. The first number represents the angle (in radians) between *zi-1* and *zi* about *xi-1*.

2. The second number represents the length (in meters) along *xi-1* of the common perpendicular between *zi-1* and *zi*.

3. The third number represents the angle (in radians) between *xi-1* and *xi* about *zi*.

4. The fourth number represents the distance (in meters) along axis *zi* between the origin of the *ith* coordinate frame and the point where the common perpendicular intersects axis *zi*

`Hayati Parameters </jhu-cisst/cisst/blob/main/cisstRobot/robHayati.h>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Hayati parameters^4^ are beneficial when representing coordinate frames that have parallel, or near parallel, *z* axes. Like the DH convention, the Hayati convention assumes that the *ith* coordinate frame is at the *i+1* joint. Furthermore, the nature of the parameters depends on whether the joint is revolute or prismatic. For a prismatic joint, the parameters are

1. The first number represents the angle (in radians) between *xi-1* and *xi*

2. The second number represents the angle (in radians) between *yi* and *yi*

3. The third number represents the angle (in radians) between *zi-1* and *zi*

4. The fourth number represents the distance (in meters) between the origin of the *ith* coordinate frame and *zi*

Joint Type
~~~~~~~~~~

Following these four numbers, you must specify the properties of the joint. For now, only two types of joints are supported -- "prismatic" and "revolute" -- and you must specify one of these for each link. Thus, the first joint is always on the first link and is defined on the second line (the first line contains the number of links). All the kinematics conventions mentioned above assume that a revolute joint rotates about a *z* axis and that a prismatic joint translates along a *z* axis.

Joint Mode
~~~~~~~~~~

The joint mode determines if a joint is "active" or "passive". An "active" joint is a joint that has a motor whereas a "passive" joint is a joint that is not linked to a motor and must be moved by external forces.

Joint Offset
~~~~~~~~~~~~

This value represents a constant offset that is applied to a joint. This is mostly useful for using joint values that correspond to manufacturer's values. For example, the DH convention of a robot has a fixed "zero" configuration where all the joints have a "zero" angle or translation. That configuration, however, can differ from a manufacturer "zero" configuration. Often, manufacturers will add an offset to specific joints to obtain a more practical "zero" configuration. This offset is never changed and is always automatically added to the joints.

Joint Limits
~~~~~~~~~~~~

The next two numbers represent the limits. These limits are as specified by the manufacturer and include any offset in the values. Limits for revolute joints are in radians and limits for prismatic joints are in meters.

Maximum Force/Torque
~~~~~~~~~~~~~~~~~~~~

The next parameter (``tm``) specifies the maximum force (for prismatic joints) or torque (for revolute joints) that can be applied by the joint.

`Dynamics Parameters </jhu-cisst/cisst/blob/main/cisstRobot/robMass.h>`__
---------------------------------------------------------------------------

Following the kinematics parameters you can specify dynamics parameters. These parameters are optional; that is you do not have to specify them or you can enter your grocery list if you do not intend to use dynamics of your robot.

Mass
~~~~

This is the mass (kg) of the *ith* link.

Center of Mass
~~~~~~~~~~~~~~

The next three numbers represent the X-Y-Z coordinates of the center of mass of the *ith* link with respect to the link's coordinate frame.

Mass Moment of Inertia
~~~~~~~~~~~~~~~~~~~~~~

The next 12 numbers represent the mass moment of inertia of the *ith* link. These 12 numbers are divided into two groups. All the values represent the mass distribution of the *ith* link with respect to the center of mass of the *ith* link.

a. The first 3 numbers represent the principal axes of the X-Y-Z principal moments of inertia.

b. The last 9 numbers represent the 3 unit vectors representing the (X-Y-Z) orientation of each of the principal moments.

Using robManipulator
--------------------

robManipulator provides the following methods

Base Transformation
~~~~~~~~~~~~~~~~~~~

You can set the transformation of the base frame of your robot (link 0) in the constructor. For example, if your robot is rotated with respect to your *world* coordinate frame, then you can capture this transformation in the base transformation.

Forward Kinematics
~~~~~~~~~~~~~~~~~~

Given a vector of joints, you can use the method ``robManipulator::ForwardKinematics``. The methods uses a vector of joint positions as an input and an optional link number. The method returns a 4x4 homogeneous transformation representing the orientation and position of the end-effector with respect to the world frame. The forward kinematics will account for the [#basetransformation base transformation].

By default, the forward kinematics will return the position and orientation of the end-effector, but if you need a the orientation and position of any link you can specify an optional parameter indicating a link number. The link number are *zero indexed* such that link number *0* refer to the base of the robot and will return the [#basetransformation base transformation]. Link number *1* will return the position and orientation of the first link and so on.

.. code:: c++

   vctDynamicVector<double> q( 7, 0.0 );                          // 7 joints with "zero" position
   vctFrame4x4<double> Rtw7 = manipulator.ForwardKinematics( q ); // Compute the forward kinematics

Inverse Kinematics
~~~~~~~~~~~~~~~~~~

robManipulator provides the method ``robManipulator::InverseKinematics`` to solve inverse kinematics problems. The method is generic and applicable to any manipulator. !InverseKinematics computes the inverse kinematics by solving a damped least-squares problem by using Newton's method. As Newton's method iterates towards a solution, it is important to provide an initial guess that is as close as possible to the desired solution. The method returns ``robManipulator::ESUCCESS`` if the algorithm converged within the specified accuracy and iteration limits. Otherwise it returns ``robManipulator::EFAILURE``.

The first argument is a vector of joint positions. As an input, this vector represents the initial guess to the inverse kinematics problem. As an output, the vector contains the solution of the problem (given that the algorithm was successful). The second argument is a 4x4 homogeneous transformation that represents the desired position and orientation of the end-effector. The third argument is optional and represent the accuracy tolerance. By default, this value is set to 1e-12. The fourth argument is also optional and represent the maximum number of iteration that the algorithm is given. By default this value is 1000 (which is very large). Thus InverseKinematics will return ``robManipulator::EFAILURE`` if the algorithm fails to converge within 1e-12 by using less than 1000 iterations. The last argument is the damping coefficient for tuning the solver.

.. code:: c++

   vctDynamicVector<double> q( 7, 0.0 );                          // 7 joints with "zero" position
   vctFrame4x4<double> Rtw7 = manipulator.ForwardKinematics( q ); // Compute the forward kinematics
   manipulator.InverseKinematics( q, Rtw7 );                      // Compute the inverse kinematics

Spatial Manipulator Jacobian
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The geometric spatial Jacobian is a linear transformation that maps joints velocities to spatial velocities *[vs, ws] = Js(q)qd*. The reader is referred to ^5^ for the difference between ''spatial'' and ''body'' Jacobians. The method used to evaluate the spatial Jacobian is ``robManipulator::JacobianSpatial``. Its only argument is a vector of joint positions.

Body Manipulator Jacobian
~~~~~~~~~~~~~~~~~~~~~~~~~

The geometric body Jacobian is a linear transformation that maps joints velocities to body velocities *[vb, wb] = Jb(q)qd*. The reader is referred to ^5^ for the difference between ''spatial'' and ''body'' Jacobians. The method used to evaluate the body Jacobian is ``robManipulator::JacobianBody``. Its only argument is a vector of joint positions.

Joint Space Inverse Dynamics
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you have the dynamics parameters of your manipulator, you can compute the inverse dynamics of your robot in joint space. Essentially, if you know the joints positions, velocities and accelerations, ``robManipulator::InverseDynamics`` will compute and return the corresponding joint torques/forces. The method requires a vector for the joint positions, velocities and accelerations. If the accelerations is zero, then only the coriolis, centrifugal and gravity load is computed.

.. code:: c++

   vctDynamicVector<double> q(7, 0.0), qd(7, 0.2), qdd(7, 0.5); // joint positions, velocities and accelerations
   vctDynamicVector<double> tau = manipulator.InverseDynamics( q, qd, qdd );

Cartesian Space Inverse Dynamics
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you have the dynamics parameters of your manipulator, you can compute the inverse dynamics of your robot in Cartesian space. Essentially, if you know the joints positions, velocities and the Cartesian accelerations of the end-effector, ``robManipulator::InverseDynamics`` will compute and return the corresponding joint torques/forces. The method requires a vector for the joint positions, velocities and a *6D* vector of Cartesian accelerations *[ vxd, vyd, vzd, wxd, wyd, wzd]*. If the accelerations is zero, then only the coriolis, centrifugal and gravity load is computed.

.. code:: c++

   vctDynamicVector<double> q(7, 0.0), qd(7, 0.2);   // joint positions, velocities
   vctFixedSizeVector<double,6> ft(0.5);             // Cartesian accelerations of the end-effector
   vctDynamicVector<double> tau = manipulator.InverseDynamics( q, qd, ft );

Coriolis, Centrifugal and Gravity Forces/Torques
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you have the dynamics parameters of your manipulator, you can compute the coriolis, centrifugal and gravity forces and torques exerted at each joint. The algorithm to evaluate these values is based on the recursive Newton-Euler. The method ``robManipulator::CCG`` takes two argument. The first one is a vector of joint position *q* and the second one is a vector of joint velocities *qd*. Note that the joint velocities only affect the coriolis and centrifugal terms, such that if you only want the gravity load on your joint you can use zero velocities. The method returns a vector of joint torques/forces that represent the coriolis, centrifugal and gravity forces/torques.

.. code:: c++

   vctDynamicVector<double> q( 7, 0.0 ), qd( 7, 1.0 );       // vectors of positions and velocities
   vctDynamicVector<double> tau = manipulator.CCG( q, qd );  // vectors of forces/torques

Joint Space Manipulator Inertia Matrix
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you have the dynamic parameters of your manipulator, you can compute the manipulator inertia matrix. This matrix is used to map joint accelerations to joint forces/torques. The method ``robManipulator::JSInertia`` only argument is a vector of joint angles and it returns a *NxN* matrix (positive definite).

.. code:: c++

   vctDynamicVector<double> q(7, 0.0);                       // vector of joint positions
   vctDynamicMatrix<double> M = manipulator.JSInertia( q );  // manipulator inertia matrix

Operation Space Manipulator Inertia Matrix
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you have the dynamic parameters of your manipulator, you can compute the operation space manipulator inertia matrix. This matrix is used to map Cartesian accelerations to Cartesian forces/torques. The method ``robManipulator::OSInertia`` only argument is a vector of joint angles and it returns a *6x6* matrix (positive definite).

.. code:: c++

   vctDynamicVector<double> q(7, 0.0);                             // vector of joint positions
   vctFixedSizeMatrix<double,6,6> M = manipulator.OSInertia( q );  // manipulator inertia matrix

Robot Components (SAW)
----------------------

Robot components are a set of "blocks" that implement specific tasks in a robot system. Available components are: manipulators (e.g., `Barrett WAM </jhu-saw/sawBarrett>`__), `controllers </jhu-saw/sawControllers>`__ and `trajectories </jhu-saw/sawTrajectories>`__. There are also two simulators available. The first one is based on `OpenSceneGraph </jhu-saw/sawOpenSceneGraph>`__ and can only simulate the kinematics of a robot. The second one is based on `Open Dynamics Engine </jhu-saw/sawOpenDynamicsEngine>`__ (and OpenSceneGraph) and can simulate kinematics and dynamics. Note that both robot simulators are derived from robManipulator and thus they use the same configuration file.

Each component runs in its own thread at a specified period and messages are passed between them in the form of input and output in either joint space or Cartesian space.

Example: `Barrett WAM </jhu-saw/sawBarrett>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Barrett WAM robot interface component consists of two classes: osaWAM and mtsWAM. As with other SAW components, the "osa" class is a lower-level interface, whereas the "mts" class is a component (based on cisstMultiTask).

Note: The Barrett WAM component currently has two provided interfaces: (1) an ``Output`` interface that provides a ``GetPositionJoint`` command, and (2) an ``Input`` interface that contains a ``SetTorqueJoint`` command. This should be updated to be a single provided interface that uses standard robot commands.

References
----------

1. Michael Brady, ''Robot motion: planning and control'', MIT Press 1982

2. W. Khalil, J.F. Kleinfinger, ''A new geometric notation for open and closed-loop robots'', ICRA 1986, pp. 1174-1179

3. J.J. Craig, ''Introduction to Robotics: Mechanics and Control'', Prentiss Hall, 1986

4. S. Hayati, M. Mirmirani, ''Improving the absolute positioning accuracy of robot manipulators'', Journal of Robotic Systems 2(4) 1985

5. R.M. Murray, Z. Li, S.S, Sastry, ''A mathematical introduction to robotic manipulation''', CRC Press, 1994
