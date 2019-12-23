/*
 * Modified from $ROOTSYS/tutorials/gui/buttonTest.C
 * 
 * August 18, Wenqiang Gu (wgu@bnl.gov)
 */

#include <TGButton.h>
#include <TGButtonGroup.h>
#include <TGLabel.h>
#include <TGNumberEntry.h>
#include <TG3DLine.h>
#include <TApplication.h>

#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TLegend.h>


// //////////// auxilary class ///////////////////////////////////////////////////
// class TextMargin : public TGHorizontalFrame {

// protected:
//    TGNumberEntry *fEntry;

// public:
//    TextMargin(const TGWindow *p, const char *name) : TGHorizontalFrame(p)
//    {
//       fEntry = new TGNumberEntry(this, 0, 6, -1, TGNumberFormat::kNESInteger);
//       AddFrame(fEntry, new TGLayoutHints(kLHintsLeft));
//       TGLabel *label = new TGLabel(this, name);
//       AddFrame(label, new TGLayoutHints(kLHintsLeft, 10));
//    }
//    TGTextEntry *GetEntry() const { return fEntry->GetNumberEntry(); }

//    ClassDef(TextMargin, 0)
// };

////////////////////////////////////////////////////////////////////////////////
class ButtonWindow : public TGMainFrame {

protected:
   // TGTextButton *fButton;   // button being tested
   TRootEmbeddedCanvas* fEcanvas;
   TGCheckButton* fButtonRaw;
   TGCheckButton* fButtonWiener;
   TGCheckButton* fButtonGauss;
   TGCheckButton* fButtonTightROI;
   TGCheckButton* fButtonLooseROI;
   TGCheckButton* fButtonCleanUpROI;
   TGCheckButton* fButtonBreakROI1;
   TGCheckButton* fButtonBreakROI2;
   TGCheckButton* fButtonShrinkROI;
   TGCheckButton* fButtonExtendROI;

   TFile* m_file;
   int m_current_channel;
   TH1F* m_orig;
   TH1F* m_raw;
   TH1F* m_wiener;
   TH1F* m_gauss;
   TH1F* m_troi;
   TH1F* m_lroi;
   TH1F* m_croi;
   TH1F* m_broi1;
   TH1F* m_broi2;
   TH1F* m_sroi;
   TH1F* m_eroi;

public:
   ButtonWindow(const char*, int ch);

   void init();
   void on_change();

   ClassDef(ButtonWindow, 0)
};


