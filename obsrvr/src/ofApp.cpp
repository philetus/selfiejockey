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
    //gui.add(obs.maxArea);
    //gui.add(obs.threshold);
    gui.add(obs.figErode);
    gui.add(obs.figDilate);
    //gui.add(obs.holes);
    gui.add(cannyParam1.set("cannyParam1", 400, 0, 1024));
    gui.add(cannyParam2.set("cannyParam2", 600, 0, 1024));
    gui.add(dfy.stride);
    //gui.add(dfy.power);
    //gui.add(dfy.noise);
    gui.add(dfy.alpha);

    pllr0.setup("/dev/tty.usbmodem1471", 57600);
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
    /*
    if(resetBackground) {
        background.reset();
        resetBackground = false;
    }
    */
	if(cam.isFrameNew()) {
        scld = cam.getPixels();
        scld.mirror(false, true);
        scld.resize(camWdth/2, camHght/2);
        obs.update(scld);

        if(obs.getBackgroundModel(bgmdl)) {
            live.setFromPixels(bgmdl.getPixels());
            input.setFromColorImage(live);
            Canny(input, canny, cannyParam1 * 2, cannyParam2 * 2, 5);
            dfy.update(canny.getPixels(), bgmdl.getPixels(), obs.getFigures(), pllr0flg);
        }

        /*
        background.setLearningTime(learningTime);
        background.setThresholdValue(thresholdValue);
		background.update(cam, thresholded);
		thresholded.update();
        */
	}
}

void ofApp::draw() {
    /*
    if(obs.getBackgroundModel(bgmdl)) {
        bgmdl.draw(0, 0);
    }
    */
    if(obs.getForegroundMask(fgmsk)) {
        fgmsk.draw(camWdth, 0);
    }
    //bgmdl.draw(camWdth, 0);
    //canny.draw(camWdth, 0);
    //ofPushMatrix();
    //ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
    dfy.draw();
    //ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    //ofPopMatrix();

    /*
    //contourFinder.draw();
    for (int i = 0; i < contourFinder.size(); i++) {
        //
        //std::vector<cv::Point> contour = contourFinder.getContour(i);
        //std::vector<cv::Point> simpleContour;
        //approxPolyDP(contour, simpleContour, 8, true);
        //ofPolyline p = toOf(simpleContour);
        //

        ofPolyline p = contourFinder.getPolyline(i);
        p.simplify(0.5);
        p = p.getSmoothed(3, 0.5);
        p.draw();
    }
    */

    gui.draw();

    //obs.draw();
    
    /*
	cam.draw(0, 0);
    if(thresholded.isAllocated()) {
        thresholded.draw(640, 0);
    }
    */
}
