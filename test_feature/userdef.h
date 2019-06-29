#include <iostream>
#include <string>
#include "TH1F.h"
#include "TCanvas.h"

void execute(TH1F* hin, TH1F* hout, int ch=0, std::string tag=""){
  std::cout << "execute user function for channel: " << ch << std::endl;

  TH1* hm = hin->FFT(0,"MAG");
  hm->SetBinContent(1,0);
  int nsample = hm->GetNbinsX() / 2;
  TH1F hm2("hm2","",nsample, 0, 1.);
  hm2.GetXaxis()->SetTitle("Frequency [MHz]");
  for(int i=0; i<nsample; i++){
    hm2.SetBinContent(i+1, hm->GetBinContent(i+1));
  }

  TCanvas c1("c1","",1000,800);
  c1.Divide(1,2);
  c1.cd(1);
  hin->Draw();
  c1.cd(2);
  hm2.Draw();
  c1.Print(Form("channel%d_%s.png", ch, tag.c_str()));
 
  delete hm;
}
