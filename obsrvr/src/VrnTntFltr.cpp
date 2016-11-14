#include "VrnTntFltr.h"

using namespace ofxCv;
using namespace cv;

void VrnTntFltr::setup(ofRectangle source, ofRectangle target) {
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
    // neg.set("neg", true);
    // doCanny.set("doCanny", false);
    cannyParam1.set("cannyParam1", 300, 0, 1024);
    cannyParam2.set("cannyParam2", 150, 0, 1024);

    tntclr.setHex(0x00ffaa, 0xaa); // resin tint color
    empty = true;
}

void VrnTntFltr::update(ofPixels pxls, const std::vector<ofPolyline> & fgrs) {
    swpr.clear();
    msh.clear();

    // if there are no figures set empty flag and return
    if (fgrs.size() < 1) {
        empty = true;
        return;
    }
    empty = false;

    // add figures to voronoi sweeper
    ofRectangle roi(fgrs[0].getBoundingBox()); // get region containing all figures
    for (std::size_t i = 0; i < fgrs.size(); i++) {
        for (std::size_t j = 0; j < fgrs[i].size(); j++) {
            swpr.addSource(glm::vec3(fgrs[i][j].x, fgrs[i][j].y, 0));
            roi.growToInclude(fgrs[0].getBoundingBox());
        }
    }

    // ofLogNotice() << "rendering voronoi tint filter for roi: " << roi;

    // convert image to grayscale and run canny edge filter
    ofxCv::copyGray(pxls, inpt);
    Canny(inpt, cnny, cannyParam1 * 2, cannyParam2 * 2, 5);
    int notEdgeVal = 0;

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
            // if (infgrs(fgrs, p)) {

            swpr.addSource(p);
            pcnt--;

        // }
        }
    }

    // triangulate
    swpr.diagram();

    // set cells to tinted colors based on sampled brightness
    for(std::size_t i = 0; i < swpr.clls.size(); i++) {
        ofColor c = ofColor(tntclr);
        glm::vec3 src = swpr.srcs[swpr.clls[i].srcdx];
        c.setBrightness(pxls.getColor((int)src.x, (int)src.y).getBrightness());
        swpr.clls[i].setColor(c);
    }

    // generate cell vertices for target coords
    std::vector<glm::vec3> tsrcs, tgens;
    for (std::size_t i = 0; i < swpr.srcs.size(); i++) {
        tsrcs.push_back(glm::vec3(
            (swpr.srcs[i].x * xscl) + xdlt, 
            (swpr.srcs[i].y * yscl) + ydlt, 
            0));
    }
    for (std::size_t i = 0; i < swpr.gens.size(); i++) {
        tgens.push_back(glm::vec3(
            (swpr.gens[i].x * xscl) + xdlt, 
            (swpr.gens[i].y * yscl) + ydlt, 
            0));
    }

    // build mesh
    msh.setMode(OF_PRIMITIVE_TRIANGLES);

    // build triangles for each voronoi cell inside a figure
    for (std::size_t i = 0; i < swpr.clls.size(); i++) {
        if (cllinfgrs(fgrs, swpr.srcs, swpr.gens, swpr.clls[i])) {
            swpr.clls[i].mesh(msh, tsrcs, tgens);
        }
    }    
}

void VrnTntFltr::draw() {
    if (empty) return; // check that we generated a mesh already

    // ofLogNotice() << "drawing voronoi tint mesh";

    msh.draw(OF_MESH_FILL);
}