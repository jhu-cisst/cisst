cisstStereoVision API
=====================

``cisstStereoVision`` (SVL) is an image/video stream processing library
built on the cisstMultiTask component framework. Processing elements are
called *filters* and are connected into *streams* managed by a
``svlStreamManager``.

Key classes
-----------

.. doxygenclass:: svlFilterBase
   :project: cisst
   :members:

.. doxygenclass:: svlStreamManager
   :project: cisst
   :members:

.. doxygenclass:: svlFilterImageWindow
   :project: cisst
   :members:

.. doxygenclass:: svlFilterSplitter
   :project: cisst
   :members:

.. doxygenclass:: svlCameraGeometry
   :project: cisst
   :members:

.. doxygenclass:: svlBufferSample
   :project: cisst
   :members:
