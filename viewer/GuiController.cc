#include "GuiController.h"
#include "MainWindow.h"
#include "ViewWindow.h"
#include "ControlWindow.h"
#include "Data.h"
#include "Waveforms.h"
#include "RawWaveforms.h"
#include "BadChannels.h"

#include "TApplication.h"
#include "TSystem.h"
#include "TExec.h"
#include "TROOT.h"
#include "TMath.h"
#include "TGFileDialog.h"

#include "TGMenu.h"
#include "TGNumberEntry.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TH2I.h"
#include "TH1I.h"
#include "TBox.h"
#include "TLine.h"
#include "TColor.h"

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;


GuiController::GuiController(const TGWindow *p, int w, int h, const char* fn, const char* frame, int rebin)
{
    mw = new MainWindow(p, w, h);
    vw = mw->fViewWindow;
    cw = mw->fControlWindow;

    // data = new Data("../data/2D_display_3455_0_0.root");
    TString filename;
    if (!fn) {
        filename = OpenDialog();
    }
    else {
        filename = fn;
    }
    data = new Data(filename.Data(), frame, rebin);
    mw->SetWindowName(TString::Format("Magnify: run %i, sub-run %i, event %i",
        data->runNo, data->subRunNo, data->eventNo));

    for (int i=0; i<6; i++) {
        vw->can->cd(i+1);
        data->wfs.at(i)->Draw2D();
    }
    for (int i=0; i<3; i++) {
        vw->can->cd(i+7);
        int chanNo = data->wfs.at(i)->firstChannel;
        data->wfs.at(i)->Draw1D(chanNo);
        TH1F *h = data->wfs.at(i+3)->Draw1D(chanNo, "same"); // draw calib
        h->SetLineColor(kRed);
        hCurrent[i] = h;
    }

    InitConnections();
}

GuiController::~GuiController()
{
    // gApplication->Terminate(0);
}

void GuiController::InitConnections()
{
    mw->fMenuFile->Connect("Activated(int)", "GuiController", this, "HandleMenu(int)");

    for (int i=0; i<3; i++) {
        cw->threshEntry[i]->SetNumber(data->wfs.at(i)->threshold);
    }
    cw->threshEntry[0]->Connect("ValueSet(Long_t)", "GuiController", this, "ThresholdUChanged()");
    cw->threshEntry[1]->Connect("ValueSet(Long_t)", "GuiController", this, "ThresholdVChanged()");
    cw->threshEntry[2]->Connect("ValueSet(Long_t)", "GuiController", this, "ThresholdWChanged()");
    cw->setThreshButton->Connect("Clicked()", "GuiController", this, "SetChannelThreshold()");
    cw->threshScaleEntry->Connect("ValueSet(Long_t)", "GuiController", this, "SetChannelThreshold()");

    cw->zAxisRangeEntry[0]->SetNumber(data->wfs.at(0)->zmin);
    cw->zAxisRangeEntry[1]->SetNumber(data->wfs.at(0)->zmax);
    cw->zAxisRangeEntry[0]->Connect("ValueSet(Long_t)", "GuiController", this, "ZRangeChanged()");
    cw->zAxisRangeEntry[1]->Connect("ValueSet(Long_t)", "GuiController", this, "ZRangeChanged()");

    cw->timeRangeEntry[0]->SetNumber(0);
    cw->timeRangeEntry[1]->SetNumber(data->wfs.at(0)->nTDCs);

    cw->channelEntry->Connect("ValueSet(Long_t)", "GuiController", this, "ChannelChanged()");
    cw->timeEntry->Connect("ValueSet(Long_t)", "GuiController", this, "TimeChanged()");
    cw->badChanelButton->Connect("Clicked()", "GuiController", this, "UpdateShowBadChannel()");
    cw->badChanelButton->SetToolTipText(TString::Format("U: %lu, V: %lu, Y: %lu",
        data->wfs.at(0)->lines.size(),
        data->wfs.at(1)->lines.size(),
        data->wfs.at(2)->lines.size()
    ));
    cw->rawWfButton->Connect("Clicked()", "GuiController", this, "UpdateShowRaw()");
    cw->unZoomButton->Connect("Clicked()", "GuiController", this, "UnZoom()");

    // stupid way to connect signal and slots
    vw->can->GetPad(1)->Connect("RangeChanged()", "GuiController", this, "SyncTimeAxis0()");
    vw->can->GetPad(2)->Connect("RangeChanged()", "GuiController", this, "SyncTimeAxis1()");
    vw->can->GetPad(3)->Connect("RangeChanged()", "GuiController", this, "SyncTimeAxis2()");
    vw->can->GetPad(4)->Connect("RangeChanged()", "GuiController", this, "SyncTimeAxis3()");
    vw->can->GetPad(5)->Connect("RangeChanged()", "GuiController", this, "SyncTimeAxis4()");
    vw->can->GetPad(6)->Connect("RangeChanged()", "GuiController", this, "SyncTimeAxis5()");
    // vw->can->GetPad(7)->Connect("RangeChanged()", "GuiController", this, "WfRangeChanged0()");
    // vw->can->GetPad(8)->Connect("RangeChanged()", "GuiController", this, "WfRangeChanged1()");
    // vw->can->GetPad(9)->Connect("RangeChanged()", "GuiController", this, "WfRangeChanged2()");


    vw->can->Connect(
        "ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
        "GuiController",
        this,
        "ProcessCanvasEvent(Int_t,Int_t,Int_t,TObject*)"
    );
}

