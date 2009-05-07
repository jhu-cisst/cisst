/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2006 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlStreamManager.h>

#define MAX_WAIT_LOCK_TIME      1000

using namespace std;


/*************************************/
/*** svlStreamManager class **********/
/*************************************/

svlStreamManager::svlStreamManager()
{
    ThreadCount = 1;
    ControlInstanceMulti = 0;
    ControlThreadMulti = 0;
    SyncPoint = 0;
    CS = 0;
    StreamSource = 0;
    Initialized = false;
    Running = false;
    Entity.Stream = this;
}

svlStreamManager::svlStreamManager(unsigned int threadcount)
{
    // To do: autodetect the number of available processor cores
    ThreadCount = std::max(1u, threadcount);
    ControlInstanceMulti = 0;
    ControlThreadMulti = 0;
    SyncPoint = 0;
    CS = 0;
    StreamSource = 0;
    Initialized = false;
    Running = false;
    Entity.Stream = this;
}

svlStreamManager::~svlStreamManager()
{
    EmptyFilterList();
}

int svlStreamManager::SetThreads(unsigned int threadcount)
{
    if (Running || threadcount < 1) return SVL_FAIL;
    ThreadCount = threadcount;
    return SVL_OK;
}

svlStreamEntity& svlStreamManager::Trunk()
{
    return Entity;
}

svlStreamEntity& svlStreamManager::Branch(const std::string & name)
{
    // Search for branch
    _BranchMap::iterator iterbranch;
    for (iterbranch = Branches.begin(); iterbranch != Branches.end(); iterbranch ++) {
        if (iterbranch->first->name == name) break;
    }
    if (iterbranch == Branches.end())
        return InvalidEntity;

    return *(iterbranch->first->entity);
}

svlStreamEntity* svlStreamManager::CreateBranchAfterFilter(svlFilterBase* filter)
{
    return CreateBranchAfterFilter(filter, "");
}

svlStreamEntity* svlStreamManager::CreateBranchAfterFilter(svlFilterBase* filter, const std::string & name)
{
    if (Initialized ||
        GetBranchEntityOfFilter(filter) == 0 ||
        svlStreamBranchSource::IsTypeSupported(filter->GetOutputType()) == false)
        return 0;

    // Create branch stream object
    svlStreamManager* branchstream = new svlStreamManager();
    // Create branch structure
    _BranchStruct *branch = new _BranchStruct;
    branch->entity = &(branchstream->Entity);
    branch->name = name;
    // Add branch to the filter's branch list
    filter->OutputBranches.push_back(&(branchstream->Entity));
    // Add branch and the corresponding stream to branch map
    Branches[branch] = branchstream;
    // Create branch stream source
    svlStreamBranchSource* branchsource = new svlStreamBranchSource(filter->GetOutputType());
    // Add branch source to branch stream
    branchstream->AddSourceFilter(branchsource);

    // Return branch entity
    return &(branchstream->Entity);
}

int svlStreamManager::RemoveBranch(svlStreamEntity* entity)
{
    if (entity == 0) return SVL_FAIL;
    if (Initialized) return SVL_ALREADY_INITIALIZED;

    // Search for branch
    _BranchMap::iterator iterbranch;
    for (iterbranch = Branches.begin(); iterbranch != Branches.end(); iterbranch ++) {
        if (iterbranch->first->entity == entity) break;
    }
    if (iterbranch == Branches.end())
        return SVL_FAIL;

    // Empty and delete branch stream
    svlStreamManager* branchstream = iterbranch->second;
    if (branchstream) {
        // Remove every filter in the branch from the stream
        branchstream->EmptyFilterList();
        // Delete branch source
        if (branchstream->StreamSource) delete branchstream->StreamSource;
        // Delete branch stream
        delete branchstream;
    }

    // Delete branch structure
    delete iterbranch->first;
    // Remove branch from branch map
    Branches.erase(iterbranch);

    return SVL_OK;
}

