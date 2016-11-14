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
    void update(ofPixels pxls, const std::vector<ofPolyline> & fgrs);
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

    static bool cllinfgrs(const std::vector<ofPolyline> & fgrs, 
        const std::vector<glm::vec3> & srcs, 
        const std::vector<glm::vec3> & gens,
        const ofxSwpr::Cll & cll) 
    {
        // test source vertex
        if (!infgrs(fgrs, srcs[cll.srcdx])) return false;

        // test edge vertices
        for (std::size_t i = 0; i < cll.gendxs.size(); i++)
            if (!infgrs(fgrs, gens[cll.gendxs[i]])) return false;

        return true;
    }

    ofImage inpt, cnny;

    ofxSwpr::Swpr swpr;
    ofVboMesh msh;
    bool empty;

    ofColor tntclr;

    ofParameter<int> samples, cannyParam1, cannyParam2;
    // ofParameter<bool> doCanny, neg;

    ofRectangle src, trgt; // source & target rectangles
    float xdlt, ydlt, xscl, yscl;
};
