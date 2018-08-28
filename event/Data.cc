#include "Data.h"
#include "Waveforms.h"
#include "RawWaveforms.h"
#include "BadChannels.h"

#include "TH2F.h"
#include "TH2I.h"
#include "TH1I.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"

#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>

using namespace std;


Data::Data()
{}

Data::Data(const char* filename, double threshold, const char* frame, int rebin)
{
    rootFile = TFile::Open(filename);
    if (!rootFile) {
    	string msg = "Unable to open ";
    	msg += filename;
    	throw runtime_error(msg.c_str());
    }
    bad_channels = new BadChannels( (TTree*)rootFile->Get("T_bad") );
    load_runinfo();

    load_waveform("hu_raw", "U Plane (Denoised)", 1., threshold);
    load_waveform("hv_raw", "V Plane (Denoised)", 1., threshold);
    load_waveform("hw_raw", "W Plane (Denoised)", 1., threshold);

    for (int iplane=0; iplane<3; ++iplane) {
        load_waveform(Form("h%c_%s", 'u'+iplane, frame),
                      Form("%c Plane (Deconvoluted)", 'U'+iplane), 1./(500.*rebin/4.0), threshold);
    }

    load_rawwaveform("hu_orig", "hu_baseline");
    load_rawwaveform("hv_orig", "hv_baseline");
    load_rawwaveform("hw_orig", "hw_baseline");

    load_threshold("hu_threshold");
    load_threshold("hv_threshold");
    load_threshold("hw_threshold");
}

void Data::load_runinfo()
{
    TTree *t = (TTree*)rootFile->Get("Trun");
    if (t) {
        t->SetBranchAddress("runNo", &runNo);
        t->SetBranchAddress("subRunNo", &subRunNo);
        t->SetBranchAddress("eventNo", &eventNo);
        t->GetEntry(0);
    }
    else {
        runNo = 0;
        subRunNo = 0;
        eventNo = 0;
    }
}

int Data::GetPlaneNo(int chanNo)
{
    // 800 u + 800 v + 960 w = 2560
    int apaNo = chanNo / 2560;
    int offset = chanNo - apaNo*2560;
    if (offset < 800) {
        return 0;
    }
    else if (offset < 1600) {
        return 1;
    }
    else {
        return 2;
    }
}

// Wrap up some ROOT pointer dancing.
template<class NEED, class WANT>
WANT* maybe_cast(TObject* obj, const std::vector<std::string>& okay_types, bool throw_on_err=false)
{
    NEED* base = dynamic_cast<NEED*>(obj);
    if (!base) {
	return nullptr;
    }
    bool ok = false;
    for (auto type_name : okay_types) {
	if (base->InheritsFrom(type_name.c_str())) {
	    ok = true;
	    break;
	}
    }
    if (ok) {
	return static_cast<WANT*>(base);
    }
    if (throw_on_err) {
	stringstream ss;
	ss << "TObject not one of type: [";
	string comma = "";
	for (auto type_name : okay_types) {
	    ss << comma << type_name;
	    comma = ", ";
	}
        throw runtime_error(ss.str().c_str());
    }
    return nullptr;
}

void Data::load_waveform(const char* name, const char* title, double scale, double threshold)
{
    TObject* obj = rootFile->Get(name);
    if (!obj) {
    	TString msg = "Failed to get waveform ";
    	msg += name;
        msg += ", create dummy ...";
        cout << msg << endl;
    	// throw runtime_error(msg.c_str());
        int nChannels = 2400;
        int nTDCs = 9594;
        int firstChannel = 0;
        if (msg.Contains("hv")) {
            firstChannel = 2400;
        }
        else if (msg.Contains("hw")) {
            firstChannel = 4800;
            nChannels = 3456;
        }
        obj = new TH2F(name, title, nChannels,firstChannel-0.5,firstChannel+nChannels-0.5,nTDCs,0,nTDCs);
    }
    auto hist = maybe_cast<TH2, TH2F>(obj, {"TH2F", "TH2I"}, true);
    hist->SetXTitle("channel");
    hist->SetYTitle("ticks");
    wfs.push_back( new Waveforms(hist, bad_channels, name, title, scale, threshold) );
}

void Data::load_rawwaveform(const char* name, const char* baseline_name)
{
    TObject* obj = rootFile->Get(name);
    if (!obj) {
        TString msg = "Failed to get waveform ";
        msg += name;
        msg += ", create dummy ...";
        cout << msg << endl;
        // throw runtime_error(msg.c_str());
        int nChannels = 2400;
        int nTDCs = 9594;
        int firstChannel = 0;
        if (msg.Contains("hv")) {
            firstChannel = 2400;
        }
        else if (msg.Contains("hw")) {
            firstChannel = 4800;
            nChannels = 3456;
        }
        obj = new TH2I(name, "", nChannels,firstChannel-0.5,firstChannel+nChannels-0.5,nTDCs,0,nTDCs);
    }

    auto hist = maybe_cast<TH2, TH2I>(obj, {"TH2I", "TH2F"}, true);
    hist->SetXTitle("channel");
    hist->SetYTitle("ticks");

    TObject* obj2 = rootFile->Get(baseline_name);
    if (!obj2) {
        // string msg = "Failed to get baseline ";
        // msg += baseline_name;
        // msg += ", create dummy ...";
        // // throw runtime_error(msg.c_str());
        // obj2 = new TH1I(baseline_name, "", hist->GetNbinsX(),0,hist->GetNbinsX());
        raw_wfs.push_back( new RawWaveforms(hist, 0) );
    }
    else {
        TH1I* hist2 = dynamic_cast<TH1I*>(obj2);
        if (!hist2) {
            string msg = "Not a TH1I: ";
            msg += name;
            throw runtime_error(msg.c_str());
        }
        raw_wfs.push_back( new RawWaveforms(hist, hist2) );
    }

}

void Data::load_threshold(const char* name)
{
    TObject* obj = rootFile->Get(name);
    if (!obj) {
        string msg = "Failed to get threshold ";
        msg += name;
        msg += ", create dummy ...";
        // throw runtime_error(msg.c_str());
        obj = new TH1I(name, "", 4000,0,4000);
    }
    auto hist = maybe_cast<TH1, TH1I>(obj, {"TH1I", "TH1F"}, true);
    thresh_histos.push_back( hist );
}

Data::~Data()
{
    delete rootFile;
}
// Local Variables:
// mode: c++
// c-basic-offset: 4
// End:
