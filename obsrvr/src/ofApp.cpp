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
    gui.add(obs.minArea);
    gui.add(obs.maxArea);
    gui.add(obs.threshold);
    gui.add(obs.holes);
    gui.add(cannyParam1.set("cannyParam1", 400, 0, 1024));
    gui.add(cannyParam2.set("cannyParam2", 600, 0, 1024));
    gui.add(dfy.stride);
    gui.add(dfy.noise);
    gui.add(dfy.alpha);
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

            /* // erode & dilate mask before countour finding
            Mat msk = toCv(fgmsk);
            Mat kd = getStructuringElement(MORPH_ELLIPSE, cv::Size(6, 6), cv::Point(-1,-1));
            Mat ke = getStructuringElement(MORPH_RECT, cv::Size(16, 16), cv::Point(-1,-1));
            erode(msk, msk, kd, cv::Point(-1, -1), 1, 1, 1);
            dilate(msk, msk, ke, cv::Point(-1, -1), 2, 1, 1);
            fgmsk.update();
            */
        }

        if(obs.getBackgroundModel(bgmdl)) {
            live.setFromPixels(bgmdl.getPixels());
            input.setFromColorImage(live);
            Canny(input, canny, cannyParam1 * 2, cannyParam2 * 2, 5);
            dfy.update(canny.getPixels(), bgmdl.getPixels(), obs.getFigures());
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
