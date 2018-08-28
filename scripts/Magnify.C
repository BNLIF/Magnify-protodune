void Magnify(const char* filename=0, const char* frame="decon", int rebin=4);
void Magnify(const char* filename, const char* frame, int rebin)
{
    // gROOT->Reset();
    // gROOT->ProcessLine(".x loadClasses.C" );

    GuiController *gc = new GuiController(
        gClient->GetRoot(),
        1600,
        900,
        filename,
        frame,
        rebin
    );
}
