// See license at http://www.cisst.org/cisst/license.txt

#ifndef _CMyFilter1_h
#define _CMyFilter1_h

#include <cisstStereoVision/svlFilterBase.h>

class CMyFilter1 : public svlFilterBase
{
public:
    CMyFilter1();

protected:
    int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);
};

#endif // _CMyFilter1_h
