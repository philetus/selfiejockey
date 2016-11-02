#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "ofxFgr.h"
#include "Delaunify.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	
	ofVideoGrabber cam;
	int camWdth, camHght;

	ofxFgr::Obsrvr obs;
	ofImage fgmsk, bgmdl;
    
    ofxCv::ContourFinder contourFinder;

    Delaunify dfy;
    ofxCvColorImage live;
    ofxCvGrayscaleImage input;
    ofImage canny;
    ofPixels scld;
    
    ofxPanel gui;
    ofParameter<int> cannyParam1, cannyParam2; 

    ofSerial pllr0;
    bool pllr0flg = false;

    void setCam(ofVideoGrabber *grabber, std::string camName) {
        vector<ofVideoDevice> devices = grabber->listDevices();
        int useCam = 0;
        for(int i = 0; i < devices.size(); i++){
            if(devices[i].bAvailable){
                ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - available ";
                if (devices[i].deviceName.find(camName) != std::string::npos) {
                    ofLogNotice() << "found " << camName << "!\n";
                    useCam = i;
                }
            }else{
                ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
            }
        }
        grabber->setDeviceID(useCam);
    }     
};