int svlStreamManager::RemoveBranch(const std::string & name)
{
    if (Initialized) return SVL_ALREADY_INITIALIZED;

    // Search for branch
    _BranchMap::iterator iterbranch;
    for (iterbranch = Branches.begin(); iterbranch != Branches.end(); iterbranch ++) {
        if (iterbranch->first->name == name) break;
    }
    if (iterbranch == Branches.end())
        return SVL_FAIL;

    // Empty and delete branch stream
    svlStreamManager* branchstream = iterbranch->second;
    if (branchstream) {
        // Remove every filter in the branch from the stream
        branchstream->EmptyFilterList();
        // Delete branch source
        if (branchstream->StreamSource) delete branchstream->StreamSource;
        // Delete branch stream
        delete branchstream;
    }

    // Delete branch structure
    delete iterbranch->first;
    // Remove branch from branch map
    Branches.erase(iterbranch);

    return SVL_OK;
}

int svlStreamManager::AddFilter(svlFilterBase* filter)
{
    if (filter == 0) return SVL_FAIL;

    Filters.push_back(filter);

    return SVL_OK;
}

int svlStreamManager::RemoveFilter(svlFilterBase* filter)
{
    if (filter == 0) return SVL_FAIL;

    _FilterList::iterator iterfilter;
    svlFilterBase::_OutputBranchList::iterator iterbranchlist;

    for (iterfilter = Filters.begin(); iterfilter != Filters.end(); iterfilter ++) {
        if ((*iterfilter) == filter) {
        // Filter found in this stream
            // Disconnect filter from its neighbors
            if (DisconnectFilterInput(filter)  == SVL_ALREADY_INITIALIZED ||
                DisconnectFilterOutput(filter) == SVL_ALREADY_INITIALIZED)
                return SVL_ALREADY_INITIALIZED;
            // Remove brenches after the filter, if any
            for (iterbranchlist  = filter->OutputBranches.begin();
                 iterbranchlist != filter->OutputBranches.end();
                 iterbranchlist ++) {
                RemoveBranch(*iterbranchlist);
            }
            // Remove filter from the list of filters
            Filters.erase(iterfilter);
            return SVL_OK;
        }
    }

    // Look for the filter in branches as well
    svlStreamManager* branchstream = GetBranchEntityOfFilter(filter)->Stream;
    if (branchstream == 0) return SVL_FILTER_NOT_IN_LIST;

    // Remove brenches after the filter, if any
    for (iterbranchlist  = filter->OutputBranches.begin();
         iterbranchlist != filter->OutputBranches.end();
         iterbranchlist ++) {
        RemoveBranch(*iterbranchlist);
    }

    return branchstream->RemoveFilter(filter);
}

int svlStreamManager::EmptyFilterList()
{
    Release();

    svlFilterBase* filter;
    while (!(Filters.empty())) {
        filter = Filters.front();
        RemoveFilter(filter);
    }

    return SVL_OK;
}

int svlStreamManager::SetSourceFilter(svlFilterBase* filter)
{
    if (filter == 0) return SVL_FAIL;

    if (!(IsFilterInList(filter)))
        return SVL_FILTER_NOT_IN_LIST;

    if (filter->InputType != svlTypeStreamSource)
        return SVL_NOT_SOURCE;

    if (filter == StreamSource)
        return SVL_OK;

    if (Initialized)
        return SVL_ALREADY_INITIALIZED;

    StreamSource = filter;

    return SVL_OK;
}

int svlStreamManager::ConnectFilterToBranch(svlStreamEntity* entity, svlFilterBase* filter)
{
    if (entity == 0 || filter == 0) return SVL_FAIL;
    return ConnectFilters(entity->Stream->StreamSource, filter);
}

int svlStreamManager::ConnectFilters(svlFilterBase* filter1, svlFilterBase* filter2)
{
    if (filter1 == 0 || filter2 == 0) return SVL_FAIL;

    svlStreamEntity* branch1 = GetBranchEntityOfFilter(filter1);
    svlStreamEntity* branch2 = GetBranchEntityOfFilter(filter2);
    if (branch1 == 0 || branch2 == 0) return SVL_FILTER_NOT_IN_LIST;

    // Stream types have to match
    if (filter2->OnConnectInput(filter1->OutputType) == SVL_FAIL)
        return SVL_TYPE_MISMATCH;

    // Only uninitalized filters can be connected
    if (filter1->Initialized || filter2->Initialized)
        return SVL_ALREADY_INITIALIZED;

    // Move right filter to the left filter's branch if needed
    if (branch1 != branch2) {
        RemoveFilter(filter2);
        branch1->Stream->AddFilter(filter2);
    }

    filter1->NextFilter = filter2;
    filter2->PrevFilter = filter1;

    return SVL_OK;
}

