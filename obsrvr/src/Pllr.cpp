#include "Pllr.h"

void Pllr::setup(int number, string prt, int bdrt, Fltr &filter, ofPolyline domain) {

    // init serial
    srl.setup(prt, bdrt);

    nmbr = number;
    fltr = &filter;
    dmn = domain;
}

const std::vector<ofPolyline> & Pllr::update(const ofPixels & pxls, const std::vector<ofPolyline> & figures) {

    // check figure centers against domain
    fgrs.clear();
    bool g = false;
    for (std::size_t i = 0; i < fgrs.size(); i++) {
        if (dmn.inside(figures[i].getCentroid2D())) {
            fgrs.push_back(figures[i]);
            g = true;
        }
    }

    // send signal to arduino when ghosts enter/leave
    if (ghstflg != g) {
        ghstflg = g;
        if (ghstflg) {
            // char two(2);
            // srl.writeByte(two);
            ofLogNotice() << "pillar " << nmbr << " ghosted";
        } else {
            // char one(1);
            // srl.writeByte(one);
            ofLogNotice() << "pillar " << nmbr << " *not* ghosted";
        }
    }

    // check serial connection for input
    if(srl.isInitialized() && srl.available() > 0) {
        int b = 0;
        b = srl.readByte();

        ofLogNotice() << "pillar " << nmbr << " read: " << b;

        if (b == 97) hallflg = false; // 97 -> 'a'
        if (b == 98) hallflg = true; // 98 -> 'b'
        if (b == 99) swtchflg = false; // 99 -> 'c'
        if (b == 100) swtchflg = true; // 100 -> 'd'
    }

    // when switch is on update filter
    if (swtchflg) {
        fltr->update(pxls, fgrs);
        static std::vector<ofPolyline> empty;
        return empty; // return empty list of figures
    }

    return fgrs; // if switch is off, figures are ghosts return list
}

bool Pllr::switched() {
    return swtchflg;
}

// when switch is on draw filter
void Pllr::draw() {
    if (swtchflg) fltr->draw();
}