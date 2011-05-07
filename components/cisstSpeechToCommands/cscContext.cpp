/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cscContext.cpp 2936 2011-04-19 16:32:39Z mkelly9 $

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

#include <map>
#include "cscContext.h"

CMN_IMPLEMENT_SERVICES(cscContext);

#include <cisstCommon/cmnAssert.h>

cscContext::cscContext(const std::string & name):
    Name(name),
    OutOfGrammarFiltering(false)
{
    Words.SetOwner(*this);
}


bool cscContext::AddWordWithActions(const std::string & word, cscWordActions * wordActions)
{
    if (Words.AddItem(word, wordActions, CMN_LOG_LOD_INIT_ERROR)) {
        CMN_LOG_CLASS_INIT_DEBUG << "AddWord: added word \"" << word << "\""
                                 << " to context \"" << this->GetName() << "\"" << std::endl;
        return true;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddWord: failed to add word \"" << word << "\""
                             << " to context \"" << this->GetName() << "\"" << std::endl;
    return false;
}

bool cscContext::AddWord(const std::string & word)
{
    cscWordActions * wordActions = new cscWordActions();
    if (this->AddWordWithActions(word, wordActions)) {
        return true;
    }
    delete wordActions;
    return false;
}


bool cscContext::AddWordWithTransition(const std::string & word, const std::string & endContext)
{
    cscWordActions * wordActions = new cscWordActions(endContext);
    if (this->AddWordWithActions(word, wordActions)) {
        return true;
    }
    delete wordActions;
    return false;
}


const std::string & cscContext::GetName(void) const
{
	return this->Name;
}


std::vector<std::string> cscContext::GetVocabulary(void)
{
    std::vector<std::string> result;
    Words.GetNames(result);
	return result;
}


bool cscContext::PerformActionsForWord(const std::string & word)
{
    cscWordActions * wordActions = this->Words.GetItem(word);
    if (wordActions) {
        CMN_LOG_CLASS_RUN_DEBUG << "PerformActionsForWord: performing actions for word \"" << word << "\"" << std::endl;
        CMN_ASSERT(this->SpeechToCommands);
        wordActions->PerformActions(this->SpeechToCommands);
        return true;
    }
    CMN_LOG_CLASS_RUN_ERROR << "PerformActionsForWord: can't find word \"" << word
                            << "\" in context \"" << this->GetName() << "\"" << std::endl;
    return false;
}

bool cscContext::FilteringEnabled(void)
{
    return OutOfGrammarFiltering;
}

void cscContext::SetFiltering(bool filter)
{
    OutOfGrammarFiltering = filter;
}