int svlStreamManager::AddSourceFilter(svlFilterBase* filter)
{
    if (Initialized) return SVL_ALREADY_INITIALIZED;

    int err = AddFilter(filter);
    if (err != SVL_OK) return err;

    err = SetSourceFilter(filter);
    if (err == SVL_OK) return SVL_OK;

    // Remove filter on error
    RemoveFilter(filter);
    return err;
}

int svlStreamManager::AddFilterAndConnect(svlFilterBase* filter, svlFilterBase* connect_to)
{
    if (Initialized) return SVL_ALREADY_INITIALIZED;

    int err = AddFilter(filter);
    if (err != SVL_OK) return err;

    err = ConnectFilters(connect_to, filter);
    if (err == SVL_OK) return SVL_OK;

    // Remove filter on error
    RemoveFilter(filter);
    return err;
}

int svlStreamManager::DisconnectFilterInput(svlFilterBase* filter)
{
    if (Initialized) return SVL_ALREADY_INITIALIZED;

    if (filter == 0) return SVL_FAIL;

    if (!(IsFilterInList(filter)))
        return SVL_FILTER_NOT_IN_LIST;

    if (filter->Initialized ||
        (filter->PrevFilter != 0 && filter->PrevFilter->Initialized))
        return SVL_ALREADY_INITIALIZED;

    if (filter->PrevFilter != 0) {
        filter->PrevFilter->NextFilter = 0;
        filter->PrevFilter = 0;
    }

    return SVL_OK;
}

int svlStreamManager::DisconnectFilterOutput(svlFilterBase* filter)
{
    if (Initialized) return SVL_ALREADY_INITIALIZED;

    if (filter == 0) return SVL_FAIL;

    if (!(IsFilterInList(filter)))
        return SVL_FILTER_NOT_IN_LIST;

    if (filter->Initialized ||
        (filter->NextFilter != 0 && filter->NextFilter->Initialized))
        return SVL_ALREADY_INITIALIZED;

    if (filter->NextFilter != 0) {
        filter->NextFilter->PrevFilter = 0;
        filter->NextFilter = 0;
    }

    return SVL_OK;
}

int svlStreamManager::GetUpstreamFilter(svlFilterBase* filter, svlFilterBase** upstreamfilter)
{
    if (filter == 0) return SVL_FAIL;

    if (!(IsFilterInList(filter)))
        return SVL_FILTER_NOT_IN_LIST;

    *upstreamfilter = filter->PrevFilter;

    return SVL_OK;
}

int svlStreamManager::GetDownstreamFilter(svlFilterBase* filter, svlFilterBase** downstreamfilter)
{
    if (filter == 0) return SVL_FAIL;

    if (!(IsFilterInList(filter)))
        return SVL_FILTER_NOT_IN_LIST;

    *downstreamfilter = filter->PrevFilter;

    return SVL_OK;
}

int svlStreamManager::Initialize()
{
    if (Initialized) return SVL_ALREADY_INITIALIZED;

    int err;
    svlFilterBase *prevfilter, *filter = StreamSource;
    svlFilterBase::_OutputBranchList::iterator iterbranchlist;
    svlStreamManager* branchstream;

    if (filter == 0)
        return SVL_NO_SOURCE_IN_LIST;

    // Initializing the stream
    // starting from the stream source
    err = filter->Initialize();
    if (err != SVL_OK) {
        Release();
        return err;
    }
    filter->Initialized = true;
    // Initializing branches connected to the
    // filter output recursively, if any
    for (iterbranchlist  = filter->OutputBranches.begin();
         iterbranchlist != filter->OutputBranches.end();
         iterbranchlist ++) {
        // Setup input sample
        branchstream = (*iterbranchlist)->Stream;
        dynamic_cast<svlStreamBranchSource*>(branchstream->StreamSource)->SetupSource(filter->OutputData, 100.0);
        // Initialize branch stream
        err = branchstream->Initialize();
        if (err != SVL_OK) {
            Release();
            return err;
        }
    }
    prevfilter = filter;
    filter = filter->NextFilter;

    // Going downstream filter by filter
    while (filter != 0) {
        // Check if the previous output is valid input for the next filter
        err = filter->IsDataValid(filter->InputType, prevfilter->OutputData);
        if (err != SVL_OK) {
            Release();
            return err;
        }
        err = filter->Initialize(prevfilter->OutputData);
        if (err != SVL_OK) {
            Release();
            return err;
        }
        filter->OutputFormatModified = false;
        filter->Initialized = true;
        // Initializing branches connected to the
        // filter output recursively, if any
        for (iterbranchlist  = filter->OutputBranches.begin();
             iterbranchlist != filter->OutputBranches.end();
             iterbranchlist ++) {
            // Setup input sample
            branchstream = (*iterbranchlist)->Stream;
            dynamic_cast<svlStreamBranchSource*>(branchstream->StreamSource)->SetupSource(filter->OutputData, 100.0);
            // Initialize branch stream
            err = branchstream->Initialize();
            if (err != SVL_OK) {
                Release();
                return err;
            }
        }
        prevfilter = filter;
        filter = filter->NextFilter;
    }

    Initialized = true;

    return SVL_OK;
}

