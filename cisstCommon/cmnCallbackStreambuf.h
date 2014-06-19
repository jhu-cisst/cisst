/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides
  Created on: 2006-01-13

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

/*! \file
    \brief Declaration of the class cmnCallbackStreambuf
*/
#pragma once

#ifndef _cmnCallbackStreambuf_h
#define _cmnCallbackStreambuf_h

#include <cisstCommon/cmnPortability.h>
#include <iostream>

/*!
  \brief A Streambuffer class that outputs via a callback function.

  This file declares *class cmnCallbackStreambuf*. It is a templated
  class derived from the standard library's basic_streambuf, with the
  additional feature that it outputs to a user-specified callback function.
  This is implemented by overriding the basic_streambuf output functions
  xsputn(), overflow() and sync().

  Usage:
  Include the module in your application with:
  \#include <cisstCommon/cmnCallbackStreambuf.h>

  Specify the callback function in the constructor.  Note that the callback
  function should accept two parameters:
     -# an array of elements to be printed (e.g., a character array)
     -# the number of elements to be printed

  The callback function is invoked when a newline character is encountered
  or when the internal buffer is full.

  This class could be used, for example, to redirect *cmnLogger* output
  to a GUI text window, assuming that the GUI text window provides a
  "WriteText" function.  This class would not be needed, however, if
  the GUI text window is derived from the streambuf class.

  Note:  It is not clear whether the comparison to '\n' (for the newline)
  will work if ElementType is not a char.

   \sa C++ manual on basic_ostream and basic_streambuf.
   */

template<class _element, class _trait = std::char_traits<_element> >
class cmnCallbackStreambuf : public std::basic_streambuf<_element, _trait>
{
public:
    typedef typename std::basic_streambuf<_element, _trait>::int_type int_type;
	/*! Type of the array, e.g., char. */
	typedef _element ElementType;
	/*! Type of the callback function, e.g., void func(char *line, int len). */
    typedef void (*CallbackType)(const ElementType *, int len);

    /*!
     * Constructor: Sets the callback function.
     *
	 * \param func The callback function.
     */
    cmnCallbackStreambuf(CallbackType func) : Idx(0), Callback(func) { }

protected:

    ElementType Buffer[256];
    unsigned int Idx;
    CallbackType Callback;

    /*! Protected function to print the buffer (call the callback function). */
    virtual void PrintLine() {  Buffer[Idx] = 0; Callback(Buffer, Idx); Idx = 0; }

    /*! Override the basic_streambuf overflow to store the character in the buffer.
	    The buffer is printed if the character is a newline or if it is full. */
	virtual int_type overflow( int_type c)
    {
        // follow the basic_streambuf standard
        if (_trait::eq_int_type(_trait::eof(), c))
            return (_trait::not_eof(c));

        // This code assumes that the buffer can never be full
        // on entry because full buffers are always printed.
        Buffer[Idx++] = c;
        if (c == '\n')
            PrintLine();   // We got a newline, print the buffer
        else if (Idx >= sizeof(Buffer)-1)
            PrintLine();   // Buffer is full, print it

        // follow the basic_streambuf standard
        return _trait::not_eof(c);
    }

    /*! Override the basic_streambuf xsputn to store the characters in the buffer.
	    The buffer is printed every time a newline is encountered or the buffer becomes full. */
    virtual std::streamsize xsputn(const ElementType* s, std::streamsize n)
    {
		std::streamsize todo = n;
        while (todo > 0) {
			std::streamsize tmp = static_cast<std::streamsize>(sizeof(Buffer)-Idx-1);
			std::streamsize limit = (todo < tmp) ? todo : tmp;
			for (std::streamsize i=0; i < limit; i++) {
                Buffer[Idx++] = *s;
                if (*s == '\n')
                    PrintLine();
                s++;
            }
            todo -= limit;
            // If buffer is full, print it
            if (Idx >= sizeof(Buffer)-1)
                PrintLine();
        }
        // If buffer is full, print it
        if (Idx >= sizeof(Buffer)-1)
            PrintLine();
        return n;
    }

    /*! Override the basic_streambuf sync to flush (print) the buffer. */
    virtual int sync()
    {
        if (Idx > 0)
            PrintLine();
        return 0;
    }
};

#endif // _cmnCallbackStreambuf_h
