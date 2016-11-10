#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "ofxFgr.h"
#include "Delaunify.h"
#include "Pllr.h"
#include "Fltr.h"
#include "CannyDlnyFltr.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	
	ofVideoGrabber cam;
	int camWdth, camHght;
    ofRectangle src, trgt;

	ofxFgr::Obsrvr obs;
	ofImage fgmsk, bgmdl;
    ofPixels scld;
    
    ofxCv::ContourFinder contourFinder;

    Delaunify dfy;
    
    ofxPanel gui;
    ofParameter<int> cannyParam1, cannyParam2; 

    ofPolyline domn0, domn1;
    CannyDlnyFltr fltr0, fltr1;
    string prt0, prt1;
    Pllr pllr0, pllr1;
    std::vector<Pllr> pllrs;

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
