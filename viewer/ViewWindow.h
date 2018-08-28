#ifndef VIEW_WINDOW_H
#define VIEW_WINDOW_H

#include "TGFrame.h"
#include "TRootEmbeddedCanvas.h"

class TPad;

class ViewWindow: public TRootEmbeddedCanvas
{
public:
    ViewWindow(const TGWindow *p, int w,int h);
    virtual ~ViewWindow();

    void InitCanvas();
    void SetStyle();
    void SetPalette(int i);
    void PaletteRainbow();
    void PaletteGray();
    void PaletteSummer();
    void PaletteGrayInv();
    void PaletteFire();
    void SetTheme(int theme);
    TCanvas* can;

    ClassDef(ViewWindow, 0)
};

#endif
