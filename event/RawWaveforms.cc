#include "RawWaveforms.h"

#include "TH2I.h"
#include "TH1I.h"
#include "TColor.h"
#include "TDirectory.h"
#include "TString.h"
#include "TPad.h"
#include "TMath.h"

#include <iostream>
#include <vector>
using namespace std;

RawWaveforms::RawWaveforms()
{}

RawWaveforms::RawWaveforms(TH2I *h2, TH1I *h)
{
    // 2D hist: x: channel id; y: tdc 0 - 9600 or 0 - 9600/4
    hOrig = h2;
    fName = hOrig->GetName();
    hBaseline = h;
    nChannels = hOrig->GetNbinsX();
    nTDCs = hOrig->GetNbinsY();
    firstChannel = hOrig->GetXaxis()->GetBinCenter(1);
    SetBaseline();
    // cout << nChannels << ", " << nTDCs << endl;
}

RawWaveforms::~RawWaveforms()
{
}

void RawWaveforms::SetBaseline()
{
    if(hBaseline) return;

    // otherwise calculate baseline myself.
    hBaseline = new TH1I(fName+"_baseline", "", nChannels, -0.5+firstChannel, -0.5+firstChannel+nChannels);
    cout << "calculating baseline for " << fName << " ..." << endl;
    TH1I hf("hf","Bin Frequency",4096,0,4096); //12-bit ADC
    for (int chid=0; chid!=nChannels; chid++) {
        hf.Reset();
        for (int ibin=0; ibin!=nTDCs; ibin++) {
            hf.Fill(int(hOrig->GetBinContent(chid+1, ibin+1)));
        }
        hBaseline->SetBinContent(chid+1, hf.GetMaximumBin()-1);
    }
}

TH1I* RawWaveforms::Draw1D(int chanNo, const char* options)
{
    TString name = TString::Format("hWire_%s", fName.Data());

    TH1I *hWire = (TH1I*)gDirectory->FindObject(name);
    if (hWire) delete hWire;

    hWire = new TH1I(name, "",
        nTDCs,
        hOrig->GetYaxis()->GetBinLowEdge(0),
        hOrig->GetYaxis()->GetBinUpEdge(nTDCs)
    );
    int binNo = hOrig->GetXaxis()->FindBin(chanNo);
    int baseline = hBaseline->GetBinContent(binNo);
    for (int i=1; i<=nTDCs; i++) {
        hWire->SetBinContent(i, hOrig->GetBinContent(binNo, i) - baseline);
    }
    hWire->SetTitle( TString::Format("baseline @%i", baseline) );
    hWire->Draw(options);

    return hWire;
}
