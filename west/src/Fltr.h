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
    virtual void update(ofPixels pxls, ofPixels edgpxls, const std::vector<ofPolyline> & fgrs) = 0;
    virtual void draw() = 0;

    static glm::vec3 randInRect(const ofRectangle &r) {
        float rx = ofRandom(r.x, r.x + r.width);
        float ry = ofRandom(r.y, r.y + r.height);
        return glm::vec3(rx, ry, 0);
    }

    static bool inFigures(const std::vector<ofPolyline> & fgrs, glm::vec3 pnt) {
        for (std::size_t i = 0; i < fgrs.size(); i++) {
            if (fgrs[i].inside(pnt)) return true;
        }
        return false;
    }
};
