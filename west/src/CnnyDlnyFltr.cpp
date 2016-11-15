#include "CnnyDlnyFltr.h"

using namespace ofxCv;
using namespace cv;

void CnnyDlnyFltr::setup(ofRectangle source, ofRectangle target) {
    src = source;
    trgt = target; 
    xdlt = trgt.x - src.x;
    ydlt = trgt.y - src.y;
    xscl = trgt.width / (float)src.width;
    yscl = trgt.height / (float)src.height;

    ofLogNotice() << "xdlt " << xdlt << " ydlt " << ydlt << " xscl " << xscl << " yscl " << yscl;

    samples.set("canny delaunay samples", 6000, 1, 10000);
    neg.set("neg", true);
    doCanny.set("doCanny", false);

    cnny.allocate(src.width, src.height, OF_IMAGE_GRAYSCALE);
}

void CnnyDlnyFltr::update(ofPixels pxls, ofPixels edgpxls, const std::vector<ofPolyline> & fgrs) {
    dlny.reset();
    msh.clear();

    cnny.setFromPixels(edgpxls);

    int notEdgeVal = 0;
    if (neg) {
        invert(cnny);
        cnny.update();
        edgpxls = cnny.getPixels();
        notEdgeVal = 255;
    } else {
        cnny.update();
    }

    // monte carlo sample for points on canny edges
    int pcnt = samples;
    while (pcnt > 0) {

        // get random point in region of interest
        glm::vec3 p = randInRect(src); 

        // test if point is on an edge
        int b = edgpxls[((int)p.y * src.width) + (int)p.x];
        if (b != notEdgeVal) {
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
    if (doCanny) {
        cnny.resize(trgt.width, trgt.height);
        cnny.update();
    }
}

void CnnyDlnyFltr::draw() {
    // ofLogNotice() << "drawing canny delaunay mesh";

    if(doCanny && neg){
        cnny.draw(trgt.x, trgt.y);
        ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
        msh.draw();
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    } else if (neg) {
        ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
        msh.draw();
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    } else if (doCanny) {
        msh.draw();
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        ofSetColor(255, 255, 255, 127);
        cnny.draw(trgt.x, trgt.y);
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        ofSetColor(255);
    } else {
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        msh.draw();
    }
}