void svlStreamManager::Release()
{
    Stop();

    _FilterList::iterator iterfilter;
    svlFilterBase::_OutputBranchList::iterator iterbranchlist;

    // To make sure that no filter is left initialized
    // we try to release all filters in the list, not only
    // those are in the stream
    for (iterfilter = Filters.begin(); iterfilter != Filters.end(); iterfilter ++) {
        if ((*iterfilter)->Initialized) {
            (*iterfilter)->Release();
            (*iterfilter)->Initialized = false;
        }
        // Release branches connected to the filters recursively, if any
        for (iterbranchlist  = (*iterfilter)->OutputBranches.begin();
             iterbranchlist != (*iterfilter)->OutputBranches.end();
             iterbranchlist ++) {
            // Release branch stream
            (*iterbranchlist)->Stream->Release();
        }
    }

    Initialized = false;
}

bool svlStreamManager::IsInitialized()
{
    return Initialized;
}

int svlStreamManager::Start()
{
    if (Running) return SVL_ALREADY_RUNNING;

    int err;
    _FilterList::iterator iterfilter;
    svlFilterBase::_OutputBranchList::iterator iterbranchlist;
    svlStreamManager* branchstream;

    if (!Initialized) {
        // Try to initialize it if it hasn't been done before
        err = Initialize();
        if (err != SVL_OK) return err;
    }

    Running = true;

    for (iterfilter = Filters.begin(); iterfilter != Filters.end(); iterfilter ++) {
        (*iterfilter)->Running = true;
        if ((*iterfilter)->OnStart(ThreadCount) != SVL_OK) {
            Stop();
            return SVL_FAIL;
        }
    }

    // Allocate thread control object array
    ControlInstanceMulti = new svlStreamControlMultiThread*[ThreadCount];
    ControlThreadMulti = new osaThread*[ThreadCount];
    // Create thread synchronization object
    if (ThreadCount > 1) {
        SyncPoint = new svlSyncPoint;
        SyncPoint->Count(ThreadCount);
        CS = new osaCriticalSection;
    }

    for (unsigned int i = 0; i < ThreadCount; i ++) {
        // Starting multi thread processing
        ControlInstanceMulti[i] = new svlStreamControlMultiThread(ThreadCount, i);
        ControlThreadMulti[i] = new osaThread;

        StopThread = false;
        ErrorOnThread = false;
        ControlThreadMulti[i]->Create<svlStreamControlMultiThread, svlStreamManager*>(ControlInstanceMulti[i],
                                                                                      &svlStreamControlMultiThread::Proc,
                                                                                      this);
    }

    // Start branches connected to the filters recursively, if any
    for (iterfilter = Filters.begin(); iterfilter != Filters.end(); iterfilter ++) {
        for (iterbranchlist  = (*iterfilter)->OutputBranches.begin();
             iterbranchlist != (*iterfilter)->OutputBranches.end();
             iterbranchlist ++) {
            // Get branch stream object from handle
            branchstream = (*iterbranchlist)->Stream;
            // Set thread count in branches
            branchstream->SetThreads(ThreadCount);
            // Start branch stream
            err = branchstream->Start();
            if (err != SVL_OK) {
                Release();
                return err;
            }
        }
    }

    return SVL_OK;
}

