#include "ControlWindow.h"

#include "TGButton.h"
#include "TGNumberEntry.h"
#include "TGButtonGroup.h"
#include "TGListBox.h"
#include "TGLabel.h"
#include "TGDoubleSlider.h"

#include <iostream>
using namespace std;

ControlWindow::ControlWindow(const TGWindow *p, int w, int h)
    :TGHorizontalFrame(p, w, h)
{
    TGGroupFrame *group_general = new TGGroupFrame(this, "General", kHorizontalFrame);
    group_general->SetTitlePos(TGGroupFrame::kLeft);
    AddFrame(group_general, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 1, 1));

    group_general->AddFrame(new TGLabel(group_general, "channel: "), new TGLayoutHints(kLHintsTop | kLHintsLeft,  2, 2, 1, 1));
    channelEntry = new TGNumberEntry(group_general, 0, 5, -1,
        TGNumberFormat::kNESInteger,
        TGNumberFormat::kNEANonNegative,
        TGNumberFormat::kNELLimitMinMax,
        0, 8255);
    group_general->AddFrame(channelEntry, new TGLayoutHints(kLHintsTop | kLHintsLeft,  1, 1, 1, 1));

    rawWfButton = new TGCheckButton(group_general, "raw waveform  ");
    rawWfButton->SetState(kButtonUp);
    group_general->AddFrame(rawWfButton, new TGLayoutHints(kLHintsTop | kLHintsLeft,  1, 1, 1, 1));

    badChanelButton = new TGCheckButton(group_general, "bad channel  ");
    badChanelButton->SetState(kButtonUp);
    group_general->AddFrame(badChanelButton, new TGLayoutHints(kLHintsTop | kLHintsLeft,  1, 1, 1, 1));

    setThreshButton = new TGTextButton(group_general, "ch. thresh. x");
    group_general->AddFrame(setThreshButton, new TGLayoutHints(kLHintsTop | kLHintsLeft, 3, 2, 2, 2));
    threshScaleEntry = new TGNumberEntry(group_general, 3, 5, -1,
        TGNumberFormat::kNESReal,
        TGNumberFormat::kNEANonNegative,
        TGNumberFormat::kNELLimitMinMax,
        0, 10);
    threshScaleEntry->SetNumber(1);
    group_general->AddFrame(threshScaleEntry, new TGLayoutHints(kLHintsTop | kLHintsLeft,  1, 1, 1, 1));

    group_general->AddFrame(new TGLabel(group_general, "tick: "), new TGLayoutHints(kLHintsTop | kLHintsLeft,  2, 2, 1, 1));
    timeEntry = new TGNumberEntry(group_general, 0, 5, -1,
        TGNumberFormat::kNESInteger,
        TGNumberFormat::kNEANonNegative,
        TGNumberFormat::kNELLimitMinMax,
        0, 8255);
    group_general->AddFrame(timeEntry, new TGLayoutHints(kLHintsTop | kLHintsLeft,  1, 1, 1, 1));

    timeModeButton = new TGCheckButton(group_general, "time mode  ");
    timeModeButton->SetState(kButtonUp);
    group_general->AddFrame(timeModeButton, new TGLayoutHints(kLHintsTop | kLHintsLeft,  1, 1, 1, 1));

    TGGroupFrame *group[3];
    for (int i=0; i<3; i++) {
        group[i] = new TGGroupFrame(this, "", kHorizontalFrame);
        group[i]->SetTitlePos(TGGroupFrame::kLeft);
        AddFrame(group[i], new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 1, 1));

        group[i]->AddFrame(new TGLabel(group[i], "threshold: "), new TGLayoutHints(kLHintsTop | kLHintsLeft,  2, 2, 1, 1));
        threshEntry[i] = new TGNumberEntry(group[i], 3, 5, -1,
            TGNumberFormat::kNESInteger,
            TGNumberFormat::kNEANonNegative,
            TGNumberFormat::kNELLimitMinMax,
            3, 30);
        group[i]->AddFrame(threshEntry[i], new TGLayoutHints(kLHintsTop | kLHintsLeft,  1, 1, 1, 1));
    }
    group[0]->SetTitle("U Plane");
    group[1]->SetTitle("V Plane");
    group[2]->SetTitle("W Plane");

    TGGroupFrame *group_misc = new TGGroupFrame(this, "Range", kHorizontalFrame);
    group_misc->SetTitlePos(TGGroupFrame::kLeft);
    AddFrame(group_misc, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 1, 1));

    group_misc->AddFrame(new TGLabel(group_misc, "x range: "), new TGLayoutHints(kLHintsTop | kLHintsLeft,  2, 2, 1, 1));
    for (int i=0; i<2; i++) {
        timeRangeEntry[i] = new TGNumberEntry(group_misc, 0, 5, -1,
            TGNumberFormat::kNESInteger,
            TGNumberFormat::kNEAAnyNumber,
            TGNumberFormat::kNELLimitMinMax,
            0, 10000);
        group_misc->AddFrame(timeRangeEntry[i], new TGLayoutHints(kLHintsTop | kLHintsLeft,  1, 1, 1, 1));
    }

    group_misc->AddFrame(new TGLabel(group_misc, "y range: "), new TGLayoutHints(kLHintsTop | kLHintsLeft,  2, 2, 1, 1));
    for (int i=0; i<2; i++) {
        adcRangeEntry[i] = new TGNumberEntry(group_misc, 0, 5, -1,
            TGNumberFormat::kNESInteger,
            TGNumberFormat::kNEAAnyNumber,
            TGNumberFormat::kNELLimitMinMax,
            -1000, 1000);
        group_misc->AddFrame(adcRangeEntry[i], new TGLayoutHints(kLHintsTop | kLHintsLeft,  1, 1, 1, 1));
    }

    unZoomButton = new TGTextButton(group_misc, "UnZoom");
    group_misc->AddFrame(unZoomButton, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));

    group_misc->AddFrame(new TGLabel(group_misc, "color range: "), new TGLayoutHints(kLHintsTop | kLHintsLeft,  2, 2, 1, 1));
    for (int i=0; i<2; i++) {
        zAxisRangeEntry[i] = new TGNumberEntry(group_misc, 10, 5, -1,
            TGNumberFormat::kNESInteger,
            TGNumberFormat::kNEAAnyNumber,
            TGNumberFormat::kNELLimitMinMax,
            -100, 100);
        group_misc->AddFrame(zAxisRangeEntry[i], new TGLayoutHints(kLHintsTop | kLHintsLeft,  1, 1, 1, 1));
    }





}

ControlWindow::~ControlWindow()
{
}
