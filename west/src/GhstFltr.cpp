#include "GhstFltr.h"

using namespace ofxCv;
using namespace cv;

void GhstFltr::setup(ofRectangle source, ofRectangle target) {
    src = source;
    trgt = target; 
    xdlt = trgt.getMinX() - src.getMinX();
    ydlt = trgt.getMinY() - src.getMinY();
    xscl = trgt.getWidth() / src.getWidth();
    yscl = trgt.getHeight() / src.getHeight();
 
    samples.set("background samples", 6000, 1, 10000);
    bgAlpha.set("background alpha", 127, 0, 255);
    ghostAlpha.set("ghost alpha", 191, 0, 255);
}

void GhstFltr::update(ofPixels pxls, ofPixels edgpxls, const std::vector<ofPolyline> & fgrs) {
    dlny.reset();
    msh.clear();

    // monte carlo sample for points on canny edges
    int notEdgeVal = 0;
    int pcnt = samples;
    while (pcnt > 0) {
        glm::vec3 p = randInRect(src);
        int b = edgpxls[((int)p.y * src.width) + (int)p.x];
        if (b != notEdgeVal) {
            dlny.addPoint(p);
            pcnt--;
        }
    }

    // add figure points to delaunay diagram
    for(int i = 0; i < fgrs.size(); i++) {
        for(int j = 0; j < fgrs[i].size(); j++) {
            dlny.addPoint(glm::vec3(fgrs[i][j].x, fgrs[i][j].y, 0));
        }
    }

    // add frame & triangulate
    dlny.addPoint(src.getBottomLeft());
    dlny.addPoint(src.getBottomRight());
    dlny.addPoint(src.getTopRight());
    dlny.addPoint(src.getTopLeft());
    dlny.triangulate();

    // sample colors
    for (std::size_t i = 0; i < dlny.triangleMesh.getNumIndices()/3; i++) {
        int idx0 = dlny.triangleMesh.getIndex(i*3);
        int idx1 = dlny.triangleMesh.getIndex(i*3+1);
        int idx2 = dlny.triangleMesh.getIndex(i*3+2);
         
        glm::vec3 v0 = dlny.triangleMesh.getVertex(idx0);
        glm::vec3 v1 = dlny.triangleMesh.getVertex(idx1);
        glm::vec3 v2 = dlny.triangleMesh.getVertex(idx2);
         
        glm::vec3 gp = (v0+v1+v2)/3.0;
         
        ofColor clr = pxls.getColor((int)(gp.x), (int)(gp.y));
        clr.a = bgAlpha;

        // if center is in a figure set alpha to ghost alpha
        if (inFigures(fgrs, gp)) {
            clr.a = ghostAlpha;
        }

        // adjust vertices to target scale
        glm::vec3 nv0((v0.x * xscl) + xdlt, (v0.y * yscl) + ydlt, 0);
        glm::vec3 nv1((v1.x * xscl) + xdlt, (v1.y * yscl) + ydlt, 0);
        glm::vec3 nv2((v2.x * xscl) + xdlt, (v2.y * yscl) + ydlt, 0);
         
        msh.addVertex(nv0);
        msh.addVertex(nv1);
        msh.addVertex(nv2);
         
        msh.addColor(clr);
        msh.addColor(clr);
        msh.addColor(clr);
    }
}

void GhstFltr::draw() {
    msh.draw();    
}