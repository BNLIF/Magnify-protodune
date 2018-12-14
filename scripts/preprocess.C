/*
Script to merge histograms from six APAs
Wenqiang Gu (wgu@bnl.gov)
*/

#include <iostream>
#include <cassert>
#include <string>
#include <regex>
#include "TFile.h"
#include "TH2F.h"
#include "TH2I.h"
#include "TCollection.h"
#include "TKey.h"
#include "TClass.h"
#include "TString.h"
#include "TROOT.h"

/*
 * Get File Name from a Path with or without extension
 * e.g., std::string filePath = "/home/user/sample/temp/data.csv";
 * std::string name = getFileName(filePath);
 * assert(name == "data.csv");
 */
std::string getFileName(const std::string& fullPath, const std::string extension=".root")
{
  const size_t lastSlashIndex = fullPath.find_last_of("/\\");
  return fullPath.substr(lastSlashIndex + 1, fullPath.size() - lastSlashIndex - extension.size() - 1);
}

void MergeByTag(TFile* f1, TH2* hall, const char* tag="hu_orig", bool set_baseline=false, double scale=1){
     TIter keyList(f1->GetListOfKeys());
     TKey *key;
     TH1I* hbase = new TH1I("baseline","baseline", 4096, 0, 4096);
     while ((key = (TKey*)keyList())) {
        TClass *cl = gROOT->GetClass(key->GetClassName());
        if (!cl->InheritsFrom("TH1")) continue;
        TH2 *h = (TH2*)key->ReadObj();
        TString hname(h->GetName());
        if(hname.Contains(tag)){
          int nX = h->GetNbinsX();
          int nY = h->GetNbinsY();
          for(int i=1; i<=nX; i++){
            // cout << "channel " << i << endl;
            if(set_baseline) hbase->Reset();
            int X = h->GetXaxis()->GetBinCenter(i);
            for(int j=1; j<=nY; j++){
              int Y = h->GetYaxis()->GetBinCenter(j);
              double content = h->GetBinContent(i,j);
              if(set_baseline) hbase->Fill(content);
              else{
                int bin1 = hall->FindBin(X,Y);
                hall->SetBinContent(bin1, content);
              }
            }
            if(set_baseline){
              float baseline = hbase->GetBinCenter(hbase->GetMaximumBin());
              // cout << "channel " << i << " baseline= " << baseline << endl;
              for(int j=1; j<=nY; j++){
                // cout << "tick= " << j << endl;
                int Y = h->GetYaxis()->GetBinCenter(j);
                double content = h->GetBinContent(i,j);
                int bin1 = hall->FindBin(X,Y);
                hall->SetBinContent(bin1, content - baseline);             
              }

            }
          }
          std::cout << "Mergeing " << h->GetName() << " to " << hall->GetName() << std::endl;

        }
     }
     hbase->Delete();
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
          for(int i=1; i<=nX; i++){
              int X = h->GetXaxis()->GetBinCenter(i);
              double content = h->GetBinContent(i);
              int bin1 = hall->FindBin(X);
              hall->SetBinContent(bin1, h->GetBinContent(i));
          }
          std::cout << "Mergeing " << h->GetName() << " to " << hall->GetName() << std::endl;
        }
     }
}


// merge histograms given in- and out-tags
void preprocess(
      std::string inPath = "data/magnify_5141_23468.root",
      std::string outDir = "data/", 
      const char* intag= "orig",
      const char* outtag = "orig",
      std::string suffix="v2",
      bool set_baseline=false,
      const char* file_open_mode = "update",
      float xmin=-0.5,
      float xmax=15359.5,
      float ymin=0,
      float ymax=6000
) {

  std::string fileName = getFileName(inPath);
  std::string outPath = outDir + "/" + fileName + "-" + suffix + ".root";
  std::cout << "input file: " << inPath << std::endl;
  std::cout << "output file: " << outPath << std::endl;
  std::cout << "in tag: " << intag << std::endl;
  std::cout << "out tag: " << outtag << std::endl;
  int nbinsx = xmax-xmin;
  int nbinsy = ymax-ymin;

  TFile *f1 = TFile::Open(inPath.c_str());
  auto h = f1->Get(Form("hu_%s", intag));
  if(h) return;
  if(std::string(outtag)=="threshold"){
    TH1I* hu = new TH1I(Form("hu_%s", outtag),Form("hu_%s", outtag), nbinsx, xmin, xmax);
    TH1I* hv = new TH1I(Form("hv_%s", outtag),Form("hv_%s", outtag), nbinsx, xmin, xmax);
    TH1I* hw = new TH1I(Form("hw_%s", outtag),Form("hw_%s", outtag), nbinsx, xmin, xmax);

    TFile* fout = new TFile(outPath.c_str(), file_open_mode);
    Merge1DByTag(f1, hu, Form("hu_%s", intag));
    Merge1DByTag(f1, hv, Form("hv_%s", intag));
    Merge1DByTag(f1, hw, Form("hw_%s", intag));
    fout->cd();
    hu->Write();
    hv->Write();
    hw->Write();
    fout->Close();
  }
  else if (std::string(outtag)=="orig"){
    TH2I* hu = new TH2I(Form("hu_%s", outtag),Form("hu_%s", outtag), nbinsx, xmin, xmax, nbinsy, ymin,ymax);
    TH2I* hv = new TH2I(Form("hv_%s", outtag),Form("hv_%s", outtag), nbinsx, xmin, xmax, nbinsy, ymin,ymax);
    TH2I* hw = new TH2I(Form("hw_%s", outtag),Form("hw_%s", outtag), nbinsx, xmin, xmax, nbinsy, ymin,ymax);

    TFile* fout = new TFile(outPath.c_str(), file_open_mode);
    MergeByTag(f1, hu, Form("hu_%s", intag));
    MergeByTag(f1, hv, Form("hv_%s", intag));
    MergeByTag(f1, hw, Form("hw_%s", intag));
    fout->cd();
    hu->Write();
    hv->Write();
    hw->Write();
    fout->Close();
  }
  else{
    TH2F* hu = new TH2F(Form("hu_%s", outtag),Form("hu_%s", outtag), nbinsx, xmin, xmax, nbinsy, ymin,ymax);
    TH2F* hv = new TH2F(Form("hv_%s", outtag),Form("hv_%s", outtag), nbinsx, xmin, xmax, nbinsy, ymin,ymax);
    TH2F* hw = new TH2F(Form("hw_%s", outtag),Form("hw_%s", outtag), nbinsx, xmin, xmax, nbinsy, ymin,ymax);

    TFile* fout = new TFile(outPath.c_str(), file_open_mode);
    MergeByTag(f1, hu, Form("hu_%s", intag), set_baseline);
    MergeByTag(f1, hv, Form("hv_%s", intag), set_baseline);
    MergeByTag(f1, hw, Form("hw_%s", intag), set_baseline);
    fout->cd();
    hu->Write();
    hv->Write();
    hw->Write();
    fout->Close();

   }
   f1->Close();

   std::cout << "\n Now try: ./magnify.sh " + outPath + "\n"; 
}