void svlStreamManager::Stop()
{
    if (!Running) return;

    _FilterList::iterator iterfilter;
    svlFilterBase::_OutputBranchList::iterator iterbranchlist;

    // Stop branches connected to the filters recursively, if any
    for (iterfilter = Filters.begin(); iterfilter != Filters.end(); iterfilter ++) {
        for (iterbranchlist  = (*iterfilter)->OutputBranches.begin();
             iterbranchlist != (*iterfilter)->OutputBranches.end();
             iterbranchlist ++) {
            // Stop branch stream
            (*iterbranchlist)->Stream->Stop();
        }
    }

    Running = false;
    StopThread = true;

    for (iterfilter = Filters.begin(); iterfilter != Filters.end(); iterfilter ++) {
        (*iterfilter)->OnStop();
        (*iterfilter)->Running = false;
    }

    // Stopping multi thread processing and delete thread objects
    for (unsigned int i = 0; i < ThreadCount; i ++) {
        if (ControlThreadMulti[i]) {
            ControlThreadMulti[i]->Wait();
            delete ControlThreadMulti[i];
        }
        if (ControlInstanceMulti[i]) {
            delete ControlInstanceMulti[i];
        }
    }
    // Release thread control arrays and objects
    if (ControlThreadMulti) {
        delete [] ControlThreadMulti;
        ControlThreadMulti = 0;
    }
    if (ControlInstanceMulti) {
        delete [] ControlInstanceMulti;
        ControlInstanceMulti = 0;
    }
    if (SyncPoint) {
        delete SyncPoint;
        SyncPoint = 0;
    }
    if (CS) {
        delete CS;
        CS = 0;
    }
}

bool svlStreamManager::IsRunning()
{
    return Running;
}

int svlStreamManager::AppendFilterToTrunk(svlFilterBase* filter)
{
    if (Initialized) return SVL_ALREADY_INITIALIZED;

    int err = AddFilter(filter);
    if (err != SVL_OK) return err;

    if (StreamSource == 0) {
        // No source filters set yet, so the
        // filter has to be a source filter
        err = SetSourceFilter(filter);
        if (err == SVL_OK) return SVL_OK;

        // Remove filter on error
        RemoveFilter(filter);
        return SVL_FAIL;
    }

    // If source has already been set, the filter
    // has to connect to the end of the trunk

    // Finding the end of the trunk
    svlFilterBase *endfilter = StreamSource;

    // Going downstream filter by filter
    while (endfilter->NextFilter != 0) endfilter = endfilter->NextFilter;

    // Now, try to add filter to the end of the trunk
    err = ConnectFilters(endfilter, filter);
    if (err == SVL_OK) return SVL_OK;

    // Remove filter on error
    RemoveFilter(filter);
    return err;
}

svlStreamEntity* svlStreamManager::GetBranchEntityOfFilter(svlFilterBase* filter)
{
    if (filter == 0) return 0;

    if (IsFilterInList(filter))
        return &Entity;

    // Search in branches
    _BranchMap::iterator iterbranch;
    for (iterbranch = Branches.begin(); iterbranch != Branches.end(); iterbranch ++) {
        if (iterbranch->second->IsFilterInList(filter))
            return iterbranch->first->entity;
    }

    return 0;
}

bool svlStreamManager::IsFilterInList(svlFilterBase* filter)
{
    if (filter == 0) return false;

    _FilterList::iterator iterfilter;

    for (iterfilter = Filters.begin(); iterfilter != Filters.end(); iterfilter ++) {
        if ((*iterfilter) == filter) {
            return true;
        }
    }

    return false;
}

int svlStreamManager::ReInitializeDownstreamFilters(svlFilterBase* fromfilter)
{
    // Called from trusted source, no need to check
    int err;
    svlFilterBase *prevfilter = fromfilter;
    svlFilterBase *filter = fromfilter->NextFilter;

    // Going downstream filter by filter
    while (filter != 0) {
        err = filter->Initialize(prevfilter->OutputData);
        if (err != SVL_OK) {
            Release();
            return err;
        }
        prevfilter = filter;
        filter = filter->NextFilter;
    }

    return SVL_OK;
}


/*******************************************/
/*** svlStreamEntity class *****************/
/*******************************************/

svlStreamEntity::svlStreamEntity() :
    Stream(0)
{
}

int svlStreamEntity::Append(svlFilterBase* filter)
{
    if (Stream == 0 || filter == 0) return SVL_FAIL;
    return Stream->AppendFilterToTrunk(filter);
}


