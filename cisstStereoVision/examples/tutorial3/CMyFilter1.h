// See license at http://www.cisst.org/cisst/license.txt

#ifndef _CMyFilter1_h
#define _CMyFilter1_h

// [doc-filter-h-start]
#include <cisstStereoVision/svlFilterBase.h>

class CMyFilter1 : public svlFilterBase
{
public:
    CMyFilter1();

protected:
    int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);
};
// [doc-filter-h-end]

#endif // _CMyFilter1_h
