#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    camWdth = 1280;
    camHght = 720;
    setCam(&cam, "C920"); // try to find external webcam
	cam.setup(camWdth, camHght);

    dfy.setup(camWdth, camHght);
    ofBackground(255);
    live.allocate(camWdth, camHght);
    input.allocate(camWdth, camHght);
    canny.allocate(camWdth, camHght, OF_IMAGE_GRAYSCALE);
    
    gui.setup();
    gui.add(minArea.set("Min area", 100, 1, 500));
    gui.add(maxArea.set("Max area", 1500, 1, 2000));
    gui.add(threshold.set("Threshold", 16, 0, 255));
    gui.add(holes.set("Holes", true));
    gui.add(cannyParam1.set("cannyParam1", 400, 0, 1024));
    gui.add(cannyParam2.set("cannyParam2", 600, 0, 1024));
    /*
    gui.add(resetBackground.set("Reset Background", false));
    gui.add(learningTime.set("Learning Time", 30, 0, 30));
    gui.add(thresholdValue.set("Threshold Value", 10, 0, 255));
    */
}

void ofApp::update() {
	cam.update();
    /*
    if(resetBackground) {
        background.reset();
        resetBackground = false;
    }
    */
	if(cam.isFrameNew()) {

        obs.update(cam.getPixels());
        if(obs.getForegroundMask(fgmsk)) {
            contourFinder.setMinAreaRadius(minArea);
            contourFinder.setMaxAreaRadius(maxArea);
            contourFinder.setThreshold(threshold);
            contourFinder.findContours(fgmsk);
            contourFinder.setFindHoles(holes);
        }

        if(obs.getBackgroundModel(bgmdl)) {
            live.setFromPixels(bgmdl.getPixels());
            input.setFromColorImage(live);
            Canny(input, canny, cannyParam1 * 2, cannyParam2 * 2, 5);
            dfy.update(canny.getPixels(), bgmdl.getPixels());
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
    dfy.draw();
    if(obs.getForegroundMask(fgmsk)) {
        fgmsk.draw(camWdth, 0);
    }

    contourFinder.draw();

    //gui.draw();


    //obs.draw();
    
    /*
	cam.draw(0, 0);
    if(thresholded.isAllocated()) {
        thresholded.draw(640, 0);
    }
    */
}
