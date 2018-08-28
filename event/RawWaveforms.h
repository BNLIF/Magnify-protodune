#ifndef RAWWAVEFORMS_H
#define RAWWAVEFORMS_H

#include "TString.h"

class TH2I;
class TH1I;

class RawWaveforms {
public:
    RawWaveforms();
    RawWaveforms(TH2I *h2, TH1I *h);
    virtual ~RawWaveforms();

    TH1I* Draw1D(int chanNo, const char* options="");

    TH2I *hOrig;
    TH1I *hBaseline;
    TString fName;


    int nChannels;  // nBinsX
    int nTDCs;       // nBinsY
    int firstChannel;

private:
    void SetBaseline();
};

#endif