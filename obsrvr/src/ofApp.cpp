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

    dfy.setup(camWdth, camHght);
    ofBackground(255);
    live.allocate(camWdth / 2, camHght / 2);
    input.allocate(camWdth / 2, camHght / 2);
    canny.allocate(camWdth / 2, camHght / 2, OF_IMAGE_GRAYSCALE);
    
    gui.setup();
    gui.add(obs.minArea);
    gui.add(obs.figErode);
    gui.add(obs.figDilate);
    gui.add(cannyParam1.set("cannyParam1", 400, 0, 1024));
    gui.add(cannyParam2.set("cannyParam2", 600, 0, 1024));
    gui.add(dfy.stride);
    gui.add(dfy.alpha);

    pllr0.setup("/dev/tty.usbmodem14711", 57600);
}

void ofApp::update() {
    if(pllr0.available() > 0) {
        int b = 0;
        b = pllr0.readByte();
        ofLogNotice() << "from pillar 0 read: " << b << endl;
        if (b == 1) pllr0flg = true;
        if (b == 2) pllr0flg = false;
    }

	cam.update();
	if(cam.isFrameNew()) {
        scld = cam.getPixels();
        scld.mirror(false, true);
        scld.resize(camWdth/2, camHght/2);
        obs.update(scld);

        if(obs.getBackgroundModel(bgmdl)) {
            live.setFromPixels(bgmdl.getPixels());
            input.setFromColorImage(live);
            Canny(input, canny, cannyParam1 * 2, cannyParam2 * 2, 5);
            canny.update();

            dfy.update(canny.getPixels(), bgmdl.getPixels(), obs.getFigures(), pllr0flg);
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
        input.draw(camWdth, 0);
        canny.draw(camWdth, 0);
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
