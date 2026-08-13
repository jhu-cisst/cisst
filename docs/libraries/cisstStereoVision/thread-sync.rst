Thread Synchronization in SVL Filters
=====================================

Sample processing inside cisstStereoVision filters is multithreaded. During filter implementation, the developer needs to be aware of the fact that the ``Process()`` method of the filter may be executed in any number of thread simultaneously.

Helper Macros
-------------

+--------------------------------------------------------------------------------------------------------------+-------------+
| Name                                                                                                         | Description |
+==============================================================================================================+=============+
| ``_OnSingleThread(svlProcInfo * _info) { }``                                                                 |             |
+--------------------------------------------------------------------------------------------------------------+-------------+
| ``_ParallelLoop(svlProcInfo * _info, unsigned int _idx, unsigned int _count) { }``                           |             |
+--------------------------------------------------------------------------------------------------------------+-------------+
| ``_GetParallelSubRange(svlProcInfo * _info, unsigned int _count, unsigned int & _from, unsigned int & _to)`` |             |
+--------------------------------------------------------------------------------------------------------------+-------------+
| ``_SynchronizeThreads(svlProcInfo * _info)``                                                                 |             |
+--------------------------------------------------------------------------------------------------------------+-------------+
| ``_CriticalSection(svlProcInfo * _info) { }``                                                                |             |
+--------------------------------------------------------------------------------------------------------------+-------------+

Code samples
------------

-  :doc:`Tutorial part 4 </libraries/cisstStereoVision/tutorials/tutorial-4>`
-  :doc:`Tutorial part 5 </libraries/cisstStereoVision/tutorials/tutorial-5>`