/*******************************************/
/*** svlStreamControlMultiThread class *****/
/*******************************************/

svlStreamControlMultiThread::svlStreamControlMultiThread()
{ /* Private default constructor never called */ }

svlStreamControlMultiThread::svlStreamControlMultiThread(unsigned int threadcount, unsigned int threadid) :
    ThreadID(threadid),
    ThreadCount(threadcount)
{ /* NOP */ }

svlStreamControlMultiThread::~svlStreamControlMultiThread()
{ /* NOP */ }

double svlStreamControlMultiThread::GetAbsoluteTime(osaTimeServer* timeserver)
{
    if (!timeserver) return -1.0;

    osaAbsoluteTime abstime;

    timeserver->RelativeToAbsolute(timeserver->GetRelativeTime(), abstime);

    // Calculate time since UNIX creation
    return abstime.sec + abstime.nsec / 1000000000.0;
}

void* svlStreamControlMultiThread::Proc(svlStreamManager* baseref)
{
    svlFilterBase *filter, *prevfilter;
    svlFilterBase::_OutputBranchList::iterator iterbranchlist;
    svlFilterBase::ProcInfo info;
    svlSyncPoint *sync = baseref->SyncPoint;
    unsigned int counter = 0;
    osaTimeServer* timeserver = 0;
    int err = SVL_OK;

    // Initializing thread info structure
    info.count = ThreadCount;
    info.id    = ThreadID;
    info.sync  = sync;
    info.cs    = baseref->CS;

    if (ThreadID == 0) {
    // Execute only on one thread - BEGIN

        // Initialize time server for accessing absolute time
        timeserver = new osaTimeServer;
        timeserver->SetTimeOrigin();

    // Execute only on one thread - END
    }

    while (baseref->StopThread == false) {
        filter = baseref->StreamSource;
        filter->FrameCounter = counter;

    ////////////////////////////////////
    // Starting from the stream source

        err = filter->ProcessFrame(&info);
        if (err < 0) break;

        if (ThreadID == 0) {
        // Execute only on one thread - BEGIN

            if (filter->OutputData) {
                // Get timestamp and assign it to the output sample
                filter->OutputData->SetTimestamp(GetAbsoluteTime(timeserver));
            }

        // Execute only on one thread - END
        }

        // Check if the filter modified the output sample
        if (err == SVL_ALREADY_PROCESSED) filter->OutputSampleModified = false;
        else filter->OutputSampleModified = true;

        if (ThreadCount > 1) {
        // Execute only if multi-threaded - BEGIN

            // Synchronization point, wait for other thread
            if (sync->Sync(ThreadID) != SVL_SYNC_OK) break;

        // Execute only if multi-threaded - END
        }

        // Check for errors and stop request
        if (baseref->StopThread || baseref->ErrorOnThread) break;

        if (filter->OutputData) {
            // Set modified flag
            filter->OutputData->SetModified(filter->OutputSampleModified);
        }

        if (ThreadID == 0) {
        // Execute only on one thread - BEGIN

            // Feed sample to branches, if any
            for (iterbranchlist  = filter->OutputBranches.begin();
                iterbranchlist != filter->OutputBranches.end();
                iterbranchlist ++) {
                dynamic_cast<svlStreamBranchSource*>((*iterbranchlist)->Stream->StreamSource)->PushSample(filter->OutputData);
            }

        // Execute only on one thread - END
        }

        prevfilter = filter;
        filter = filter->NextFilter;

    ////////////////////////////////////////////
    // Going downstream filter by filter

        while (filter != 0) {
            filter->FrameCounter = counter;

            // Check if the previous output is valid input for the next filter
            err = filter->IsDataValid(filter->InputType, prevfilter->OutputData);
            if (err != SVL_OK) break;

            err = filter->ProcessFrame(&info, prevfilter->OutputData);
            if (err < 0) break;

            // Check if the filter modified the output sample
            if (err == SVL_ALREADY_PROCESSED) filter->OutputSampleModified = false;
            else filter->OutputSampleModified = true;

            if (ThreadCount > 1) {
            // Execute only if multi-threaded - BEGIN

                // Synchronization point, wait for other thread
                if (sync->Sync(ThreadID) != SVL_SYNC_OK) break;

            // Execute only if multi-threaded - END
            }

            // Check for errors and stop request
            if (baseref->StopThread || baseref->ErrorOnThread) break;

            if (filter->OutputData) {
                // Set modified flag
                filter->OutputData->SetModified(filter->OutputSampleModified);

                // Store timstamp of previously processed input sample
                filter->PrevInputTimestamp = prevfilter->OutputData->GetTimestamp();
            }

            if (ThreadID == 0) {
            // Execute only on one thread - BEGIN

                // Feed sample to branches, if any
                for (iterbranchlist  = filter->OutputBranches.begin();
                     iterbranchlist != filter->OutputBranches.end();
                     iterbranchlist ++) {
                     dynamic_cast<svlStreamBranchSource*>((*iterbranchlist)->Stream->StreamSource)->PushSample(filter->OutputData);
                }

            // Execute only on one thread - END
            }

            prevfilter = filter;
            filter = filter->NextFilter;
        }
        if (err < 0) break;

        // incrementing frame counter
        counter ++;
    }

    if (ThreadID == 0) {
    // Execute only on one thread - BEGIN

        // Delete time server
        if (timeserver) delete timeserver;

    // Execute only on one thread - END
    }

    // Signal the error status
    if (err < 0) baseref->ErrorOnThread = true;

    if (ThreadCount > 1) {
    // Execute only if multi-threaded - BEGIN

        sync->ReleaseAll();

    // Execute only if multi-threaded - END
    }

	return this;
}


