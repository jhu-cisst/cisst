// See license at http://www.cisst.org/cisst/license.txt

#ifndef _CMyFilter2_h
#define _CMyFilter2_h

// [doc-filter2-h-start]
#include <cisstStereoVision/svlFilterBase.h>

class CMyFilter2 : public svlFilterBase
{
public:
    CMyFilter2();

protected:
    int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);
};
// [doc-filter2-h-end]

#endif // _CMyFilter2_h
