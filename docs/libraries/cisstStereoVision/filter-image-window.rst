svlFilterImageWindow class
==========================

The cisstStereoVision library has a built in filter called ``svlFilterImageWindow`` to display image data at any given stage of the stream. By default the filter displays one separate window for each input video channel and renders the images into the windows' client area. Optionally, the filter can be configured to hide the window borders. The underlying platform-dependent API that is used to manage windows automatically choses the best possible image display option from the list of supported methods.

+------------+-------------+------------------------------------+----------------------------------------------------------------------------------------------------------------+
| Platform   | GUI Library | API                                | Notes                                                                                                          |
+============+=============+====================================+================================================================================================================+
| MS Windows | Win32       | ``SetDIBitsToDevice``              |                                                                                                                |
+------------+-------------+------------------------------------+----------------------------------------------------------------------------------------------------------------+
| Linux      | X11 or Xv   | ``XPutImage`` or ``XvShmPutImage`` | Xv is a hardware-accelerated imaging library. When ''Xv'' is available, image refresh is synchronized to VSYNC |
+------------+-------------+------------------------------------+----------------------------------------------------------------------------------------------------------------+
| Mac OS X   | X11         | ``XPutImage``                      | Xv is not available on Mac OS X; Carbon is not used due to the lack of 64 bit support                          |
+------------+-------------+------------------------------------+----------------------------------------------------------------------------------------------------------------+

**Please note**: If you are using a different GUI library in your application then you might need to implement your own image display filter that fits your needs.

svlFilterImageWindow:: public methods
-------------------------------------

+---------------------------------------------------------------+----------------------------------------------------------------------------------------------------------------------------------+
| Name                                                          | Description                                                                                                                      |
+===============================================================+==================================================================================================================================+
| ``int SetPosition(int x, int y, unsigned int videoch)``       | Moves the window to the specified position on the screen. ``x`` and ``y`` are the coordinates of the window's upper left corner. |
+---------------------------------------------------------------+----------------------------------------------------------------------------------------------------------------------------------+
| ``int GetPosition(int & x, int & y, unsigned int videoch)``   | Retrieves the screen coordinates of the upper left corner of the window.                                                         |
+---------------------------------------------------------------+----------------------------------------------------------------------------------------------------------------------------------+
| ``void SetEventHandler(svlWindowEventHandlerBase * handler)`` | Registers a user-specified window event handler object that will receive user (keyboard and mouse) events.                       |
+---------------------------------------------------------------+----------------------------------------------------------------------------------------------------------------------------------+
| ``void SetFullScreen(bool & fullscreen)``                     | Call this method to enable/disable borderless window mode.                                                                       |
+---------------------------------------------------------------+----------------------------------------------------------------------------------------------------------------------------------+
| ``void GetFullScreen(bool & fullscreen)``                     | Retrieves the status of the borderless window mode.                                                                              |
+---------------------------------------------------------------+----------------------------------------------------------------------------------------------------------------------------------+
| ``void SetTitle(std::string & title)``                        | Sets the title of the image window.                                                                                              |
+---------------------------------------------------------------+----------------------------------------------------------------------------------------------------------------------------------+
| ``void GetTitle(std::string & title)``                        | Retrieves the title of the image window.                                                                                         |
+---------------------------------------------------------------+----------------------------------------------------------------------------------------------------------------------------------+

Handling keyboard and mouse inputs
----------------------------------

In order to handle keyboard and mouse inputs inside the image window, the user may register a event handler object to the filter that will receive callbacks upon user events. The custom event handler class needs to be derived from the base class ``svlWindowEventHandlerBase`` declared in ``<cisstStereoVision/svlFilterImageWindow.h>``. User input events are dispatched to the callback method ``OnUserEvent`` that needs to be overloaded by the user in order to perform custom event-handling. Additionally, another callback method called ``OnNewFrame`` may be overloaded by the user. ``OnNewFrame`` is called by the filter once for every video frame before rendering.

The method ``GetMousePos`` may be used to get the position of the mouse cursor. The retrieved cursor position is valid only when the mouse cursor is hovering on top of the window's client area or the mouse focus is currently captured. The '''mouse focus''' becomes captured when the user clicks on the image window and keeps the button depressed. As long as the mouse button remains depressed all the mouse input will be dispatched to the event handler, even if the mouse cursor moves off the window area. The mouse cursor loses the focus when the user releases the button.

svlWindowEventHandlerBase:: callback methods
--------------------------------------------

+-----------------------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Name                                                                        | Description                                                                                                                                                                                                                                                                                         |
+=============================================================================+=====================================================================================================================================================================================================================================================================================================+
| ``void OnUserEvent(unsigned int winid, bool ascii, unsigned int eventide)`` | Called by the image window filter upon every user input event. ``winid`` specifies the video channel for which the event was generated; the flag ``ascii`` is set to ``true`` if ``eventid`` is an ASCII code; ``eventid`` contains either an ASCII code or the code of the user event (see below). |
+-----------------------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| ``void OnNewFrame(unsigned int frameid)``                                   | Called every time a new input image arrives to the filter.                                                                                                                                                                                                                                          |
+-----------------------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

svlWindowEventHandlerBase:: public methods
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

+----------------------------------------+-------------------------------------------------------------------------------------+
| Name                                   | Description                                                                         |
+========================================+=====================================================================================+
| ``void GetMousePos(int & x, int & y)`` | Use this method to retrieve the current position of the mouse cursor on the screen. |
+----------------------------------------+-------------------------------------------------------------------------------------+

User event codes
----------------

========================= ============================================
Event code                Description
========================= ============================================
``winInput_KEY_F1``       ``F1`` key pressed
``winInput_KEY_F2``       ``F2`` key pressed
``winInput_KEY_F3``       ``F3`` key pressed
``winInput_KEY_F4``       ``F4`` key pressed
``winInput_KEY_F5``       ``F5`` key pressed
``winInput_KEY_F6``       ``F6`` key pressed
``winInput_KEY_F7``       ``F7`` key pressed
``winInput_KEY_F8``       ``F8`` key pressed
``winInput_KEY_F9``       ``F9`` key pressed
``winInput_KEY_F10``      ``F10`` key pressed
``winInput_KEY_F11``      ``F11`` key pressed
``winInput_KEY_F12``      ``F12`` key pressed
``winInput_KEY_PAGEUP``   ``Page Up`` key pressed
``winInput_KEY_PAGEDOWN`` ``Page Down`` key pressed
``winInput_KEY_HOME``     ``Home`` key pressed
``winInput_KEY_END``      ``End`` key pressed
``winInput_KEY_INSERT``   ``Insert`` key pressed
``winInput_KEY_DELETE``   ``Delete`` key pressed
``winInput_KEY_LEFT``     Left arrow key pressed
``winInput_KEY_RIGHT``    Right arrow key pressed
``winInput_KEY_UP``       Up arrow key pressed
``winInput_KEY_DOWN``     Down arrow key pressed
``winInput_MOUSEMOVE``    Mouse moved over of the window's client area
``winInput_LBUTTONDOWN``  Left mouse button pressed
``winInput_LBUTTONUP``    Left mouse button released
``winInput_RBUTTONDOWN``  Right mouse button pressed
``winInput_RBUTTONUP``    Right mouse button released
========================= ============================================
