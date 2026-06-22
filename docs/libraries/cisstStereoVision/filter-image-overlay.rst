svlFilterImageOverlay class
===========================

svlFilterImageOverlay:: public methods
--------------------------------------

================================================= ===========
Name                                              Description
================================================= ===========
``int AddInputImage(const std::string & name)``   
``int AddInputTargets(const std::string & name)`` 
``int AddInputBlobs(const std::string & name)``   
``int AddInputText(const std::string & name)``    
``void AddOverlay(svlOverlay & overlay)``         
``int AddQueuedItems()``                          
================================================= ===========

Overlay Objects
---------------

=================================== ===========
Class name                          Description
=================================== ===========
``svlOverlayStaticRect``            
``svlOverlayStaticEllipse``         
``svlOverlayStaticTriangle``        
``svlOverlayStaticPoly``            
``svlOverlayStaticBar``             
``svlOverlayStaticText``            
``svlOverlayText``                  
``svlOverlayStaticImage``           
``svlOverlayImage``                 
``svlOverlayTargets``               
``svlOverlayBlobs``                 
``svlOverlayFramerate``             
``svlOverlayTimestamp``             
``svlOverlayAsyncOutputProperties`` 
=================================== ===========

Code samples
------------

-  :doc:`Tutorial part 6 </libraries/cisstStereoVision/tutorials/tutorial-6>`