//______________________________________________________________________________
ButtonWindow::ButtonWindow(const char* filename, int ch)
: TGMainFrame(gClient->GetRoot(), 10, 10, kHorizontalFrame)
, m_current_channel(ch)
{
   m_file = TFile::Open(filename);

   // Main test window.

   SetCleanup(kDeepCleanup);

   // Controls on right
   TGVerticalFrame *controls = new TGVerticalFrame(this);
   AddFrame(controls, new TGLayoutHints(kLHintsRight | kLHintsExpandY,
                                        5, 5, 5, 5));

   // Separator
   TGVertical3DLine *separator = new TGVertical3DLine(this);
   AddFrame(separator, new TGLayoutHints(kLHintsRight | kLHintsExpandY));

   // Contents
   TGHorizontalFrame *contents = new TGHorizontalFrame(this);
   AddFrame(contents, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,5,5));


   fEcanvas = new TRootEmbeddedCanvas("Ecanvas", this, 1000, 500);
   AddFrame(fEcanvas, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,
                      20, 20, 20, 20));

   TGGroupFrame *group = new TGGroupFrame(controls, "Enable/Disable");
   group->SetTitlePos(TGGroupFrame::kCenter);


   fButtonRaw = new TGCheckButton(group, "Raw Waveform");
   fButtonRaw->SetOn();
   fButtonTightROI = new TGCheckButton(group, "Decon (tight LF)");
   // fButtonTightROI->SetOn();
   fButtonLooseROI = new TGCheckButton(group, "Decon (loose LF)");
   fButtonLooseROI->SetOn();
   fButtonCleanUpROI = new TGCheckButton(group, "CleanUp ROI");
   fButtonCleanUpROI->SetOn();
   fButtonBreakROI1 = new TGCheckButton(group, "1st Break ROI");
   // fButtonBreakROI1->SetOn();
   fButtonBreakROI2 = new TGCheckButton(group, "2nd Break ROI");
   // fButtonBreakROI2->SetOn();
   fButtonShrinkROI = new TGCheckButton(group, "Shrink ROI");
   // fButtonShrinkROI->SetOn();
   fButtonExtendROI = new TGCheckButton(group, "Extend ROI");
   // fButtonExtendROI->SetOn();
   fButtonWiener = new TGCheckButton(group, "Deconvolved (Wiener)");
   // fButtonWiener->SetOn();
   fButtonGauss = new TGCheckButton(group, "Deconvolved (Gauss)");
   // fButtonGauss->SetOn();

   fButtonRaw->Connect("Toggled(Bool_t)", "ButtonWindow", this, "on_change()");
   fButtonTightROI->Connect("Toggled(Bool_t)", "ButtonWindow", this, "on_change()");
   fButtonLooseROI->Connect("Toggled(Bool_t)", "ButtonWindow", this, "on_change()");
   fButtonCleanUpROI->Connect("Toggled(Bool_t)", "ButtonWindow", this, "on_change()");
   fButtonBreakROI1->Connect("Toggled(Bool_t)", "ButtonWindow", this, "on_change()");
   fButtonBreakROI2->Connect("Toggled(Bool_t)", "ButtonWindow", this, "on_change()");
   fButtonShrinkROI->Connect("Toggled(Bool_t)", "ButtonWindow", this, "on_change()");
   fButtonExtendROI->Connect("Toggled(Bool_t)", "ButtonWindow", this, "on_change()");
   fButtonWiener->Connect("Toggled(Bool_t)", "ButtonWindow", this, "on_change()");
   fButtonGauss->Connect("Toggled(Bool_t)", "ButtonWindow", this, "on_change()");

   group->AddFrame(fButtonRaw, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   group->AddFrame(fButtonTightROI, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   group->AddFrame(fButtonLooseROI, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   group->AddFrame(fButtonCleanUpROI, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   group->AddFrame(fButtonBreakROI1, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   group->AddFrame(fButtonBreakROI2, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   group->AddFrame(fButtonShrinkROI, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   group->AddFrame(fButtonExtendROI, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   group->AddFrame(fButtonWiener, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   group->AddFrame(fButtonGauss, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   controls->AddFrame(group, new TGLayoutHints(kLHintsExpandX));


   TGTextButton *quit = new TGTextButton(controls, "Quit");
   controls->AddFrame(quit, new TGLayoutHints(kLHintsBottom | kLHintsExpandX,
                                              0, 0, 0, 5));
   quit->Connect("Pressed()", "TApplication", gApplication, "Terminate()");

   Connect("CloseWindow()", "TApplication", gApplication, "Terminate()");
   DontCallClose();

   MapSubwindows();
   Resize();

   SetWMSizeHints(GetDefaultWidth(), GetDefaultHeight(), 1000, 1000, 0 ,0);
   SetWindowName("O-Scope");
   MapRaised();

   init();
   on_change();
}

//______________________________________________________________________________
int planeid(int ch){
  int ch1 = ch % 2560;
  if (ch1<800) return 0;
  else if (ch1<1600) return 1;
  else return 2;
}

const char* plane_label(int id){
  if (id==0) return "u";
  else if (id==1) return "v";
  else return "w";
}

void fill_channel(TH2F* hin, TH1F* hout, int ch)
{
   if (!hin) return;
   // double offset = 0;
   // std:string hname(hout->GetName());
   // if (hname=="h_troi") offset = -20 / 0.005;
   // cout << hname << " " << offset << endl;

   int nticks = hout->GetNbinsX();
   int chbin = hin->GetXaxis()->FindBin(ch);
   for (int i=0; i<nticks; i++){
      int tbin = hin->GetYaxis()->FindBin(i);
      hout->SetBinContent(i+1, hin->GetBinContent(chbin, tbin));
   }
}

//______________________________________________________________________________
void ButtonWindow::init()
{
   // int ch = 867;
   int ch = m_current_channel;
   int wpid  = planeid(ch);
   const char* label = plane_label(wpid);
   int anodeid = ch / 2560;
   const char* orig_tag = Form("h%s_orig%d",label,anodeid);
   const char* raw_tag = Form("h%s_raw%d",label,anodeid);
   const char* wiener_tag = Form("h%s_wiener%d",label,anodeid);
   const char* gauss_tag = Form("h%s_gauss%d",label,anodeid);
   const char* troi_tag = Form("h%s_tight_lf%d",label,anodeid); // tight roi
   const char* lroi_tag = Form("h%s_loose_lf%d",label,anodeid); // loose roi
   const char* croi_tag = Form("h%s_cleanup_roi%d",label,anodeid);
   const char* broi1_tag = Form("h%s_break_roi_1st%d",label,anodeid);
   const char* broi2_tag = Form("h%s_break_roi_2nd%d",label,anodeid);
   const char* sroi_tag = Form("h%s_shrink_roi%d",label,anodeid);
   const char* eroi_tag = Form("h%s_extend_roi%d",label,anodeid);

   // auto file = TFile::Open("/home/wgu/Magnify-protodune/data/gallery/protodune-data-check.root");
   TH2F* h2_orig = (TH2F*)m_file->Get(orig_tag);
   TH2F* h2_raw = (TH2F*)m_file->Get(raw_tag);
   TH2F* h2_wiener = (TH2F*)m_file->Get(wiener_tag);
   TH2F* h2_gauss = (TH2F*)m_file->Get(gauss_tag);
   TH2F* h2_troi = (TH2F*)m_file->Get(troi_tag);
   TH2F* h2_lroi = (TH2F*)m_file->Get(lroi_tag);
   TH2F* h2_croi = (TH2F*)m_file->Get(croi_tag);
   TH2F* h2_broi1 = (TH2F*)m_file->Get(broi1_tag);
   TH2F* h2_broi2 = (TH2F*)m_file->Get(broi2_tag);
   TH2F* h2_sroi = (TH2F*)m_file->Get(sroi_tag);
   TH2F* h2_eroi = (TH2F*)m_file->Get(eroi_tag);

   const int nticks = h2_orig->GetNbinsY();
   m_orig = new TH1F("h_orig","",nticks,0,nticks);
   m_raw = new TH1F("h_raw","",nticks,0,nticks);
   m_wiener = new TH1F("h_wiener","",nticks,0,nticks);
   m_gauss = new TH1F("h_gauss","",nticks,0,nticks);
   m_troi = new TH1F("h_troi","",nticks,0,nticks);
   m_lroi = new TH1F("h_lroi","",nticks,0,nticks);
   m_croi = new TH1F("h_croi","",nticks,0,nticks);
   m_broi1 = new TH1F("h_broi1","",nticks,0,nticks);
   m_broi2 = new TH1F("h_broi2","",nticks,0,nticks);
   m_sroi = new TH1F("h_sroi","",nticks,0,nticks);
   m_eroi = new TH1F("h_eroi","",nticks,0,nticks);

   fill_channel(h2_orig, m_orig, ch);
   fill_channel(h2_raw, m_raw, ch);
   fill_channel(h2_wiener, m_wiener, ch);
   fill_channel(h2_gauss, m_gauss, ch);
   fill_channel(h2_troi, m_troi, ch);
   fill_channel(h2_lroi, m_lroi, ch);
   fill_channel(h2_croi, m_croi, ch);
   fill_channel(h2_broi1, m_broi1, ch);
   fill_channel(h2_broi2, m_broi2, ch);
   fill_channel(h2_sroi, m_sroi, ch);
   fill_channel(h2_eroi, m_eroi, ch);

   m_orig->SetLineColor(1);
   m_raw->SetLineColor(4);
   m_wiener->SetLineColor(2);
   m_gauss->SetLineColor(2);
   m_troi->SetLineColor(8);
   m_lroi->SetLineColor(8);

   m_croi->SetLineColor(6);
   m_broi1->SetLineColor(6);
   m_broi2->SetLineColor(6);
   m_sroi->SetLineColor(6);
   m_eroi->SetLineColor(6);

   m_gauss->SetLineStyle(7);
   m_troi->SetLineStyle(7);
   m_lroi->SetLineStyle(9);
   m_broi1->SetLineStyle(1);
   m_broi2->SetLineStyle(7);
   m_sroi->SetLineStyle(9);

   m_raw->GetXaxis()->SetTitle("nticks / 0.5us");
   m_raw->SetTitle(Form("Channel %d", ch));

   m_wiener->Scale(0.005);
   m_gauss->Scale(0.005);
   m_troi->Scale(0.005);
   m_lroi->Scale(0.005);
   // m_croi->Scale(0.005);
   // m_broi1->Scale(0.005);
   // m_broi2->Scale(0.005);
   // m_sroi->Scale(0.005);
   // m_eroi->Scale(0.005);

   // int icnt = 0;
   // for(int i=3851; i<=3950; i++) {
   //    if (icnt%5==0) std::cout << std::endl;
   //    std::cout << m_raw->GetBinContent(i) << ", ";
   //    icnt ++;
   // }
   // std::cout << std::endl << "icnt: " << icnt << std::endl;

   m_raw->Draw();
   m_wiener->Draw("same hist");
   m_gauss->Draw("same hist");
   m_troi->Draw("same hist");
   m_lroi->Draw("same hist");
   m_croi->Draw("same hist");
   m_broi1->Draw("same hist");
   m_broi2->Draw("same hist");
   m_sroi->Draw("same hist");
   m_eroi->Draw("same hist");

   auto lg = new TLegend(0.75,0.60,1.0,1.0);
   lg->AddEntry(m_raw, "Denoised Raw", "l")->SetTextColor(4);

   lg->AddEntry(m_troi, "Decon (tight LF)", "l")->SetTextColor(8);
   lg->AddEntry(m_lroi, "Decon (loose LF)", "l")->SetTextColor(8);

   lg->AddEntry(m_croi, "CleanUp ROI", "l")->SetTextColor(6);
   lg->AddEntry(m_broi1, "1st Break ROI", "l")->SetTextColor(6);
   lg->AddEntry(m_broi2, "2nd Break ROI", "l")->SetTextColor(6);
   lg->AddEntry(m_sroi, "Shrink ROI", "l")->SetTextColor(6);
   lg->AddEntry(m_eroi, "Extend ROI", "l")->SetTextColor(6);

   lg->AddEntry(m_wiener, "Deconvolved (Wiener)", "l")->SetTextColor(2);
   lg->AddEntry(m_gauss, "Deconvolved (Gauss)", "l")->SetTextColor(2);
   lg->Draw();

   gPad->SetGridx();
   gPad->SetGridy();

   // gStyle->SetTitleAlign(33);
   gStyle->SetTitleX(.4);

}

//______________________________________________________________________________
void ButtonWindow::on_change()
{
   // std::cout << "handling raw waveform..." << std::endl;
   if( fButtonRaw->IsOn() ) {
      m_raw->SetLineWidth(2);
      // m_raw->Draw("same hist");
   }
   else m_raw->SetLineWidth(0);

   if( fButtonWiener->IsOn() ) {
      m_wiener->SetLineWidth(2);
      // m_wiener->Draw("same hist");
   }
   else m_wiener->SetLineWidth(0);

   if( fButtonGauss->IsOn() ) {
      m_gauss->SetLineWidth(2);
      // m_gauss->Draw("same hist");
   }
   else m_gauss->SetLineWidth(0);

   if( fButtonTightROI->IsOn() ) {
      m_troi->SetLineWidth(2);
      // m_troi->Draw("same hist");
   }
   else m_troi->SetLineWidth(0);

   if( fButtonLooseROI->IsOn() ) {
      m_lroi->SetLineWidth(2);
      // m_lroi->Draw("same hist");
   }
   else m_lroi->SetLineWidth(0);

   if( fButtonCleanUpROI->IsOn() ) {
      m_croi->SetLineWidth(2);
      // m_croi->Draw("same hist");
   }
   else m_croi->SetLineWidth(0);

   if( fButtonBreakROI1->IsOn() ) {
      m_broi1->SetLineWidth(2);
      // m_broi1->Draw("same hist");
   }
   else m_broi1->SetLineWidth(0);

   if( fButtonBreakROI2->IsOn() ) {
      m_broi2->SetLineWidth(2);
      // m_broi2->Draw("same hist");
   }
   else m_broi2->SetLineWidth(0);

   if( fButtonShrinkROI->IsOn() ) {
      m_sroi->SetLineWidth(2);
      // m_sroi->Draw("same hist");
   }
   else m_sroi->SetLineWidth(0);

   if( fButtonExtendROI->IsOn() ) {
      m_eroi->SetLineWidth(2);
      m_eroi->Draw("same hist");
   }
   else m_eroi->SetLineWidth(0);

   gPad->Modified();
   gPad->Update();

}


////////////////////////////////////////////////////////////////////////////////
void buttonTest(const char* filename, int ch)
{
   // Main program.

   new ButtonWindow(filename, ch);
}
