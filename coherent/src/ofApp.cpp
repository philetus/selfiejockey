#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
 	
 	//camWdth = 1280;
 	//camHght = 720;
 	camWdth = 1920;
 	camHght = 1080;
    setCam(&cam, "C920"); // try to find external webcam
    cam.setDesiredFrameRate(30);
    cam.initGrabber(camWdth, camHght);

    ofBackground(255);
    live.allocate(camWdth, camHght);
	input.allocate(camWdth, camHght);
	output.allocate(camWdth, camHght, OF_IMAGE_GRAYSCALE);
	canny.allocate(camWdth, camHght, OF_IMAGE_GRAYSCALE);
	
	
	gui.setup();
    //gui.add(doFDoG.set("doFDoG", true));
    /*
    gui.add(halfw.set("halfw", 4, 1, 8));
    gui.add(smoothPasses.set("smoothPasses", 2, 1, 4));
    gui.add(sigma1.set("sigma1", 0.68, 0.01, 2.0));
    gui.add(sigma2.set("sigma2", 6.0, 0.01, 10.0));
    gui.add(tau.set("tau", 0.974, 0.8, 1.0));
    gui.add(black.set("black", -8, -255, 255));
    gui.add(doThresh.set("doThresh", true));
     gui.add(doThin.set("doThin", true));
    */
	gui.add(stride.set("stride", 17, 11, 500));
	gui.add(noise.set("noise", 100, 0, 1000));
    gui.add(doCanny.set("doCanny", true));
    gui.add(cannyParam1.set("cannyParam1", 400, 0, 1024));
    gui.add(cannyParam2.set("cannyParam2", 600, 0, 1024));
}

void ofApp::update(){
    cam.update();
    if (cam.isFrameNew()) {

    	delaunay.reset();
        mesh.clear();

    	live.setFromPixels(cam.getPixels());
    	input.setFromColorImage(live);

		Canny(input, canny, cannyParam1 * 2, cannyParam2 * 2, 5);
		invert(canny);
		canny.update();

        ofPixels & edgePixels = canny.getPixels();

        float w = camWdth;
        float h = camHght;
         
        int up = stride;
        for (int i=0; i<w*h; i+=up) {
            if (edgePixels[i] >= 250){ continue;}
            else{
                int coordY = floor(i/w);
                delaunay.addPoint(glm::vec3(i-w*coordY, coordY, 0));
            }
        }

        for (int i=0; i<noise; i++) {
            delaunay.addPoint(ofPoint(ofRandom(0, w), ofRandom(0, h)));
        }

		// add frame & triangulate
        delaunay.addPoint(ofPoint(0,0));
        delaunay.addPoint(ofPoint(0,camHght));
        delaunay.addPoint(ofPoint(camWdth,camHght));
        delaunay.addPoint(ofPoint(camWdth,0));
        delaunay.triangulate();

		// sample colors
		ofPixels pixels = cam.getPixelsRef();
        for (int i=0; i<delaunay.triangleMesh.getNumIndices()/3; i++) {
            int idx1 = delaunay.triangleMesh.getIndex(i*3);
            int idx2 = delaunay.triangleMesh.getIndex(i*3+1);
            int idx3 = delaunay.triangleMesh.getIndex(i*3+2);
             
            glm::vec3 v1 = delaunay.triangleMesh.getVertex(idx1);
            glm::vec3 v2 = delaunay.triangleMesh.getVertex(idx2);
            glm::vec3 v3 = delaunay.triangleMesh.getVertex(idx3);
             
            glm::vec3 gp = (v1+v2+v3)/3.0;
             
            ofColor color = pixels.getColor((int)gp.x, (int)gp.y);
             
            mesh.addVertex(v1);
            mesh.addVertex(v2);
            mesh.addVertex(v3);
             
            mesh.addColor(color);
            mesh.addColor(color);
            mesh.addColor(color);
        }
	}
}

void ofApp::draw(){
    
	//ofTranslate(300, 0);
	//ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	//input.draw(0, 0);
	//ofEnableBlendMode(OF_BLENDMODE_ADD);
	//output.draw(0, 0);
	
	//input.draw(0, 0);
	if(doCanny){
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		canny.draw(0, 0);
		ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
	} else {
		ofFill();
    	ofSetColor(255);
	}
	mesh.draw();
	gui.draw();

	
/*	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofDrawBitmapStringHighlight("Coherent line drawing", 10, 20);
	ofDrawBitmapStringHighlight("Canny edge detection", 10, 256 + 20);
	*/
}
