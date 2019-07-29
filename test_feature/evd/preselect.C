void dothis(const char* in_file, //input file name
	const char* out_file, //output file name
	const char* in_hist, // input histogram
	const char* out_hist, // output histogram
	int x_start_bin=1, 
	int x_end_bin=2560,
	int y_start_bin=1,
	int y_end_bin=6000,
	int x_rebin=1,
	int y_rebin=1,
        bool set_baseline=false,
	const char* open_file_mode="update"){

	TFile* file = TFile::Open(in_file);
	std::string hname1 = "hu_"; hname1 += in_hist;
	std::string hname2 = "hv_"; hname2 += in_hist;
	std::string hname3 = "hw_"; hname3 += in_hist;

	TH2F* hin  = (TH2F*)file->Get(hname1.c_str());
	TH2F* hin2 = (TH2F*)file->Get(hname2.c_str());
	TH2F* hin3 = (TH2F*)file->Get(hname3.c_str());
        hin->Add(hin2);
        hin->Add(hin3);

	TFile* ofile = new TFile(out_file, open_file_mode);

	int nBinsX = x_end_bin - x_start_bin +1;
	int nBinsY = y_end_bin - y_start_bin +1;
	TH2F* hout = new TH2F("","", nBinsX, x_start_bin, x_end_bin, nBinsY, y_start_bin, y_end_bin);

    TH1F* hb=0;
    if (set_baseline){
      hb = new TH1F("","",4096,0, 4096); 
    }
    // TH1F* hb = new TH1F("hb","hb",nBinsY, y_start_bin, y_end_bin); 

	for(int ix=x_start_bin; ix<=x_end_bin; ix++){
		for(int iy=y_start_bin; iy<=y_end_bin; iy++){
			double content = hin->GetBinContent(ix, iy);
			int binx = ix - x_start_bin +1; // bin number in the new histogram
			int biny = iy - y_start_bin +1;
			hout->SetBinContent(binx, biny, content);
            if(set_baseline) hb->Fill(content);
		}
        if (set_baseline){
            float baseline = hb->GetBinCenter(hb->GetMaximumBin());
            // cout << "[wgu] maximum bin: " << hb->GetMaximumBin() << endl;
            // cout << "[wgu] baseline: " << baseline << endl;
			for(int iy=y_start_bin; iy<=y_end_bin; iy++){
				double content = hin->GetBinContent(ix, iy);
				int binx = ix - x_start_bin +1; // bin number in the new histogram
				int biny = iy - y_start_bin +1;
				hout->SetBinContent(binx, biny, content - baseline);
			}
        	hb->Reset();
        }

	}
    delete hb;
	file->Close();

	hout->Rebin2D(x_rebin, y_rebin);

	hout->SetTitle(out_hist);
	hout->SetName(out_hist);
	hout->Write();
	ofile->Close();

}


void preselect(std::string in_file,
               std::string out_file,
               int x1,
               int x2,
               int y1,
               int y2,
               int x_rebin=1,
               int y_rebin=4){

	// std::string in_file="magnify_4696_103-spadj-v2.root";
	// std::string out_file="magnify_4696_103-spadj-v2-presel.root";
	//
	// parameters for 2D sub-regions
	int x_start_bin= x1; // bin starts from 1, chn starts from 0
	int x_end_bin= x2; // [start, end] close bracket at two ends
	//
	int y_start_bin= y1;
	int y_end_bin= y2;
	//
	// int x_rebin=1;
	// int y_rebin=4;

	//
	// preselection of a 2D sub-region
	std::string in_hist="orig";
	std::string out_hist="orig";
	dothis(in_file.c_str(), out_file.c_str(), in_hist.c_str(), out_hist.c_str(),
		   x_start_bin, x_end_bin, y_start_bin, y_end_bin, x_rebin, y_rebin,true,"recreate");

	in_hist="raw";
	out_hist="raw";
	dothis(in_file.c_str(), out_file.c_str(), in_hist.c_str(), out_hist.c_str(),
		   x_start_bin, x_end_bin, y_start_bin, y_end_bin, x_rebin, y_rebin);

	in_hist="decon";
	out_hist="decon";
	dothis(in_file.c_str(), out_file.c_str(), in_hist.c_str(), out_hist.c_str(),
		   x_start_bin, x_end_bin, y_start_bin, y_end_bin, x_rebin, y_rebin);	


}
