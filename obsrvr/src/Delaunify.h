/*
track background and identify foreground elements in video stream
*/

#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxDelaunay.h"

class Delaunify {

public:

    void setup(ofRectangle source, ofRectangle target);
    void update(ofPixels & colorPx, const std::vector<ofPolyline> & fgrs);
    void draw();

    static glm::vec3 randInRect(const ofRectangle &r) {
        float rx = ofRandom(r.x, r.x + r.width);
        float ry = ofRandom(r.y, r.y + r.height);
        return glm::vec3(rx, ry, 0);
    }

    static bool infgrs(const std::vector<ofPolyline> & fgrs, glm::vec3 pnt) {
        for (std::size_t i = 0; i < fgrs.size(); i++) {
            if (fgrs[i].inside(pnt)) return true;
        }
        return false;
    }

    ofRectangle src, trgt; // source & target rectangles
    float xdlt, ydlt, xscl, yscl;

    ofImage inpt, cnny;
    ofxDelaunay dlny;
    ofVboMesh msh;

    ofParameter<int> samples, cannyParam1, cannyParam2, alpha;
    ofParameter<bool> neg;
};
