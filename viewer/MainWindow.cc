#include "MainWindow.h"
#include "ViewWindow.h"
#include "ControlWindow.h"
#include "TGLayout.h"
#include "TGMenu.h"

#include "TString.h"

#include <iostream>
using namespace std;

MainWindow::MainWindow(const TGWindow *p, int w,int h)
    : TGMainFrame(p, w, h)
{
    InitMenu();

    fViewAndControlFrame = new TGVerticalFrame(this, w, h);
    AddFrame(fViewAndControlFrame, new TGLayoutHints(kLHintsExpandX  | kLHintsExpandY, 2, 2, 2, 2));

    int controlWindowHeight = 100;
    fViewWindow = new ViewWindow(fViewAndControlFrame, w, h-controlWindowHeight);
    fViewAndControlFrame->AddFrame(fViewWindow, new TGLayoutHints(
        kLHintsTop | kLHintsExpandX | kLHintsExpandY , 2, 2, 2, 2));

    fControlWindow = new ControlWindow(fViewAndControlFrame, w, controlWindowHeight);
    fViewAndControlFrame->AddFrame(fControlWindow, new TGLayoutHints(
        kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));

    // set main window stats
    SetWindowName("Magnify");
    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();
}

void MainWindow::InitMenu()
{
    fMenuBar = new TGMenuBar(this, 10, 100);
    AddFrame(fMenuBar, new TGLayoutHints(kLHintsTop | kLHintsExpandX));
    fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);

    fMenuFile = new TGPopupMenu(gClient->GetRoot());
    // fMenuFile->AddEntry("&Open...", M_FILE_OPEN);
    // fMenuFile->AddSeparator();
    fMenuFile->AddEntry(new TGHotString("E&xit"), M_FILE_EXIT);

    fMenuBar->AddPopup(new TGHotString("&File"), fMenuFile, fMenuBarItemLayout);

}


MainWindow::~MainWindow()
{
}

