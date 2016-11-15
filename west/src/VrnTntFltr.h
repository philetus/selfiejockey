/*
 * video filter to render a single polyline figure
 */
#pragma once

#include "ofMain.h"
#include "Fltr.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxSwpr.h"

class VrnTntFltr : public Fltr {
public:

    void setup(ofRectangle source, ofRectangle target);
    void update(ofPixels pxls, ofPixels edgpxls, const std::vector<ofPolyline> & fgrs);
    void draw();

    static bool cellInRect(ofRectangle rct, 
        const std::vector<glm::vec3> & srcs, 
        const std::vector<glm::vec3> & gens,
        const ofxSwpr::Cll & cll) 
    {
        // test source vertex
        if (!rct.inside(srcs[cll.srcdx])) return false;

        // test edge vertices
        for (std::size_t i = 0; i < cll.gendxs.size(); i++)
            if (!rct.inside(gens[cll.gendxs[i]])) return false;

        return true;
    }

    ofxSwpr::Swpr swpr;
    ofVboMesh msh;

    ofColor tntclr;

    ofParameter<int> samples;

    ofRectangle src, trgt; // source & target rectangles
    float xdlt, ydlt, xscl, yscl;
};
