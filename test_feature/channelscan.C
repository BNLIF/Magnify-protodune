#include <iostream>
#include <fstream>

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLeaf.h"

#include "userdef.h"

void channelscan(
       std::string inpath,
       std::string inchan
       // std::string userload
){
  auto file = TFile::Open(inpath.c_str());

  // fill the channel list with pre-defined tree, or a text file
  std::vector<int> chlist; 
  if (inchan.size()>0){
    if (inchan.find("tree:")==0){
      std::string tree_name = inchan.substr(5, inchan.size());
      TTree* tree = (TTree*)file->Get(tree_name.c_str());
      if(tree){
        for(int ientry=0; ientry<tree->GetEntries(); ientry++){
          tree->GetEntry(ientry);
          int chid = tree->GetLeaf("chid")->GetValue();
          chlist.push_back(chid);
        }
      }
      else {
        std::cout << "[ROOT] " << tree_name << " does not exist!" << std::endl;
      }
    }
    else{
      std::ifstream in(inchan.c_str());
      if (in.is_open()){
        int chno;
        while(in >> chno){
          chlist.push_back(chno);
        }
      }
      else{
        std::cout << "[std] cannot open " << inchan << std::endl;
      }
    }
  }

  std::cout << "Channel list: " << std::endl;
  for(auto ich: chlist){
    std::cout << ich << ", ";
  }
  std::cout << std::endl;

  

  // load 2D historgrams
  TH2F* hu = (TH2F*)file->Get("hu_orig");
  TH2F* hv = (TH2F*)file->Get("hv_orig");
  TH2F* hw = (TH2F*)file->Get("hw_orig");
  hu->Add(hv);
  hu->Add(hw);
  
  TH2F* hu2 = (TH2F*)file->Get("hu_raw");
  TH2F* hv2 = (TH2F*)file->Get("hv_raw");
  TH2F* hw2 = (TH2F*)file->Get("hw_raw");
  hu2->Add(hv2);
  hu2->Add(hw2);

  // loop over channels
  const int nticks = hu->GetNbinsY();
  TH1F* hin_orig = new TH1F("hin_orig","",nticks, 0,nticks);
  TH1F* hout_orig = new TH1F("hout_orig","",nticks, 0,nticks);
  TH1F* hin_nf = new TH1F("hin_nf","",nticks, 0,nticks);
  TH1F* hout_nf = new TH1F("hout_nf","",nticks, 0,nticks);
  hin_orig->GetXaxis()->SetTitle("Ticks [0.5#mus]");
  hin_nf->GetXaxis()->SetTitle("Ticks [0.5#mus]");

  for(auto ich: chlist) {
    for (int itk=0; itk<nticks; itk++){
      hin_orig->SetBinContent(itk+1, hu->GetBinContent(ich+1, itk+1) );
      hin_nf->SetBinContent(itk+1, hu2->GetBinContent(ich+1, itk+1) );
    }

    //////////////////////////////
    //
    // Applly user defined function here!
    //
    /////////////////////////////

    execute(hin_orig, hin_nf, ich);

  }
}
                 
