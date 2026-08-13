svlFilterSplitter class
=======================

Used to divide any stream into multiple asynchronous streams. The stream has one synchronous input and one synchronous output by default. The user may add any number of asynchronous outputs by calling the ``AddOutput`` method and specifying a name for it that is unique on the filter. The filter never modifies the contents of the incoming samples, however it may make copies of them and send the copies out to its asynchronous outputs.

svlFilterImageWindow:: public methods
-------------------------------------

+------------------------------------------------------------------------------------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Name                                                                                     | Description                                                                                                                                                                                                                                                                                                                                                                                                                                  |
+==========================================================================================+==============================================================================================================================================================================================================================================================================================================================================================================================================================================+
| ``int AddOutput(std::string & name, unsigned int threadcount, unsigned int buffer size`` | Adds a asychronous output to the splitter filter. The output can be referred to by its ``name``. The number of threads in the corresponding thread pool is defined by ``threadcount``. The output has a built in circular buffer that is capable of storing data samples in case the connected branch is not able to process them as fast as they arrive. The unit of buffer-size is *samples* and the size is specified in ``buffer size``. |
+------------------------------------------------------------------------------------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Code samples
------------

-  :doc:`Tutorial part 5 </libraries/cisstStereoVision/tutorials/tutorial-5>`
