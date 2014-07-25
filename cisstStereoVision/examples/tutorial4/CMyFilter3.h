// See license at http://www.cisst.org/cisst/license.txt

#ifndef _CMyFilter3_h
#define _CMyFilter3_h

#include <cisstStereoVision/svlFilterBase.h>

class CMyFilter3 : public svlFilterBase
{
public:
    CMyFilter3();

protected:
    int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);

private:
    unsigned int MaxVal;
};

#endif // _CMyFilter3_h
