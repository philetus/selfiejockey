/*
 *
 */
#pragma once

#include "ofMain.h"
#include "Fltr.h"

class Pllr {
public:

    Pllr() {}
    ~Pllr() {}

    void setup(int number, ofSerial serial, Fltr &filter, ofPolyline domain);
    const std::vector<ofPolyline> & update(const ofPixels & pxls, const std::vector<ofPolyline> & figures);
    void draw();
    bool switched();

    ofSerial srl;
    int nmbr;
    ofRectangle src, trgt;
    ofPolyline dmn; // domain of this pillar in source frame
    bool hallflg, swtchflg, ghstflg;

    std::vector<ofPolyline> fgrs;
    Fltr *fltr; // video filter to render figures
};
