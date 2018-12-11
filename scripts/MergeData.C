/*
Script to merge histograms from different APA
Wenqiang Gu (wgu@bnl.gov)
*/
#include <iostream>
#include "TFile.h"
#include "TH2F.h"
#include "TH2I.h"
#include "TCollection.h"
#include "TKey.h"
#include "TClass.h"
#include "TString.h"
#include "TROOT.h"
#include <string>
#include <regex>

void MergeByTag(TFile* f1, TH2* hall, const char* tag="hu_orig", double baseline=0, double scale=1){
     TIter keyList(f1->GetListOfKeys());
     TKey *key;
     while ((key = (TKey*)keyList())) {
        TClass *cl = gROOT->GetClass(key->GetClassName());
        if (!cl->InheritsFrom("TH1")) continue;
        TH2 *h = (TH2*)key->ReadObj();
        TString hname(h->GetName());
        if(hname.Contains(tag)){
          int nX = h->GetNbinsX();
          int nY = h->GetNbinsY();
          for(int i=1; i<=nX; i++)
            for(int j=1; j<=nY; j++){
              int X = h->GetXaxis()->GetBinCenter(i);
              int Y = h->GetYaxis()->GetBinCenter(j);
              double content = h->GetBinContent(i,j);
              //if(content>0){
                int bin1 = hall->FindBin(X,Y);
                hall->SetBinContent(bin1, scale* (h->GetBinContent(i,j) - baseline) );
              //}
            }
          std::cout << "Mergeing " << h->GetName() << " to " << hall->GetName() << std::endl;
        }
     }
}

void Merge1DByTag(TFile* f1, TH1* hall, const char* tag="hu_threshold"){
     TIter keyList(f1->GetListOfKeys());
     TKey *key;
     while ((key = (TKey*)keyList())) {
        TClass *cl = gROOT->GetClass(key->GetClassName());
        if (!cl->InheritsFrom("TH1")) continue;
        TH1 *h = (TH1*)key->ReadObj();
        TString hname(h->GetName());
        if(hname.Contains(tag)){
          int nX = h->GetNbinsX();
          // int nY = h->GetNbinsY();
          for(int i=1; i<=nX; i++){
              int X = h->GetXaxis()->GetBinCenter(i);
              double content = h->GetBinContent(i);
              //if(content>0){
                int bin1 = hall->FindBin(X);
                hall->SetBinContent(bin1, h->GetBinContent(i));
              //}
          }
          std::cout << "Mergeing " << h->GetName() << " to " << hall->GetName() << std::endl;
        }
     }
}

void Get2DRange(double& Xmin, double& Xmax, double& Ymin, double & Ymax, TFile* f1, const char* tag="orig"){
  Xmin=0xFFFF; Ymin=0xFFFF;
  Xmax=0; Ymax=0;

  TIter keyList(f1->GetListOfKeys());
  TKey *key;
  while ((key = (TKey*)keyList())) {
     TString clname(key->GetClassName());
     if(!clname.Contains("TH2")) continue;
     TH2 *h = (TH2*)key->ReadObj();
     TString hname(h->GetName());
     if(hname.Contains(tag)){
       if(h->GetXaxis()->GetXmax() > Xmax) Xmax = h->GetXaxis()->GetXmax();         
       if(h->GetYaxis()->GetXmax() > Ymax) Ymax = h->GetYaxis()->GetXmax();
       if(h->GetXaxis()->GetXmin() < Xmin) Xmin = h->GetXaxis()->GetXmin();
       if(h->GetYaxis()->GetXmin() < Ymin) Ymin = h->GetYaxis()->GetXmin(); 
     }
  }
}

void Get1DRange(double& Xmin, double& Xmax, TFile* f1, const char* tag="threshold"){
  Xmin=0xFFFF;
  Xmax=0;

  TIter keyList(f1->GetListOfKeys());
  TKey *key;
  while ((key = (TKey*)keyList())) {
     TString clname(key->GetClassName());
     if(!clname.Contains("TH1")) continue;
     TH1 *h = (TH1*)key->ReadObj();
     TString hname(h->GetName());
     if(hname.Contains(tag)){
       if(h->GetXaxis()->GetXmax() > Xmax) Xmax = h->GetXaxis()->GetXmax();         
       if(h->GetXaxis()->GetXmin() < Xmin) Xmin = h->GetXaxis()->GetXmin();
     }
  }
}