void GuiController::UpdateShowBadChannel()
{
    if (cw->badChanelButton->IsDown()) {
        for (int ind=0; ind<6; ind++) {
            vw->can->cd(ind+1);
            data->wfs.at(ind)->DrawLines();
            vw->can->GetPad(ind+1)->Modified();
            vw->can->GetPad(ind+1)->Update();
        }
    }
    else {
        for (int ind=0; ind<6; ind++) {
            vw->can->cd(ind+1);
            data->wfs.at(ind)->HideLines();
            vw->can->GetPad(ind+1)->Modified();
            vw->can->GetPad(ind+1)->Update();
        }
    }

}

void GuiController::ThresholdChanged(int i)
{
    int newThresh = cw->threshEntry[i]->GetNumber();
    cout << "new threshold: " << newThresh << endl;
    for (int ind=i; ind<6; ind+=3) {
        vw->can->cd(ind+1);
        data->wfs.at(ind)->SetThreshold(newThresh);
        data->wfs.at(ind)->Draw2D();
        vw->can->GetPad(ind+1)->Modified();
        vw->can->GetPad(ind+1)->Update();
    }
}

void GuiController::SetChannelThreshold()
{
    // cout << "new threshold: " << newThresh << endl;

    TH1I *ht = 0;
    for (int ind=3; ind<6; ind++) {
        vw->can->cd(ind+1);
        ht = data->thresh_histos.at(ind-3);
        data->wfs.at(ind)->SetThreshold(ht, cw->threshScaleEntry->GetNumber());
        data->wfs.at(ind)->Draw2D();
        vw->can->GetPad(ind+1)->Modified();
        vw->can->GetPad(ind+1)->Update();
    }
}

void GuiController::UnZoom()
{
    cw->timeRangeEntry[0]->SetNumber(0);
    cw->timeRangeEntry[1]->SetNumber(data->wfs.at(0)->nTDCs);
    cw->adcRangeEntry[0]->SetNumber(0);
    cw->adcRangeEntry[1]->SetNumber(0);

    for (int ind=0; ind<6; ind++) {
        data->wfs.at(ind)->hDummy->GetXaxis()->UnZoom();
        data->wfs.at(ind)->hDummy->GetYaxis()->UnZoom();
        vw->can->GetPad(ind+1)->Modified();
        vw->can->GetPad(ind+1)->Update();
    }
}

void GuiController::ZRangeChanged()
{
    int min = cw->zAxisRangeEntry[0]->GetNumber();
    int max = cw->zAxisRangeEntry[1]->GetNumber();
    for (int ind=0; ind<6; ind++) {
        vw->can->cd(ind+1);
        data->wfs.at(ind)->SetZRange(min, max);
        data->wfs.at(ind)->Draw2D();
        vw->can->GetPad(ind+1)->Modified();
        vw->can->GetPad(ind+1)->Update();
    }

}

void GuiController::SyncTimeAxis(int i)
{
    double min = data->wfs.at(i)->hDummy->GetYaxis()->GetFirst();
    double max = data->wfs.at(i)->hDummy->GetYaxis()->GetLast();
    // double min = vw->can->GetPad(i+1)->GetUymin();
    // double max = vw->can->GetPad(i+1)->GetUymax();

    for (int ind=0; ind<6; ind++) {
        if (i==ind) continue;
        data->wfs.at(ind)->hDummy->GetYaxis()->SetRange(min, max);
        vw->can->GetPad(ind+1)->Modified();
        vw->can->GetPad(ind+1)->Update();
    }

    // cout << "range changed: " << min << ", " << max << endl;
}

