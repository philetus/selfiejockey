#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    camWdth = 720;
    camHght = 480;
    camPd = 20;
    wnWdth = 1470;
    wnHght = 1480;

    setCam(&cam, "C920"); // try to find external webcam
    cam.initGrabber(camWdth, camHght);
    camWdth = cam.getWidth();
    camHght = cam.getHeight();

    colorImg.allocate(camWdth, camHght);
    grayImage.allocate(camWdth, camHght);
    grayBg.allocate(camWdth, camHght);
    grayDiff.allocate(camWdth, camHght);

    bLearnBakground = true;
    threshold = 80;
}

//--------------------------------------------------------------
void ofApp::update(){
    ofBackground(100,100,100);

    bool bNewFrame = false;

   cam.update();
   bNewFrame = cam.isFrameNew();

    if (bNewFrame){

        colorImg.setFromPixels(cam.getPixels());

        grayImage = colorImg;
        if (bLearnBakground == true){
            grayBg = grayImage;     // the = sign copys the pixels from grayImage into grayBg (operator overloading)
            bLearnBakground = false;
        }

        // take the abs value of the difference between background and incoming and then threshold:
        grayDiff.absDiff(grayBg, grayImage);
        grayDiff.threshold(threshold);

        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        contourFinder.findContours(grayDiff, 20, (340*240)/3, 10, true);    // find holes
    }

}

//--------------------------------------------------------------
void ofApp::draw(){

    int x0 = camPd;
    int x1 = (2 * camPd) + camWdth;
    int y0 = camPd;
    int y1 = (2 * camPd) + camHght;
    int y2 = (3 * camPd) + (2 * camHght);

    // draw the incoming, the grayscale, the bg and the thresholded difference
    ofSetHexColor(0xffffff);
    colorImg.draw(x0,y0);
    grayImage.draw(x1,y0);
    grayBg.draw(x0,y1);
    grayDiff.draw(x1,y1);

    // then draw the contours:

    ofFill();
    ofSetHexColor(0x333333);
    ofDrawRectangle(x1,y2,camWdth,camHght);
    ofSetHexColor(0xffffff);

    // we could draw the whole contour finder
    //contourFinder.draw(360,540);

    // or, instead we can draw each blob individually from the blobs vector,
    // this is how to get access to them:
    for (int i = 0; i < contourFinder.nBlobs; i++){
        contourFinder.blobs[i].draw(x1,y2);
        
        // draw over the centroid if the blob is a hole
        ofSetColor(255);
        if(contourFinder.blobs[i].hole){
            ofDrawBitmapString("hole",
                contourFinder.blobs[i].boundingRect.getCenter().x + x1,
                contourFinder.blobs[i].boundingRect.getCenter().y + y2);
        }
    }

    // finally, a report:
    ofSetHexColor(0xffffff);
    stringstream reportStr;
    reportStr << "bg subtraction and blob detection" << endl
              << "press ' ' to capture bg" << endl
              << "threshold " << threshold << " (press: +/-)" << endl
              << "num blobs found " << contourFinder.nBlobs << ", fps: " << ofGetFrameRate();
    ofDrawBitmapString(reportStr.str(), x0, y2 + 60);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    switch (key){
        case ' ':
            bLearnBakground = true;
            break;
        case '+':
            threshold ++;
            if (threshold > 255) threshold = 255;
            break;
        case '-':
            threshold --;
            if (threshold < 0) threshold = 0;
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
