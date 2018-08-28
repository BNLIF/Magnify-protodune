void loadClasses()
{
    TString include = ".include ";
    TString load = ".L ";
    TString pwd = gSystem->pwd();
    pwd = pwd + '/';
//    gSystem->Load("libGLEW");
//    gSystem->Load("libFTGL");

    TString prefix;
    prefix = "../event";
    gROOT->ProcessLine( include + pwd + prefix );
    gROOT->ProcessLine( load + prefix + "/BadChannels.cc+" );
    gROOT->ProcessLine( load + prefix + "/RawWaveforms.cc+" );
    gROOT->ProcessLine( load + prefix + "/Waveforms.cc+" );
    gROOT->ProcessLine( load + prefix + "/Data.cc+" );

    prefix = "../viewer";
    gROOT->ProcessLine( include + pwd + prefix );
    gROOT->ProcessLine( load + prefix + "/ViewWindow.cc+" );
    gROOT->ProcessLine( load + prefix + "/ControlWindow.cc+" );
    gROOT->ProcessLine( load + prefix + "/MainWindow.cc+" );
    gROOT->ProcessLine( load + prefix + "/GuiController.cc+" );
}
