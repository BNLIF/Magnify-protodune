#ifndef BADCHANNELS_H
#define BADCHANNELS_H

class TTree;

#include <vector>

class BadChannels {
public:
    BadChannels();
    BadChannels(TTree* t);

    virtual ~BadChannels();

    // void load_badchannels();

    vector<int> bad_id;
    vector<int> bad_start;
    vector<int> bad_end;

};

#endif