void MergeData(const char* rootfile,
      const char* inpath = "data/",  
      const char* outpath = "data/",  
      const char* file_open_mode = "update",  
      const char* tag= "orig",
      const char* outtag = "orig",
      const char* suffix="-v2.root") {

  std::string inpathname(inpath);
  std::string outpathname(outpath);
  std::string finname(rootfile);
  std::string foutname(rootfile);
  finname = inpathname + "/"+ finname;
  foutname = outpathname + "/"+ foutname;
  foutname = std::regex_replace(foutname, std::regex(".root"), std::string(suffix));

  std::cout << "input file: " << rootfile << std::endl;
  std::cout << "output file: " << foutname << std::endl;
  std::cout << "input path: " << inpath << std::endl;
  std::cout << "output path: " << outpath << std::endl;
  std::cout << "intag: " << tag << std::endl;
  std::cout << "outtag: " << outtag << std::endl;

  TFile *f1 = TFile::Open(finname.c_str());
  auto h = f1->Get(Form("hu_%s", tag));
  if(h) return;

  if(strcmp(tag, "threshold")==0){
      TH1I* hu_orig = (TH1I*)f1->Get(Form("hu_%s", tag));
      TH1I* hv_orig = (TH1I*)f1->Get(Form("hv_%s", tag));
      TH1I* hw_orig = (TH1I*)f1->Get(Form("hw_%s", tag));

      double xmin, xmax;
      //Get1DRange(xmin, xmax, f1);
      xmin=-0.5; xmax=15359.5; // hard coded as the data missed some channels
      std::cout << "1D range: " << xmin << " " << xmax << std::endl;

     int nbinsx = xmax-xmin;

     //foutname = std::regex_replace(foutname, std::regex(".root"), std::string(suffix));
     TFile* fout = new TFile(foutname.c_str(), file_open_mode);
     hu_orig = new TH1I(Form("hu_%s", outtag),Form("hu_%s", outtag), nbinsx, xmin, xmax);
     hv_orig = new TH1I(Form("hv_%s", outtag),Form("hv_%s", outtag), nbinsx, xmin, xmax);
     hw_orig = new TH1I(Form("hw_%s", outtag),Form("hw_%s", outtag), nbinsx, xmin, xmax);
     Merge1DByTag(f1, hu_orig, Form("hu_%s", tag));
     Merge1DByTag(f1, hv_orig, Form("hv_%s", tag));
     Merge1DByTag(f1, hw_orig, Form("hw_%s", tag));
     fout->cd();
     hu_orig->Write();
     hv_orig->Write();
     hw_orig->Write();
     fout->Close();
  }
  else if (strcmp(tag, "orig")==0){
   TH2I* hu_orig = (TH2I*)f1->Get(Form("hu_%s", tag));
   TH2I* hv_orig = (TH2I*)f1->Get(Form("hv_%s", tag));
   TH2I* hw_orig = (TH2I*)f1->Get(Form("hw_%s", tag));

     double xmin, ymin, xmax, ymax;
     //Get2DRange(xmin, xmax, ymin, ymax, f1);
     xmin=-0.5; xmax=15359.5;
     ymin=0; ymax=6000;
     std::cout << "2D range: " << xmin << " " << xmax << " " << ymin << " " << ymax << std::endl;

     int nbinsx = xmax-xmin;
     int nbinsy = ymax-ymin;

     //foutname = std::regex_replace(foutname, std::regex(".root"), std::string(suffix));
     //TFile* fout = new TFile("protodune-sim-check-v2.root","recreate");
     TFile* fout = new TFile(foutname.c_str(), file_open_mode);
     hu_orig = new TH2I(Form("hu_%s", outtag),Form("hu_%s", outtag), nbinsx, xmin, xmax, nbinsy, ymin,ymax);
     hv_orig = new TH2I(Form("hv_%s", outtag),Form("hv_%s", outtag), nbinsx, xmin, xmax, nbinsy, ymin,ymax);
     hw_orig = new TH2I(Form("hw_%s", outtag),Form("hw_%s", outtag), nbinsx, xmin, xmax, nbinsy, ymin,ymax);
     double u_baseline=0;
     double w_baseline=0;
     // if(strcmp(outtag, "raw") ==0){
     // 	u_baseline = 2349.;
     // 	w_baseline = 901.;
     // }
     MergeByTag(f1, hu_orig, Form("hu_%s", tag), u_baseline);
     MergeByTag(f1, hv_orig, Form("hv_%s", tag), u_baseline);
     MergeByTag(f1, hw_orig, Form("hw_%s", tag), w_baseline);
     fout->cd();
     hu_orig->Write();
     hv_orig->Write();
     hw_orig->Write();
     fout->Close();
  }
  else{
   TH2F* hu_orig = (TH2F*)f1->Get(Form("hu_%s", tag));
   TH2F* hv_orig = (TH2F*)f1->Get(Form("hv_%s", tag));
   TH2F* hw_orig = (TH2F*)f1->Get(Form("hw_%s", tag));

     double xmin, ymin, xmax, ymax;
     //Get2DRange(xmin, xmax, ymin, ymax, f1);
     xmin=-0.5; xmax=15359.5;
     ymin=0; ymax=6000;
     std::cout << "2D range: " << xmin << " " << xmax << " " << ymin << " " << ymax << std::endl;

     int nbinsx = xmax-xmin;
     int nbinsy = ymax-ymin;

     //foutname = std::regex_replace(foutname, std::regex(".root"), std::string(suffix));
     //TFile* fout = new TFile("protodune-sim-check-v2.root","recreate");
     TFile* fout = new TFile(foutname.c_str(), file_open_mode);
     hu_orig = new TH2F(Form("hu_%s", outtag),Form("hu_%s", outtag), nbinsx, xmin, xmax, nbinsy, ymin,ymax);
     hv_orig = new TH2F(Form("hv_%s", outtag),Form("hv_%s", outtag), nbinsx, xmin, xmax, nbinsy, ymin,ymax);
     hw_orig = new TH2F(Form("hw_%s", outtag),Form("hw_%s", outtag), nbinsx, xmin, xmax, nbinsy, ymin,ymax);
     double u_baseline=0;
     double w_baseline=0;
     double u_scale=1.;
     double w_scale=1.;
     // if(strcmp(outtag, "raw") ==0){
     // 	u_baseline = 2349.;
     // 	w_baseline = 901.;
     // }
     //else if(strcmp(outtag, "decon") ==0){
     // u_scale=1.;
     // w_scale=1.;
     //}
     MergeByTag(f1, hu_orig, Form("hu_%s", tag), u_baseline, u_scale);
     MergeByTag(f1, hv_orig, Form("hv_%s", tag), u_baseline, u_scale);
     MergeByTag(f1, hw_orig, Form("hw_%s", tag), w_baseline, w_scale);
     //if(strcmp(tag, "raw") ==0){
     // hu_orig->Rebin2D(1,4);
     // hv_orig->Rebin2D(1,4);
     // hw_orig->Rebin2D(1,4);
     //}     
     fout->cd();
     hu_orig->Write();
     hv_orig->Write();
     hw_orig->Write();
     fout->Close();

     // //fout = new TFile("protodune-sim-check-v3.root","update");
     // fout = new TFile(foutname.c_str(),"update");
     // TH2F* hu_raw = new TH2F("hu_raw","hu_raw", nbinsx, xmin, xmax, nbinsy, ymin,ymax);
     // TH2F* hv_raw = new TH2F("hv_raw","hv_raw", nbinsx, xmin, xmax, nbinsy, ymin,ymax);
     // TH2F* hw_raw = new TH2F("hw_raw","hw_raw", nbinsx, xmin, xmax, nbinsy, ymin,ymax);
     // MergeByTag(f1, hu_raw, "hu_orig", 1842.75);
     // MergeByTag(f1, hv_raw, "hv_orig", 1842.75);
     // MergeByTag(f1, hw_raw, "hw_orig", 409.5);
     // fout->cd();
     // hu_raw->Write();
     // hv_raw->Write();
     // hw_raw->Write();
     // fout->Close();

     // //fout = new TFile("protodune-sim-check-v3.root","update");
     // fout = new TFile(foutname.c_str(),"update");
     // TH2F* hu_decon = new TH2F("hu_decon","hu_decon", nbinsx, xmin, xmax, nbinsy, ymin,ymax);
     // TH2F* hv_decon = new TH2F("hv_decon","hv_decon", nbinsx, xmin, xmax, nbinsy, ymin,ymax);
     // TH2F* hw_decon = new TH2F("hw_decon","hw_decon", nbinsx, xmin, xmax, nbinsy, ymin,ymax);
     // // MergeByTag(f1, hu_decon, "hu_orig", 1842.75, 125);
     // // MergeByTag(f1, hv_decon, "hv_orig", 1842.75, 125);
     // // MergeByTag(f1, hw_decon, "hw_orig", 409.5, 125);
     // MergeByTag(f1, hu_decon, "hu_gauss", 1842.75);
     // MergeByTag(f1, hv_decon, "hv_gauss", 1842.75);
     // MergeByTag(f1, hw_decon, "hw_gauss", 409.5);
     // hu_decon->Rebin2D(1,4);
     // hv_decon->Rebin2D(1,4);
     // hw_decon->Rebin2D(1,4);
     // fout->cd();
     // hu_decon->Write();
     // hv_decon->Write();
     // hw_decon->Write();     
     // fout->Close();

   }
   f1->Close();

}
