#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
 	
 	camWdth = 1280;
 	camHght = 720;
    setCam(&cam, "C920"); // try to find external webcam
    cam.setDesiredFrameRate(30);
    cam.initGrabber(camWdth, camHght);

    ofBackground(255);
    live.allocate(camWdth, camHght);
	input.allocate(camWdth, camHght);
	output.allocate(camWdth, camHght, OF_IMAGE_GRAYSCALE);
	canny.allocate(camWdth, camHght, OF_IMAGE_GRAYSCALE);
	
	
	gui.setup();
    gui.add(doFDoG.set("doFDoG", true));
    /*
    gui.add(halfw.set("halfw", 4, 1, 8));
    gui.add(smoothPasses.set("smoothPasses", 2, 1, 4));
    gui.add(sigma1.set("sigma1", 0.68, 0.01, 2.0));
    gui.add(sigma2.set("sigma2", 6.0, 0.01, 10.0));
    gui.add(tau.set("tau", 0.974, 0.8, 1.0));
    gui.add(black.set("black", -8, -255, 255));
    gui.add(doThresh.set("doThresh", true));
    gui.add(thresh.set("thresh", 150, 0, 255));
    gui.add(doThin.set("doThin", true));
    */
    gui.add(doCanny.set("doCanny", true));
    gui.add(cannyParam1.set("cannyParam1", 400, 0, 1024));
    gui.add(cannyParam2.set("cannyParam2", 600, 0, 1024));
}

void ofApp::update(){
    cam.update();
    if (cam.isFrameNew()) {

    	live.setFromPixels(cam.getPixels());
    	input.setFromColorImage(live);

		if(doFDoG) {
			/*
			CLD(input, output, halfw, smoothPasses, sigma1, sigma2, tau, black);
			invert(output);
			if(doThresh) {
				threshold(output, thresh);
			}
			if(doThin) {
				thin(output);
			}
			output.update();
			*/
			if(doCanny) {
				Canny(input, canny, cannyParam1 * 2, cannyParam2 * 2, 5);
				canny.update();
			}
		}

	}
}

void ofApp::draw(){
    
	//ofTranslate(300, 0);
	//ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	//input.draw(0, 0);
	//ofEnableBlendMode(OF_BLENDMODE_ADD);
	//output.draw(0, 0);
	
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	input.draw(0, 0);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	canny.draw(0, 0);

	gui.draw();

	
/*	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofDrawBitmapStringHighlight("Coherent line drawing", 10, 20);
	ofDrawBitmapStringHighlight("Canny edge detection", 10, 256 + 20);
	*/
}
