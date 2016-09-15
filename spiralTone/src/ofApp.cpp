#include "ofApp.h"
#include "float.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(120);

	ofBackground(255,255,255);

    // try to grab at this size
    camWidth = 1024;
    camHeight = 768;
    
    vidGrabber.setVerbose(true);
    vidGrabber.setup(camWidth,camHeight);

    ofEnableAlphaBlending(); // necessary???

    smile.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
    vidGrabber.update();
    if(vidGrabber.isFrameNew()) {
        smile.update(vidGrabber);
        if(smile.hasFace) {
            ofLog() << smile.smilebright;
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

    // change halftone dot size based on cursors x location
    //float dotRadius = ofMap(mouseX, 0, ofGetWidth(), 3, 12, true);
    float numdots = ofMap(mouseX, 0, ofGetWidth(), 64, 4096, true);
    ofColor smileColor = ofColor::fromHsb(
        ofMap(mouseY, 0, ofGetHeight(), 0, 255, true), 255, smile.smilebright);

    // set a white fill color with the alpha generated above
    ofSetColor(255,255,255,31);

    // draw the raw video frame with the alpha value generated above
    vidGrabber.draw(0,0);	

    ofPixels & pixels = vidGrabber.getPixels();

    if (smile.hasFace) {
        ofSetColor(smileColor);
        drawSpiral(numdots, smileColor, pixels);
        //smile.draw(); // uncomment to draw face & smile boxes
    }
}

//--------------------------------------------------------------
void ofApp::drawSpiral(int numdots, ofColor smileColor, ofPixels & pixels){

    int cx = int(smile.faceX);
    int cy = int(smile.faceY);
    float radius = smile.faceRadius;
    float deviation = 0.625;

    float phi = ((sqrt(5) + 1) / 2) - 1;
    float goldenFraction = phi * 2 * PI;
    float area = radius * radius * PI;

    float meanArea = area / numdots;

    float minArea = meanArea * (1 - deviation);
    float maxArea = meanArea * (1 + deviation);

    float areaFilled = 0;

    float dotcf = 1.1; // dots dont really fill space

    int vidWidth = pixels.getWidth();
    int vidHeight = pixels.getHeight();
    int nChannels = pixels.getNumChannels();

    for (int i = 1; i < numdots; i++){
        float angle = i * goldenFraction;
        float ratio = i / float(numdots);
        float smArea = minArea + (ratio * (maxArea - minArea));
        float smRadius = sqrt(smArea / PI);

        areaFilled += smArea;

        float spiralRadius = sqrt(areaFilled / PI);
        float x = cx + cos(angle) * spiralRadius;
        float y = cy + sin(angle) * spiralRadius;

        unsigned char r = pixels[(int(y) * vidWidth + x)*nChannels];
        float val = 1 - ((float)r / 255.0f);

        ofDrawCircle(x, y, smRadius*val*dotcf);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    // in fullscreen mode, on a pc at least, the 
    // first time video settings the come up
    // they come up *under* the fullscreen window
    // use alt-tab to navigate to the settings
    // window. we are working on a fix for this...
    
    if (key == 's' || key == 'S'){
        vidGrabber.videoSettings();
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
