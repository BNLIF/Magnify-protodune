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

// main
void display(const char* filename, int ch){
  int wpid  = planeid(ch);
  const char* label = plane_label(wpid);
  int anodeid = ch / 2560;

  const char* orig_tag = Form("h%s_orig%d",label,anodeid);
  const char* raw_tag = Form("h%s_raw%d",label,anodeid);
  const char* wiener_tag = Form("h%s_wiener%d",label,anodeid);
  const char* gauss_tag = Form("h%s_gauss%d",label,anodeid);
  const char* troi_tag = Form("h%s_troi%d",label,anodeid); // tight roi
  const char* lroi_tag = Form("h%s_lroi%d",label,anodeid); // loose roi
  //
  // const char* orig_tag = Form("h%s_orig",label);
  // const char* raw_tag = Form("h%s_raw",label);
  // const char* wiener_tag = Form("h%s_wiener",label);
  // const char* gauss_tag = Form("h%s_gauss",label);
  // const char* troi_tag = Form("h%s_troi",label); // tight roi
  // const char* lroi_tag = Form("h%s_lroi",label); // loose roi
  
  auto file = TFile::Open(filename);
  TH2F* h2_orig = (TH2F*)file->Get(orig_tag);
  TH2F* h2_raw = (TH2F*)file->Get(raw_tag);
  TH2F* h2_wiener = (TH2F*)file->Get(wiener_tag);
  TH2F* h2_gauss = (TH2F*)file->Get(gauss_tag);
  TH2F* h2_troi = (TH2F*)file->Get(troi_tag);
  TH2F* h2_lroi = (TH2F*)file->Get(lroi_tag);

  const int nticks = h2_orig->GetNbinsY();
  TH1F* h1_orig = new TH1F("h_orig","",nticks,0,nticks);
  TH1F* h1_raw = new TH1F("h_raw","",nticks,0,nticks);
  TH1F* h1_wiener = new TH1F("h_wiener","",nticks,0,nticks);
  TH1F* h1_gauss = new TH1F("h_gauss","",nticks,0,nticks);
  TH1F* h1_troi = new TH1F("h_troi","",nticks,0,nticks);
  TH1F* h1_lroi = new TH1F("h_lroi","",nticks,0,nticks);

  //int ch_offset =  anodeid*2560 + (wpid==1)*800 + (wpid==2)*1600;
  //int ch_offset = 0;
  // cout << orig_tag << endl;
  // cout << "ch+1 - ch_offset: " << ch+1 - ch_offset << endl;

  int chbin = h2_gauss->GetXaxis()->FindBin(ch);
  //cout << "ch+1-ch_offset: " << ch+1-ch_offset << " chbin: " << chbin << endl;
  for (int i=0; i<nticks; i++){
    h1_orig->SetBinContent(i+1, h2_orig->GetBinContent(chbin, i+1));
    h1_raw->SetBinContent(i+1, h2_raw->GetBinContent(chbin, i+1));
    h1_wiener->SetBinContent(i+1, h2_wiener->GetBinContent(chbin, i+1));
    h1_gauss->SetBinContent(i+1, h2_gauss->GetBinContent(chbin, i+1));
    h1_troi->SetBinContent(i+1, h2_troi->GetBinContent(chbin, i+1));
    h1_lroi->SetBinContent(i+1, h2_lroi->GetBinContent(chbin, i+1));
  }

  h1_orig->SetLineColor(1);
  h1_raw->SetLineColor(4);
  h1_wiener->SetLineColor(2);
  h1_gauss->SetLineColor(6);

  h1_raw->SetTitle(Form("Channel %d", ch));
  h1_raw->GetXaxis()->SetTitle("nticks");
  // h1_raw->GetYaxis()->SetTitle("a.u.");
  
  h1_wiener->Scale(0.005);
  h1_gauss->Scale(0.005);
  h1_troi->Scale(0.005);
  h1_lroi->Scale(0.005);

  cout << "wiener: " << h1_wiener->Integral() << endl;
  cout << "troi: " << h1_troi->Integral() << endl;
  cout << "lroi: " << h1_lroi->Integral() << endl;

  h1_troi->SetLineStyle(7);
  h1_lroi->SetLineStyle(9);
  h1_troi->SetLineColor(8);
  h1_lroi->SetLineColor(8);

  auto c1 = new TCanvas("c1","",1200,500);
  c1->SetGridy();
  c1->SetGridx();

  //h1_orig->Draw();
  h1_raw->Draw();
  h1_wiener->Draw("same hist");
  h1_gauss->Draw("same hist");
  h1_troi->Draw("same hist");
  h1_lroi->Draw("same hist");

  auto lg = new TLegend(0.65,0.65,0.85,0.85);
  lg->AddEntry(h1_raw, "Denoised", "l")->SetTextColor(4);
  lg->AddEntry(h1_wiener, "Deconvolved (Wiener)", "l")->SetTextColor(2);
  lg->AddEntry(h1_gauss, "Deconvolved (Gauss)", "l")->SetTextColor(6);
  lg->AddEntry(h1_troi, "Tight ROI", "l")->SetTextColor(8);
  lg->AddEntry(h1_lroi, "Loose ROI", "l")->SetTextColor(8);
  lg->Draw();
}
