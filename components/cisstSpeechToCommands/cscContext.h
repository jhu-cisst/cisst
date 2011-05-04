/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cscContext.h 2936 2011-04-19 16:32:39Z mkelly9 $

  Author(s):  Martin Kelly, Anton Deguet
  Created on: 2011-02-15

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _cscContext_h
#define _cscContext_h

#include <list>
#include <map>

// forward declaration
class cscSpeechToCommands;

#include <cisstCommon/cmnNamedMap.h>
#include <cisstSpeechToCommands/cscWordActions.h>

// Always include last!
#include <cisstSpeechToCommands/cscExport.h>

class CISST_EXPORT cscContext: public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    friend class cscSpeechToCommands; // to allow speech to command to set pointer to itself

    cscSpeechToCommands * SpeechToCommands;
    typedef cmnNamedMap<cscWordActions> WordMap;
	WordMap Words;
	std::string Name;
    bool OutOfGrammarFiltering;

    bool AddWordWithActions(const std::string & word, cscWordActions * wordActions);

    cscContext(const std::string & name);
    bool PerformActionsForWord(const std::string & word);

 public:
    bool AddWord(const std::string & word);
    bool AddWordWithTransition(const std::string & word, const std::string & endContext);
    const std::string & GetName(void) const;
    std::vector<std::string> GetVocabulary(void);
    bool FilteringEnabled(void);
    void SetFiltering(bool filter);

};

CMN_DECLARE_SERVICES_INSTANTIATION(cscContext);

#endif // _cscContext_h
