#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowShape(1920, 1080); //ウインドウのサイズ
 
    setCam(&cam, "C920"); // try to find external webcam
    cam.setDesiredFrameRate(30);
    cam.initGrabber(ofGetWidth(), ofGetHeight());

    grayImage.allocate(ofGetWidth(), ofGetHeight());
    edgeImage.allocate(ofGetWidth(), ofGetHeight());
}
 
//--------------------------------------------------------------
void ofApp::update() {
    float w = ofGetWidth();
    float h = ofGetHeight();
    cam.update();
    if (cam.isFrameNew()) {
        //リセット
        delaunay.reset();
        mesh.clear();
 
        /*Cannyの部分*********************************************************************/
        colorImage.setFromPixels(cam.getPixels());
        grayImage = colorImage;
        grayImage.blur();
        cvCanny(grayImage.getCvImage(), edgeImage.getCvImage(), 20, 100);　//Canny Filter
        edgeImage.flagImageChanged();
        edgeImage.dilate();
        ofPixels & edgePixels = edgeImage.getPixels();
         
        //輪郭線を間引いて頂点として扱う
        int up = 80;
        for (int i=0; i<w*h; i+=up) {
            if (edgePixels[i] == 0){ continue;}
            else{
                int coordY = floor(i/w);
                delaunay.addPoint(glm::vec3(i-w*coordY, coordY, 0));
            }
        }
        /********************************************************************************/
        //ランダムな点を増やす
        for (int i=0; i<200; i++) {
            delaunay.addPoint(ofPoint(ofRandom(0, ofGetWidth()),ofRandom(0, ofGetHeight())));
        }
        //四隅の点
        delaunay.addPoint(ofPoint(0,0));
        delaunay.addPoint(ofPoint(0,ofGetHeight()));
        delaunay.addPoint(ofPoint(ofGetWidth(),ofGetHeight()));
        delaunay.addPoint(ofPoint(ofGetWidth(),0));
        delaunay.triangulate();
         
        /*ポリゴンの中心の色を使う*/
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
 
//--------------------------------------------------------------
void ofApp::draw(){
    ofFill();
    ofSetColor(255);
    mesh.draw();
}