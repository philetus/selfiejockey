#include "CannyDlnyFltr.h"

using namespace ofxCv;
using namespace cv;

void CannyDlnyFltr::setup(ofRectangle source, ofRectangle target) {
    src = source;
    trgt = target; 
    xdlt = trgt.x - src.x;
    ydlt = trgt.y - src.y;
    xscl = trgt.width / (float)src.width;
    yscl = trgt.height / (float)src.height;

    ofLogNotice() << "xdlt " << xdlt << " ydlt " << ydlt << " xscl " << xscl << " yscl " << yscl;

    inpt.allocate(src.width, src.height, OF_IMAGE_GRAYSCALE);
    cnny.allocate(src.width, src.height, OF_IMAGE_GRAYSCALE);
 
    samples.set("samples", 1000, 1, 10000);
    neg.set("neg", true);
    doCanny.set("doCanny", false);
    cannyParam1.set("cannyParam1", 300, 0, 1024);
    cannyParam2.set("cannyParam2", 150, 0, 1024);

    empty = true;
}

void CannyDlnyFltr::update(ofPixels pxls, const std::vector<ofPolyline> & fgrs) {
    dlny.reset();
    msh.clear();

    // if there are no figures set empty flag and return
    if (fgrs.size() < 1) {
        empty = true;
        return;
    }
    empty = false;

    // add figures to delaunay mesh
    ofRectangle roi(fgrs[0].getBoundingBox()); // get region containing all figures
    for (std::size_t i = 0; i < fgrs.size(); i++) {
        for (std::size_t j = 0; j < fgrs[i].size(); j++) {
            dlny.addPoint(glm::vec3(fgrs[i][j].x, fgrs[i][j].y, 0));
            roi.growToInclude(fgrs[0].getBoundingBox());
        }
    }

    // ofLogNotice() << "rendering canny delaunay filter for roi: " << roi;

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

        // get random point in region of interest
        glm::vec3 p = randInRect(roi); 

        // test if point is on an edge
        int b = edgpxls[((int)p.y * src.width) + (int)p.x];
        if (b != notEdgeVal) {

            // if point is on edge test if it is also in a figure
            if (infgrs(fgrs, p)) {
                dlny.addPoint(p);
                pcnt--;
            }
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
}

void CannyDlnyFltr::draw() {
    if (empty) return; // check that we generated a mesh already

    // ofLogNotice() << "drawing canny delaunay mesh";

    if (neg) {
        ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
        msh.draw();
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    } else {
        msh.draw();
    }
}