/*************************************/
/*** svlFilterBase class *************/
/*************************************/

svlFilterBase::svlFilterBase()
{
    InputType = svlTypeInvalid;
    OutputType = svlTypeInvalid;
    PrevFilter = 0;
    NextFilter = 0;
    OutputData = 0;
    Initialized = false;
    Running = false;
    PrevInputTimestamp = -1.0;
    OutputFormatModified = false;
}

svlFilterBase::~svlFilterBase()
{
}

int svlFilterBase::Initialize(svlSample* CMN_UNUSED(inputdata))
{
    Initialized = true;
    return SVL_OK;
}

int svlFilterBase::OnStart(unsigned int CMN_UNUSED(procCount))
{
    return SVL_OK;
}

int svlFilterBase::ProcessFrame(ProcInfo* CMN_UNUSED(procInfo),
                                svlSample* CMN_UNUSED(inputdata))
{
    return SVL_OK;
}

void svlFilterBase::OnStop()
{
}

int svlFilterBase::IsDataValid(svlStreamType type, svlSample* data)
{
    if (data == 0) return SVL_NO_INPUT_DATA;
    if (type != data->GetType()) return SVL_INVALID_INPUT_TYPE;
    if (data->IsInitialized() == false) return SVL_NO_INPUT_DATA;
    return SVL_OK;
}

bool svlFilterBase::IsNewSample(svlSample* sample)
{
    return sample->IsModified();
}

int svlFilterBase::Release()
{
    Initialized = false;
    return SVL_OK;
}

bool svlFilterBase::IsInitialized()
{
    return Initialized;
}

bool svlFilterBase::IsRunning()
{
    return Running;
}

svlStreamType svlFilterBase::GetInputType()
{
    return InputType;
}

svlStreamType svlFilterBase::GetOutputType()
{
    return OutputType;
}

svlFilterBase* svlFilterBase::GetUpstreamFilter()
{
    return PrevFilter;
}

svlFilterBase* svlFilterBase::GetDownstreamFilter()
{
    return NextFilter;
}

void svlFilterBase::SetFilterToSource(svlStreamType output)
{
    SupportedTypes.clear();
    InputType = svlTypeStreamSource;
    OutputType = output;
}

int svlFilterBase::AddSupportedType(svlStreamType input, svlStreamType output)
{
    if (InputType == svlTypeStreamSource)
        return SVL_FAIL;

    SupportedTypes[input] = output;
    return SVL_OK;
}

void svlFilterBase::UpdateOutputFormat()
{
    OutputFormatModified = true;
}

int svlFilterBase::OnConnectInput(svlStreamType inputtype)
{
    _StreamTypeMap::iterator iter;

    iter = SupportedTypes.find(inputtype);
    if (iter == SupportedTypes.end())
        return SVL_FAIL;

    InputType = inputtype;
    OutputType = iter->second;
    return SVL_OK;
}

