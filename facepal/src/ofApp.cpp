#include "ofApp.h"

using namespace ofxCv;

void ofApp::setup() {
	ofSetVerticalSync(true);
	camWdth = 1920;
	camHght = 1080;
	wnWdth = 1280;
	wnHght = 720;
    setCam(&cam, "C920"); // try to find external webcam
	cam.initGrabber(camWdth, camHght);

	camWdth = cam.getWidth();
	camHght = cam.getHeight();
	lftd.setup(ofRectangle(0,0,camWdth/2,camHght));
	rghtd.setup(ofRectangle(camWdth/2,0,camWdth,camHght));
}

void ofApp::update() {
	cam.update();
	if(cam.isFrameNew()) {
		ofImage img;
		img.setFromPixels(cam.getPixelsRef());
		lftd.update(img);
		rghtd.update(img);
	}
}

void ofApp::draw() {
	ofSetColor(255);
	//cam.draw(0, 0);
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);

	ofSetupScreenOrtho(camWdth, camHght, -1000, 1000);
	ofTranslate(camWdth/2, camHght/2);
	ofScale(15,15,15);

	if(lftd.hasFace()) {
		lftd.draw();
	}

	//ofTranslate(840, 0);

	if(rghtd.hasFace()) {
		rghtd.draw();
	}
 }

void ofApp::keyPressed(int key) {
	if(key == 'r') {
		//tracker.reset();
	}
}
