#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetVerticalSync(true);
    //ofBackgroundHex(0xFFFF00);

    camWdth = 1920;
    camHght = 1080;

    setCam(&cam, "C920"); // try to find external webcam
    //cam.setDesiredFrameRate(30);
    cam.initGrabber(camWdth, camHght);
    
    ofBackground(255);
    live.allocate(camWdth, camHght);
    input.allocate(camWdth, camHght);
    output.allocate(camWdth, camHght, OF_IMAGE_GRAYSCALE);
    canny.allocate(camWdth, camHght, OF_IMAGE_GRAYSCALE);

    gui.setup();
    gui.add(samples.set("samples", 4000, 1, 10000));
    // gui.add(noise.set("noise", 100, 0, 1000));
    // gui.add(neg.set("neg", true));
    gui.add(shrink.set("shrink", 0.2, 0.05, 0.95));
    gui.add(doCanny.set("doCanny", false));
    gui.add(cannyParam1.set("cannyParam1", 300, 0, 1024));
    gui.add(cannyParam2.set("cannyParam2", 150, 0, 1024));

    tntclr.setHex(0x0000ff); // resin tint color
}

void ofApp::update() {
    cam.update();
     if(cam.isFrameNew()) {

        swpr.clear();
        bmbs.clear();

        ofPixels pixels = cam.getPixels();

        pixels.mirror(false, true);

        live.setFromPixels(pixels);
        input.setFromColorImage(live);

        Canny(input, canny, cannyParam1 * 2, cannyParam2 * 2, 5);
        int notEdgeVal = 0;
        canny.update();

        ofPixels & edgePixels = canny.getPixels();

        float w = camWdth;
        float h = camHght;

        int pcnt = samples;
        while (pcnt > 0) {
            glm::vec3 p(ofRandom(0, w), ofRandom(0, h), 0);
            int b = edgePixels.getColor((int)p.x, (int)p.y).getBrightness();
            if (b != notEdgeVal) {
                swpr.addSource(p);
                pcnt--;
            }
        }

        // regenerate voronoi mesh
		swpr.diagram();

        // generate bamboo cell polyline for each cell
        for(std::size_t i = 0; i < swpr.clls.size(); i++) {
            if (!swpr.clls[i].opn) {
                ofPolyline bmb;

                glm::vec3 src = swpr.srcs[swpr.clls[i].srcdx]; // source vertex

                for (std::size_t j = 0; j < swpr.clls[i].gendxs.size(); j++) {
                    glm::vec3 edgv = swpr.gens[swpr.clls[i].gendxs[j]];
                    glm::vec3 shrnk = src - edgv;
                    shrnk *= shrink;
                    glm::vec3 nwv = edgv + shrnk;
                    bmb.addVertex(nwv);
                }
                bmb.close();
                bmb = bmb.getSmoothed(2, 0.5);

                if (bmb.size() > 2) bmbs.push_back(bmb);
            }
        }
	}
}

void ofApp::draw() {

    //ofFill();
    ofSetLineWidth(2.0);  // Line widths apply to polylines
    ofSetColor(0);
    
    for (std::size_t i = 0; i < bmbs.size(); i++) {
        bmbs[i].draw();
    }

    ofSetColor(255);

    // draw the voronoi mesh
    swpr.vrn.draw(OF_MESH_FILL);
    if (doCanny) {
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        canny.draw(0, 0);
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    }

    gui.draw();

    // draw the raw points
    std::stringstream strm;
    strm << "fps -> " << ofGetFrameRate();
    ofDrawBitmapString(strm.str(), ofGetWidth() - 256, 32);
}
