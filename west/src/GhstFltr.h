/*
track background and identify foreground elements in video stream
*/

#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxDelaunay.h"
#include "Fltr.h"

class GhstFltr : public Fltr {

public:

    void setup(ofRectangle source, ofRectangle target);
    void update(ofPixels pxls, ofPixels edgpxls, const std::vector<ofPolyline> & fgrs);
    void draw();

    ofRectangle src, trgt; // source & target rectangles
    float xdlt, ydlt, xscl, yscl;

    ofxDelaunay dlny;
    ofVboMesh msh;

    ofParameter<int> samples, bgAlpha, ghostAlpha;
};
