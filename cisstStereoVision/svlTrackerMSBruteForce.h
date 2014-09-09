/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs Vagvolgyi
  Created on: 2007

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlTrackerMSBruteForce_h
#define _svlTrackerMSBruteForce_h

#include <cisstStereoVision/svlFilterImageTracker.h>
#include <cisstStereoVision/svlSeries.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlTrackerMSBruteForce : public svlImageTracker
{
public:
    svlTrackerMSBruteForce();
    virtual ~svlTrackerMSBruteForce();

    void SetErrorMetric(svlErrorMetric metric);
    void SetScales(unsigned int  scales);
    void SetTemplateRadius(unsigned int radius);
    void SetSearchRadius(unsigned int radius);
    void SetOverwriteTemplates(bool enable);
    void SetTemplateUpdate(bool enable);
    void SetTemplateUpdateWeight(double weight);
    void SetConfidenceThreshold(double threshold);
    void SetHighPassFilterRadius(int radius);
    void SetHighPassFilterStrength(double strength);
    void SetNoiseFilterRadius(int radius);

    svlErrorMetric GetErrorMetric() const;
    unsigned int GetScales() const;
    unsigned int GetTemplateRadius() const;
    unsigned int GetSearchRadius() const;
    bool GetOverwriteTemplates() const;
    bool GetTemplateUpdate() const;
    double GetTemplateUpdateWeight() const;
    double GetConfidenceThreshold() const;
    int GetHighPassFilterRadius() const;
    double GetHighPassFilterStrength() const;
    int GetNoiseFilterRadius() const;

    int GetFeatureRef(unsigned int targetid, vctDynamicVectorRef<unsigned char> & feature);
    int GetImageRef(unsigned int targetid, vctDynamicVectorRef<unsigned char> & feature);

    virtual int GetROIMargin();
    virtual int SetTarget(unsigned int targetid, const svlTarget2D & target);
    virtual int Initialize();
    virtual void ResetTargets();
    virtual int PreProcessImage(svlSampleImage & image, unsigned int videoch = SVL_LEFT);
	virtual int Track(svlSampleImage & image, unsigned int videoch = SVL_LEFT);
    virtual int Track(svlProcInfo* procInfo, svlSampleImage & image, unsigned int videoch = SVL_LEFT);
    virtual void Release();

protected:
    std::string ScaleName;

    bool TargetsAdded;
    bool OverwriteTemplates;
    bool TemplateUpdateEnabled;
    unsigned int FrameCounter;
    unsigned int ThreadCounter;
    unsigned int TemplateRadiusRequested;
    unsigned int SearchRadiusRequested;
    unsigned int TemplateRadius;
    unsigned int SearchRadius;
    vctDynamicMatrix<int> MatchMap;
    vctFixedSizeVector<vctDynamicMatrix<unsigned int>, 3> SumTable;
    vctFixedSizeVector<vctDynamicMatrix<unsigned int>, 3> SqSumTable;
    vctFixedSizeVector<vctDynamicVector<int>, 128> ZeroMeanTemplate;

    int HighPassFilterRadius;
    double HighPassFilterStrength;
    int NoiseFilterRadius;

    svlErrorMetric Metric;
    unsigned int Scale;
    unsigned char TemplateUpdateWeight;
    unsigned char ConfidenceThreshold;
    svlTrackerMSBruteForce* LowerScale;
    svlSampleImageRGB* PreProcessedImage;
    svlSampleImageRGB* PreProcessedImageTemp;
    svlSampleImageRGB* LowerScaleImage;
    svlSampleImageRGB* PreviousRawImage;
    svlSampleImageRGB* PreviousPreProcessedImage;

    virtual void CopyTemplate(unsigned char* img, unsigned char* tmp, unsigned int left, unsigned int top);
    virtual void UpdateTemplate(unsigned char* img, unsigned char* tmp, unsigned int left, unsigned int top);
    virtual void MatchTemplateSAD(unsigned char* img, unsigned char* tmp, int x, int y);
    virtual void MatchTemplateSSD(unsigned char* img, unsigned char* tmp, int x, int y);
    virtual void MatchTemplateNCC(unsigned char* img, unsigned char* tmp, int x, int y);
    virtual void MatchTemplateFastNCC(unsigned char* img, unsigned char* tmp, int* zero_mean_tmp, int x, int y);
    virtual void MatchTemplateNotQuiteNCC(unsigned char* img, unsigned char* tmp, int x, int y);
    virtual void GetBestMatch(int &x, int &y, unsigned char &conf, bool higherbetter);
    virtual void ShrinkImage(unsigned char* src, unsigned char* dst);
    virtual void CalculateSumTables(unsigned char* img);
};

#endif //_svlTrackerMSBruteForce_h

