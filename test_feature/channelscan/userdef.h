#include <iostream>
#include <string>
#include "TH1F.h"
#include "TCanvas.h"

void execute(TH1F* hin_orig, TH1F* hin_nf, int ch=0){
  std::cout << "execute user function for channel: " << ch << std::endl;

  TH1* hm = hin_orig->FFT(0,"MAG_orig");
  hm->SetBinContent(1,0);
  int nsample = hm->GetNbinsX() / 2;
  TH1F hm2("hm2","",nsample, 0, 1.);
  hm2.GetXaxis()->SetTitle("Frequency [MHz]");
  for(int i=0; i<nsample; i++){
    hm2.SetBinContent(i+1, hm->GetBinContent(i+1));
  }

  TH1* hm_nf = hin_nf->FFT(0,"MAG_nf");
  hm_nf->SetBinContent(1,0);
  TH1F hm2_nf("hm2_nf","",nsample, 0, 1.);
  hm2_nf.GetXaxis()->SetTitle("Frequency [MHz]");
  for(int i=0; i<nsample; i++){
    hm2_nf.SetBinContent(i+1, hm_nf->GetBinContent(i+1));
  }

  TCanvas c1("c1","",1000,800);
  c1.Divide(1,2);

  c1.cd(1);
  hin_orig->SetLineColor(1);
  hin_nf->SetLineColor(4);
  hin_orig->Draw();
  hin_nf->Draw("same");

  c1.cd(2);
  hm2.SetLineColor(1);
  hm2_nf.SetLineColor(4);
  hm2.Draw();
  hm2_nf.Draw("same");
  c1.Print(Form("channel%d.png", ch));
 
  delete hm;
  delete hm_nf;
}