void GuiController::WfRangeChanged(int i)
{
    // can't figureout how to get the axis range in user coordinate ...
    // ( not pad->GetUxmin() etc, nor axis->GetFirst() etc. )
}

void GuiController::UpdateShowRaw()
{
    int channel = cw->channelEntry->GetNumber();
    cout << "channel: " << channel << endl;
    int wfsNo = 0;
    if (channel>=data->wfs.at(1)->firstChannel && channel<data->wfs.at(2)->firstChannel) wfsNo = 1;
    else if (channel>=data->wfs.at(2)->firstChannel) wfsNo = 2;

    int padNo = wfsNo+7;
    vw->can->cd(padNo);

    TH1I *hh = data->raw_wfs.at(wfsNo)->Draw1D(channel, "same");
    if (cw->rawWfButton->IsDown()) {
        hh->SetLineColor(kBlue);
        // hMain->SetTitle( TString::Format("%s, %s", hMain->GetTitle(), hh->GetTitle()) );
    }
    else {
        gPad->GetListOfPrimitives()->Remove(hh);
        // hMain->SetTitle( TString::Format("%s, %s", hMain->GetTitle(), hh->GetTitle()) );
    }

    vw->can->GetPad(padNo)->Modified();
    vw->can->GetPad(padNo)->Update();
}

void GuiController::ChannelChanged()
{
    if (cw->timeModeButton->IsDown()) {
        return; // skip if time mode is selected
    }
    int channel = cw->channelEntry->GetNumber();
    cout << "channel: " << channel << endl;
    int wfsNo = 0;
    if (channel>=data->wfs.at(1)->firstChannel && channel<data->wfs.at(2)->firstChannel) wfsNo = 1;
    else if (channel>=data->wfs.at(2)->firstChannel) wfsNo = 2;

    int padNo = wfsNo+7;
    vw->can->cd(padNo);

    TH1F *hwf = data->wfs.at(wfsNo)->Draw1D(channel);
    hCurrent[wfsNo] = hwf;
    hwf->SetLineColor(kBlack);

    TString name = TString::Format("hWire_%s_2d_dummy", data->wfs.at(wfsNo)->fName.Data());
    TH2F *hMain = (TH2F*)gDirectory->FindObject(name);
    if (!hMain) {
        cout << "Error: cannot find " << name << endl;
        return;
    }

    hMain->GetXaxis()->SetRangeUser(cw->timeRangeEntry[0]->GetNumber(), cw->timeRangeEntry[1]->GetNumber());
    int adc_min = cw->adcRangeEntry[0]->GetNumber();
    int adc_max = cw->adcRangeEntry[1]->GetNumber();
    if (adc_max > adc_min) {
        hMain->GetYaxis()->SetRangeUser(adc_min, adc_max);
    }
    if (binary_search(data->bad_channels->bad_id.begin(), data->bad_channels->bad_id.end(), channel)) {
        hMain->SetTitle( TString::Format("%s (bad channel)", hMain->GetTitle()) );
    }

    TH1F *h = data->wfs.at(wfsNo+3)->Draw1D(channel, "same" ); // draw calib
    h->SetLineColor(kRed);

    TH1I *ht = data->thresh_histos.at(wfsNo);
    int thresh = ht->GetBinContent(ht->GetXaxis()->FindBin(channel));
    cout << "thresh: " << thresh << endl;
    TLine *l = new TLine(0, thresh/500., data->wfs.at(wfsNo)->nTDCs, thresh/500.);
    l->SetLineColor(kMagenta);
    l->SetLineWidth(2);
    l->Draw();

    if (cw->rawWfButton->IsDown()) {
        TH1I *hh = data->raw_wfs.at(wfsNo)->Draw1D(channel, "same"); // draw calib
        hh->SetLineColor(kBlue);
        hMain->SetTitle( TString::Format("%s, %s", hMain->GetTitle(), hh->GetTitle()) );
    }

    vw->can->GetPad(padNo)->SetGridx();
    vw->can->GetPad(padNo)->SetGridy();
    vw->can->GetPad(padNo)->Modified();
    vw->can->GetPad(padNo)->Update();
}

