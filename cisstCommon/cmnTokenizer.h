/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Ofri Sadowsky
  Created on:	2003-06-09

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file cmnTokenizer.h
  \brief Break strings into tokens.
*/
#pragma once

#ifndef _cmnTokenizer_h
#define _cmnTokenizer_h


#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnThrow.h>

#include <vector>

// Always the last cisst include
#include <cisstCommon/cmnExport.h>

/*!  cmnTokenizer provides a convenient interface for parsing a string
  into a set of tokens.  The parsing uses several sets of control
  characters:

  Delimiters: Separate tokens.  A sequence of delimiters which is not
  quoted or escaped is ignored, and a new token begins after it.

  Quote markers: Enclose parts of, or complete, tokens.  Anything
  between a pair of identical quote markers is included in a token.

  Escape markers: Quote the character immediately following them.

  The default values for delimiters is whitespace (space, tab, CR);
  for quote markers is the set of double and single quotation marks
  (",'); for escape markers is backslash (\).  But the user can
  override them by calling the appropriate method.

  A cmnTokenizer object maintains an internal copy of the tokenized
  text, and can return a pointer to an array of pointer, after the
  fashion of argv.

  \note It is important to note that the stored tokens have the life
  span of the tokenizer.  If the tokenizer is destroyed, the user
  cannot access any of the tokens. */
class CISST_EXPORT cmnTokenizer
{
 public:

    typedef std::vector<const char *> TokensContainer;
    typedef TokensContainer::size_type size_type;

    cmnTokenizer();

    ~cmnTokenizer();

    inline void SetDelimiters(const char * delimiters)
    {
        Delimiters = delimiters;
    }

    inline void SetQuoteMarkers(const char * markers)
    {
        QuoteMarkers = markers;
    }

    inline void SetEscapeMarkers(const char * markers)
    {
        EscapeMarkers = markers;
    }

    inline const char * GetDelimiters(void) const
    {
        return Delimiters;
    }

    inline const char * GetQuoteMarkers(void) const
    {
        return QuoteMarkers;
    }

    inline const char * GetEscapeMarkers(void) const
    {
        return EscapeMarkers;
    }

    static const char * GetDefaultDelimiters(void)
    {
        return DefaultDelimiters;
    }

    static const char * GetDefaultQuoteMarkers(void)
    {
        return DefaultQuoteMarkers;
    }

    static const char * GetDefaultEscapeMarkers(void)
    {
        return DefaultEscapeMarkers;
    }

    /*! Parse the input string and store the tokens internally.
      If there is a syntax error (e.g., unclosed quotes) throw
      an exception.
      \param string the text to be parsed.
    */
    void Parse(const char * string) CISST_THROW(std::runtime_error);


    inline void Parse(const std::string & string) CISST_THROW(std::runtime_error) {
        Parse(string.c_str());
    }


    /*! Return the number of tokens stored. Note that the last token is a NULL. */
    size_type GetNumTokens(void) const {
        return Tokens.size();
    }

    /*! Return the specified token.
        \param num the token number (0..GetNumTokens())
        \returns pointer to token string (0 if not valid)
    */
    const char * GetToken(size_type num) const;

    /*!
      Return the array of tokens in an argv fashion.

      \note This parsing returns exactly the tokens in the input
      string.  For an argv-style set of argument, one needs to have
      the "name of the program" argument in index 0, and the arguments
      starting at index 1.  Use the method GetArgvTokens() for
      that.
    */
    const char * const * GetTokensArray(void) const;

    /*! This method will fill the input vector with the tokens, but
      first set the 0-index element to NULL, to follow the argv
      convention, where argv[0] contains the "name of the program". */
    void GetArgvTokens(std::vector<const char *> & argvTokens) const;

private:
    const char * Delimiters;
    const char * QuoteMarkers;
    const char * EscapeMarkers;

    static const char * const DefaultDelimiters;
    static const char * const DefaultQuoteMarkers;
    static const char * const DefaultEscapeMarkers;

    std::vector<char> StringBuffer;
    TokensContainer Tokens;
};

#endif  // _cmnTokenizer_h

