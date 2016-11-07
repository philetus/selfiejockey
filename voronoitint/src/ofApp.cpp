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

    gui.add(samples.set("samples", 4000, 1, 10000));
    gui.add(noise.set("noise", 100, 0, 1000));
    gui.add(neg.set("neg", true));
    gui.add(doCanny.set("doCanny", false));
    gui.add(cannyParam1.set("cannyParam1", 300, 0, 1024));
    gui.add(cannyParam2.set("cannyParam2", 150, 0, 1024));

    tntclr.setHex(0x0000ff); // resin tint color
}

void ofApp::update() {
    cam.update();
     if(cam.isFrameNew()) {

        swpr.clear();

        live.setFromPixels(cam.getPixels());
        input.setFromColorImage(live);

        Canny(input, canny, cannyParam1 * 2, cannyParam2 * 2, 5);
        int notEdgeVal = 0;
        if (neg) {
            invert(canny);
            notEdgeVal = 255;
        }
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

        // set cells to tinted colors based on sampled brightness
        ofPixels pixels = cam.getPixels();
        for(std::size_t i = 0; i < swpr.clls.size(); i++) {
            ofColor c = ofColor(tntclr);
            glm::vec3 src = swpr.srcs[swpr.clls[i].srcdx];
            c.setBrightness(pixels.getColor((int)src.x, (int)src.y).getBrightness());
            swpr.clls[i].setColor(c);
        }

        swpr.mesh();
	}
}

void ofApp::draw() {

    // the bounds of all the points
    //ofSetColor(90);
    //ofNoFill();
    //ofDrawRectangle(bounds);
    
    //ofFill();
    //ofSetColor(255);

    // draw the voronoi mesh
    swpr.vrn.draw(OF_MESH_FILL);

    gui.draw();

    // draw the raw points
    std::stringstream strm;
    strm << "fps -> " << ofGetFrameRate();
    ofDrawBitmapString(strm.str(), ofGetWidth() - 256, 32);
}