void GuiController::TimeChanged()
{
    if (cw->timeModeButton->IsDown()) {

        int tickNo = cw->timeEntry->GetNumber();
        TH1F *hTick  = 0;
        for (int k=3; k<=5; k++) { // only draw decon signal
            int padNo = k+4;
            vw->can->cd(padNo);
            hTick = data->wfs.at(k)->Draw1DTick(tickNo); // draw time
            hTick->SetLineColor(kRed);

            TString name = TString::Format("hth_%i", k-3);
            TH1I *hth = (TH1I*)gDirectory->FindObject(name);
            if (hth) delete hth;

            hth = (TH1I*)data->thresh_histos.at(k-3)->Clone(name.Data());
            hth->Scale(data->wfs.at(k)->fScale);
            hth->Draw("same");
            hth->SetLineColor(kBlack);

            int channel_min = cw->timeRangeEntry[0]->GetNumber();
            int channel_max = cw->timeRangeEntry[1]->GetNumber();

            if (channel_min>0) {
                hTick->GetXaxis()->SetRangeUser(channel_min, channel_max);
            }

            int adc_min = cw->adcRangeEntry[0]->GetNumber();
            int adc_max = cw->adcRangeEntry[1]->GetNumber();
            if (adc_max > adc_min) {
                hTick->GetYaxis()->SetRangeUser(adc_min, adc_max);
            }

            vw->can->GetPad(padNo)->SetGridx();
            vw->can->GetPad(padNo)->SetGridy();
            vw->can->GetPad(padNo)->Modified();
            vw->can->GetPad(padNo)->Update();
        }
    }
}

void GuiController::ProcessCanvasEvent(Int_t ev, Int_t x, Int_t y, TObject *selected)
{
    if (ev == 11) { // clicked
        if (!(selected->IsA() == TH2F::Class()
            || selected->IsA() == TBox::Class()
            || selected->IsA() == TLine::Class()
        )) return;
        TVirtualPad* pad = vw->can->GetClickSelectedPad();
        int padNo = pad->GetNumber();
        double xx = pad->AbsPixeltoX(x);
        double yy = pad->AbsPixeltoY(y);
        cout << "pad " << padNo << ": (" << xx << ", " << yy << ")" << endl;

        int drawPad = (padNo-1) % 3 + 7;
        vw->can->cd(drawPad);
        if (padNo<=6) {
            int wfNo = padNo - 1;
            wfNo = wfNo < 3 ? wfNo : wfNo-3;  // draw raw first
            int chanNo = TMath::Nint(xx); // round
            int tickNo = TMath::Nint(yy); // round
            // data->wfs.at(wfNo)->Draw1D(chanNo);
            // TH1F *h = data->wfs.at(wfNo+3)->Draw1D(chanNo, "same"); // draw calib
            // h->SetLineColor(kRed);
            // TH1I *hh = data->raw_wfs.at(wfNo)->Draw1D(chanNo, "same"); // draw calib
            // hh->SetLineColor(kBlue);
            cw->channelEntry->SetNumber(chanNo);
            cw->timeEntry->SetNumber(tickNo);

            ChannelChanged();
            TimeChanged();

            // cw->timeRangeEntry[0]->SetNumber(0);
            // cw->timeRangeEntry[1]->SetNumber(data->wfs.at(0)->nTDCs);
        }
        // vw->can->GetPad(drawPad)->Modified();
        // vw->can->GetPad(drawPad)->Update();
    }

}

void GuiController::HandleMenu(int id)
{
    // const char *filetypes[] = {"ROOT files", "*.root", 0, 0};
    switch (id) {
        case M_FILE_EXIT:
            gApplication->Terminate(0);
            break;
    }
}

TString GuiController::OpenDialog()
{
    const char *filetypes[] = {"ROOT files", "*.root", 0, 0};
    TString currentDir(gSystem->WorkingDirectory());
    static TString dir(".");
    TGFileInfo fi;
    fi.fFileTypes = filetypes;
    fi.fIniDir    = StrDup(dir);
    new TGFileDialog(gClient->GetRoot(), mw, kFDOpen, &fi);
    dir = fi.fIniDir;
    gSystem->cd(currentDir.Data());

    if (fi.fFilename) {
        // UnZoom();
        cout << "open file: " << fi.fFilename << endl;
        return fi.fFilename;
    }
    else {
        gApplication->Terminate(0);
    }
    return "";

}
