cisstParameterTypes API
=======================

``cisstParameterTypes`` (``prm``) provides standardized data types for
robot state (joint positions, Cartesian poses, forces, etc.) used to enable
"plug and play" component interoperability across cisst/SAW components.

.. note::
   Some parameter types (e.g. ``prmPositionJointGet``, ``prmPositionCartesianGet``)
   are typedef specializations of ``mtsGenericObjectProxy`` and do not appear as
   standalone classes in the Doxygen XML. See the
   `cisstParameterTypes source headers <https://github.com/jhu-cisst/cisst/tree/main/cisstParameterTypes>`_
   for the full list.

Key classes
-----------

.. doxygenclass:: prmMotionBase
   :project: cisst
   :members:

.. doxygenclass:: prmForceCartesianGet
   :project: cisst
   :members:

.. doxygenclass:: prmForceCartesianSet
   :project: cisst
   :members:

.. doxygenclass:: prmVelocityJointGet
   :project: cisst
   :members:

.. doxygenclass:: prmVelocityJointSet
   :project: cisst
   :members:

.. doxygenclass:: prmVelocityCartesianSet
   :project: cisst
   :members:

.. doxygenclass:: prmEventButton
   :project: cisst
   :members:

.. doxygenclass:: prmActuatorParameters
   :project: cisst
   :members:

.. doxygenclass:: prmRobotState
   :project: cisst
   :members:
