#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    camWdth = 1280;
    camHght = 720;
    setCam(&cam, "C920"); // try to find external webcam
	cam.setup(camWdth, camHght);
    camWdth = cam.getWidth();
    camHght = cam.getHeight();
    ofLogNotice() << "cam width: " << camWdth << " cam height: " << camHght << endl;

    src = ofRectangle(0, 0, camWdth/2, camHght/2);
    trgt = ofRectangle(0, 0, 1920, 1080);

    dfy.setup(src, trgt);

    ofBackground(255);
    
    gui.setup();
    gui.add(obs.minArea);
    gui.add(obs.figErode);
    gui.add(obs.figDilate);
    gui.add(dfy.cannyParam1);
    gui.add(dfy.cannyParam2);
    gui.add(dfy.samples);
    gui.add(dfy.alpha);

    prt0 = "/dev/tty.usbmodem14711";
    domn0.addVertex(0, 0, 0);
    domn0.addVertex(356, 0, 0);
    domn0.addVertex(324, 360, 0);
    domn0.addVertex(0, 360, 0);
    domn0.close();
    fltr0.setup(src, trgt);
    pllr0.setup(0, prt0, 57600, fltr0, domn0);   
    pllrs.push_back(pllr0);

    prt1 = "/dev/tty.usbmodem14712";
    domn1.addVertex(356, 0, 0);
    domn1.addVertex(640, 0, 0);
    domn1.addVertex(640, 360, 0);
    domn1.addVertex(324, 360, 0);
    domn1.close();
    fltr1.setup(src, trgt);
    pllr1.setup(1, prt1, 57600, fltr1, domn1);   
    pllrs.push_back(pllr1);
}

void ofApp::update() {

	cam.update();
	if(cam.isFrameNew()) {
        scld = cam.getPixels();
        scld.mirror(false, true);
        scld.resize(src.width, src.height);
        obs.update(scld);

        if(obs.getBackgroundModel(bgmdl)) {
            const std::vector<ofPolyline> & fgrs = obs.getFigures();
            ofPixels bgpxls = bgmdl.getPixels();
            std::vector<ofPolyline> ghsts;
            for (std::size_t i = 0; i < pllrs.size(); i++) {
                std::vector<ofPolyline> gs = pllrs[i].update(bgpxls, fgrs);
                ghsts.insert(ghsts.end(), gs.begin(), gs.end());
            }
            dfy.update(bgmdl.getPixels(), ghsts);
        }
	}
}

void ofApp::draw() {

    // if figure observer has foreground mask draw it
    if(obs.getForegroundMask(fgmsk)) {
        ofSetHexColor(0xffff00);
        fgmsk.draw(camWdth, 0);
        ofSetHexColor(0xffffff);

        // draw grayscale background model & canny edges
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        dfy.inpt.draw(camWdth, 0);
        dfy.cnny.draw(camWdth, 0);
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    }

    // draw delaunayfied background model
    dfy.draw();

    // draw parameter sliders
    gui.draw();

    // draw framerate
    std::stringstream strm;
    strm << "fps -> " << ofGetFrameRate();
    ofDrawBitmapString(strm.str(), camWdth - 256, 32);
}
