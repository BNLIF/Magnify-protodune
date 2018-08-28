#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "TGFrame.h"

class ViewWindow;
class ControlWindow;
class TGMenuBar;
class TGPopupMenu;

enum EMenuIds {
   M_FILE_OPEN,
   M_FILE_EXIT,
};

class MainWindow: public TGMainFrame
{
public:
    MainWindow(const TGWindow *p, int w,int h);
    virtual ~MainWindow();

public:
    TGVerticalFrame   *fViewAndControlFrame;
    ViewWindow        *fViewWindow;
    ControlWindow     *fControlWindow;

    TGMenuBar         *fMenuBar;
    TGPopupMenu       *fMenuFile;
    TGLayoutHints     *fMenuBarItemLayout;

    void InitMenu();

    ClassDef(MainWindow, 0)
};

#endif


