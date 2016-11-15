/*
 * video filter to render a single polyline figure
 */
#pragma once

#include "ofMain.h"
#include "Fltr.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxDelaunay.h"

class DlnySktchFltr : public Fltr {
public:

    void setup(ofRectangle source, ofRectangle target);
    void update(ofPixels pxls, ofPixels edgpxls, const std::vector<ofPolyline> & fgrs);
    void draw();

    ofxDelaunay dlny;
    ofVboMesh msh;
    ofImage cnny;

    ofParameter<int> samples, stroke, alpha;
    ofParameter<float> power;    
    ofParameter<bool> flip;    

    ofRectangle src, trgt; // source & target rectangles
    float xdlt, ydlt, xscl, yscl;
};
