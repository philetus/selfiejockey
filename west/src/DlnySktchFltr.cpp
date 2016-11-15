#include "DlnySktchFltr.h"

using namespace ofxCv;
using namespace cv;

void DlnySktchFltr::setup(ofRectangle source, ofRectangle target) {
    src = source;
    trgt = target; 
    xdlt = trgt.x - src.x;
    ydlt = trgt.y - src.y;
    xscl = trgt.width / (float)src.width;
    yscl = trgt.height / (float)src.height;

    ofLogNotice() << "xdlt " << xdlt << " ydlt " << ydlt << " xscl " << xscl << " yscl " << yscl;

    samples.set("delaunay sketch samples", 8000, 1, 10000);
    alpha.set("delaunay sketch alpha", 191, 0, 255);
    power.set("power", 2, 0.5, 8.0);
    stroke.set("stroke", 3, 1, 5);
    flip.set("flip", true);
}

void DlnySktchFltr::update(ofPixels pxls, ofPixels edgpxls, const std::vector<ofPolyline> & fgrs) {
    dlny.reset();
    msh.clear();

    float low = 0;
    float high = 1;
    if (flip) {
        low = 1;
        high = 0;
    }

    // generate random points weighted for brightness
    int pcnt = samples;
    while (pcnt > 0) {
        glm::vec3 p = randInRect(src);
        float b = ofMap(
            pxls.getColor((int)p.x, (int)p.y).getBrightness(),
            0, 255, high, low);
        if (pow(b, power) > ofRandom(1)) {
            dlny.addPoint(p);
            pcnt--;
        }
    }

    // triangulate
    dlny.triangulate();

    // sample colors from unfiltered image pixels
    for (int i=0; i < dlny.triangleMesh.getNumIndices()/3; i++) {
        int idx0 = dlny.triangleMesh.getIndex(i*3);
        int idx1 = dlny.triangleMesh.getIndex(i*3+1);
        int idx2 = dlny.triangleMesh.getIndex(i*3+2);
         
        glm::vec3 v0 = dlny.triangleMesh.getVertex(idx0);
        glm::vec3 v1 = dlny.triangleMesh.getVertex(idx1);
        glm::vec3 v2 = dlny.triangleMesh.getVertex(idx2);
         
        glm::vec3 gp = (v0+v1+v2)/3.0;
        
        ofColor clr = pxls.getColor((int)gp.x, (int)gp.y);
        clr.a = alpha;
        if (inFigures(fgrs, gp)) clr.a = 255; // ghost figures

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

void DlnySktchFltr::draw() {
    // ofLogNotice() << "drawing canny delaunay mesh";

    if (flip) {
        ofBackground(0);
    } else {
        ofBackground(255);
    }
    ofSetLineWidth(stroke);
    //ofSetColor(255);
    msh.draw(OF_MESH_WIREFRAME);
}