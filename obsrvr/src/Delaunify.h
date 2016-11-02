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

    void setup(int w, int h);
    void update(ofPixels & edgePx, ofPixels & colorPx, const std::vector<ofPolyline> & fgrs);
    void draw();

    int wdth, hght;
    ofxDelaunay delaunay;
    ofVboMesh mesh;

    ofParameter<float> power; //sigma1, sigma2, tau;
    ofParameter<int> stride, noise, edgeThresh, alpha; // cannyParam1, cannyParam2;
    //ofParameter<bool> doFDoG, doThresh, doThin, doCanny, neg;	

};
