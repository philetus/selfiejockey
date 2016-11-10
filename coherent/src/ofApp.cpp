#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    
    camWdth = 1920;
    camHght = 1080;
    setCam(&cam, "C920"); // try to find external webcam
    cam.initGrabber(camWdth, camHght);

    ofBackground(255);
    input.allocate(camWdth, camHght, OF_IMAGE_GRAYSCALE);
    canny.allocate(camWdth, camHght, OF_IMAGE_GRAYSCALE);
    
    gui.setup();
    gui.add(samples.set("samples", 4000, 1, 10000));
    gui.add(neg.set("neg", true));
    gui.add(doCanny.set("doCanny", false));
    gui.add(cannyParam1.set("cannyParam1", 300, 0, 1024));
    gui.add(cannyParam2.set("cannyParam2", 150, 0, 1024));
}

void ofApp::update(){
    cam.update();
    if (cam.isFrameNew()) {

        delaunay.reset();
        mesh.clear();

        // get image from camera and mirror it
        ofPixels pixels = cam.getPixels();
        pixels.mirror(false,true);

        // convert image to grayscale and run canny edge filter
        ofxCv::copyGray(pixels, input);
        Canny(input, canny, cannyParam1 * 2, cannyParam2 * 2, 5);
        int notEdgeVal = 0;
        if (neg) {
            invert(canny);
            notEdgeVal = 255;
        }
        canny.update();
        ofPixels edgePixels = canny.getPixels();

        // monte carlo sample for points on canny edges
        int pcnt = samples;
        while (pcnt > 0) {
            glm::vec3 p(ofRandom(0, camWdth), ofRandom(0, camHght), 0);
            int b = edgePixels[((int)p.y * camWdth) + (int)p.x];
            if (b != notEdgeVal) {
                delaunay.addPoint(p);
                pcnt--;
            }
        }

        // add frame & triangulate
        delaunay.addPoint(ofPoint(0,0));
        delaunay.addPoint(ofPoint(0,camHght));
        delaunay.addPoint(ofPoint(camWdth,camHght));
        delaunay.addPoint(ofPoint(camWdth,0));
        delaunay.triangulate();

        // sample colors from unfiltered image pixels
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
    
    if(doCanny && neg){
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        canny.draw(0, 0);
        ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
        mesh.draw();
    } else if (neg) {
        ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
        ofFill();
        ofSetColor(255);
        mesh.draw();
    } else if (doCanny) {
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        mesh.draw();
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        canny.draw(0, 0);
    } else {
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        mesh.draw();
    }
    gui.draw();

    std::stringstream strm;
    strm << "fps -> " << ofGetFrameRate();
    ofDrawBitmapString(strm.str(), ofGetWidth() - 256, 32);
}
