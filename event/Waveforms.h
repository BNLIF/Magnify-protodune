#ifndef WAVEFORMS_H
#define WAVEFORMS_H

#include <vector>
#include "TString.h"

class TH2F;
class TH1F;
class TH1I;
class TBox;
class TLine;
class BadChannels;

class Waveforms {
public:
    Waveforms();
    Waveforms(TH2F *h, BadChannels* v, TString name="", TString title="", double scale=1, double threshold=600);
    virtual ~Waveforms();

    void SetThreshold(double x);
    void SetThreshold(TH1I* h, double scaling=1);

    void SetZRange(int min, int max);
    void Draw2D();
    TH1F* Draw1D(int chanNo, const char* options="");
    TH1F* Draw1DTick(int chanNo, const char* options="");
    void Clear();
    void DrawLines();
    void HideLines();
    int GetPlaneNo(int chanNo);

    TH2F *hOrig;
    TH2F *hDummy;
    vector<TBox*> boxes;
    vector<float> box_values;
    // vector<int>* bad_channels;
    BadChannels *bad_channels;
    vector<TLine*> lines;
    vector<TLine*> apa_lines;


    int nChannels;  // nBinsX
    int nTDCs;       // nBinsY
    int firstChannel;
    double fScale;    // electron to
    TString fName;
    TString fTitle;
    int planeNo; // 0,1,2
    double threshold;
    bool useChannelThreshold;
    int zmin;
    int zmax;
    bool isDecon;
};

#endif