#include "ofApp.h"
#include "float.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(255,255,255);
	//ofSetVerticalSync(true);
	//frameByframe = false;

    // try to grab at this size
    camWidth = 1024;
    camHeight = 768;
    
    vidGrabber.setVerbose(true);
    vidGrabber.setup(camWidth,camHeight);

    ofEnableAlphaBlending(); // necessary???
}

//--------------------------------------------------------------
void ofApp::update(){
    vidGrabber.update();
}

//--------------------------------------------------------------
void ofApp::draw(){

    // change halftone dot size based on cursors x location
    //float dotRadius = ofMap(mouseX, 0, ofGetWidth(), 3, 12, true);
    float numdots = ofMap(mouseX, 0, ofGetWidth(), 64, 1024, true);

    // set a white fill color with the alpha generated above
    ofSetColor(255,255,255,63);

    // draw the raw video frame with the alpha value generated above
    vidGrabber.draw(16,16);	

    ofSetHexColor(0x000000);
    ofPixels & pixels = vidGrabber.getPixels();

    int vidWidth = pixels.getWidth();
    int vidHeight = pixels.getHeight();
    int nChannels = pixels.getNumChannels();
    
    // let's move through the "RGB(A)" char array
    // using the red pixel to control the size of a circle.
   //  for (int i = dotRadius; i < vidWidth; i+=(dotRadius*2)){
   //      for (int j = dotRadius; j < vidHeight; j+=(dotRadius*2)){
   //          unsigned char r = pixels[(j * vidWidth + i)*nChannels];
   //          float val = 1 - ((float)r / 255.0f);
			// ofDrawCircle(16+i,16+j,dotRadius*val*2);
   //      }
   //  }

    drawSpiral((camWidth/2)+16, (camHeight/2)+16, 300, numdots, 0.625);

}

//--------------------------------------------------------------
void ofApp::drawSpiral(int cx, int cy, float radius, 
                       int numdots, float deviation){

    float phi = ((sqrt(5) + 1) / 2) - 1;
    float goldenFraction = phi * 2 * PI;
    float area = radius * radius * PI;

    float meanArea = area / numdots;

    float minArea = meanArea * (1 - deviation);
    float maxArea = meanArea * (1 + deviation);

    float areaFilled = 0;

    float dotcf = 0.87; // dots dont really fill space

    for (int i = 1; i < numdots; i++){
        float angle = i * goldenFraction;
        float ratio = i / float(numdots);
        float smArea = minArea + (ratio * (maxArea - minArea));
        float smRadius = sqrt(smArea / PI);

        areaFilled += smArea;

        float spiralRadius = sqrt(areaFilled / PI);
        float x = cx + cos(angle) * spiralRadius;
        float y = cy + sin(angle) * spiralRadius;

        ofDrawCircle(x, y, smRadius*dotcf);
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
