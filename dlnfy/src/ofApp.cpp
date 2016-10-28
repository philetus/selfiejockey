#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowShape(1920, 1080); //ウインドウのサイズ
 
    setCam(&cam, "C920"); // try to find external webcam
    cam.setDesiredFrameRate(30);
    cam.initGrabber(ofGetWidth(), ofGetHeight());

    grayImage.allocate(ofGetWidth(), ofGetHeight());
    edgeImage.allocate(ofGetWidth(), ofGetHeight());

    gui.setup();
    gui.add(particles.set("particles", 1000, 0, 20000));
    gui.add(power.set("power", 2, 0.5, 8.0));
}
 
//--------------------------------------------------------------
void ofApp::update() {
    float wdth = ofGetWidth();
    float hght = ofGetHeight();
    cam.update();
    if (cam.isFrameNew()) {

        delaunay.reset();
        mesh.clear();
 
        // sample colors
        ofPixels pxls = cam.getPixels();

        // generate random points weighted for brightness
        int pcnt = particles;
        while (pcnt > 0) {
            glm::vec3 p(ofRandom(0, wdth), ofRandom(0, hght), 0);
            float b = ofMap(
                pxls.getColor((int)p.x, (int)p.y).getBrightness(),
                0, 255, 1, 0);
            if (pow(b, power) > ofRandom(1)) {
                delaunay.addPoint(p);
                pcnt--;
            }
        }

        // triangulate points
        delaunay.triangulate();
         
        for (int i=0; i<delaunay.triangleMesh.getNumIndices()/3; i++) {
            int idx1 = delaunay.triangleMesh.getIndex(i*3);
            int idx2 = delaunay.triangleMesh.getIndex(i*3+1);
            int idx3 = delaunay.triangleMesh.getIndex(i*3+2);
             
            glm::vec3 v1 = delaunay.triangleMesh.getVertex(idx1);
            glm::vec3 v2 = delaunay.triangleMesh.getVertex(idx2);
            glm::vec3 v3 = delaunay.triangleMesh.getVertex(idx3);
             
            mesh.addVertex(v1);
            mesh.addColor(pxls.getColor((int)v1.x, (int)v1.y));
            mesh.addVertex(v2);
            mesh.addColor(pxls.getColor((int)v2.x, (int)v2.y));
            mesh.addVertex(v3);
            mesh.addColor(pxls.getColor((int)v3.x, (int)v3.y));
             
             
        }
    }
}
 
//--------------------------------------------------------------
void ofApp::draw(){
    ofFill();
    ofSetColor(255);
    mesh.draw(OF_MESH_WIREFRAME);
    gui.draw();

    std::stringstream strm;
    strm << "fps -> " << ofGetFrameRate();
    ofDrawBitmapString(strm.str(), ofGetWidth() - 256, 32);
}