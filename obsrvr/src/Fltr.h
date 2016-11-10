/*
 * video filter to render a single polyline figure
 */
#pragma once

#include "ofMain.h"

class Fltr {
public:
    Fltr() {}
    ~Fltr() {}

    virtual void setup(ofRectangle source, ofRectangle target) = 0;
    virtual void update(ofPixels pxls, const std::vector<ofPolyline> & fgrs) = 0;
    virtual void draw() = 0;

};
