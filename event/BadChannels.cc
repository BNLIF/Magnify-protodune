#include "BadChannels.h"


#include "TH2F.h"
#include "TH2I.h"
#include "TH1I.h"
#include "TTree.h"

#include <vector>

using namespace std;

BadChannels::BadChannels()
{}

BadChannels::BadChannels(TTree* t)
{
    if (t) {
        int chid, start, end;
        t->SetBranchAddress("chid", &chid);
        t->SetBranchAddress("start_time", &start);
        t->SetBranchAddress("end_time", &end);
        int nEntries = t->GetEntries();
        for (int i=0; i<nEntries; i++) {
            t->GetEntry(i);
            bad_id.push_back(chid);
            bad_start.push_back(start);
            bad_end.push_back(end);
            // cout << chid << endl;
        }
    }
}

BadChannels::~BadChannels()
{
}