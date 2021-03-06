#include "Delaunify.h"

using namespace ofxCv;
using namespace cv;

void Delaunify::setup(ofRectangle source, ofRectangle target) {
    src = source;
    trgt = target; 
    xdlt = trgt.getMinX() - src.getMinX();
    ydlt = trgt.getMinY() - src.getMinY();
    xscl = trgt.getWidth() / src.getWidth();
    yscl = trgt.getHeight() / src.getHeight();

    inpt.allocate(src.getWidth(), src.getHeight(), OF_IMAGE_GRAYSCALE);
    cnny.allocate(src.getWidth(), src.getHeight(), OF_IMAGE_GRAYSCALE);
 
    samples.set("samples", 4000, 1, 10000);
    neg.set("neg", true);
    cannyParam1.set("cannyParam1", 300, 0, 1024);
    cannyParam2.set("cannyParam2", 150, 0, 1024);
    alpha.set("alpha", 127, 0, 255);
}

void Delaunify::update(ofPixels & pxls, const std::vector<ofPolyline> & fgrs) {
    dlny.reset();
    msh.clear();

    // convert image to grayscale and run canny edge filter
    ofxCv::copyGray(pxls, inpt);
    Canny(inpt, cnny, cannyParam1 * 2, cannyParam2 * 2, 5);
    int notEdgeVal = 0;
    if (neg) {
        invert(cnny);
        notEdgeVal = 255;
    }
    cnny.update();
    ofPixels edgpxls = cnny.getPixels();

    // monte carlo sample for points on canny edges
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
        clr.a = alpha;

        // if center is in a figure set alpha to 255
        if (infgrs(fgrs, gp)) {
            clr.a = 255;
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

void Delaunify::draw() {
    msh.draw();    
}