#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "ofxFgr.h"
#include "Fltr.h"
#include "GhstFltr.h"
#include "DlnySktchFltr.h"
#include "CnnyDlnyFltr.h"
#include "VrnTntFltr.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	
	ofVideoGrabber cam;
	int camWdth, camHght;
    ofRectangle src, trgt;

	ofxFgr::Obsrvr obs;
	ofImage fgmsk, bgmdl, inpt, cnny;
    ofPixels scld;
    
    ofxPanel gui;
    ofParameter<int> cannyParam1, cannyParam2; 

    std::vector<bool> hallflgs, swtchflgs;
    int swtchdx;

    GhstFltr bgfltr;

    CnnyDlnyFltr fltr0, fltr4;
    DlnySktchFltr fltr1, fltr3;
    VrnTntFltr fltr2;
    std::vector<Fltr *> fltrs;

    ofSerial srl0, srl1, srl2, srl3, srl4;
    std::vector<ofSerial *> srls;

    void checkSerial();

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
