#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "facedmn.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
    void setCam(ofVideoGrabber *grabber, std::string camName) {

        //get back a list of devices.
        vector<ofVideoDevice> devices = grabber->listDevices();

        int useCam = 0;

        for(int i = 0; i < devices.size(); i++){
            if(devices[i].bAvailable){

                //log the device
                ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - available ";

                if (devices[i].deviceName.find(camName) != std::string::npos) {
                    ofLogNotice() << "found " << camName << "!\n";
                    useCam = i;
                }

            }else{
                //log the device and note it as unavailable
                ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
            }
        }

        grabber->setDeviceID(useCam);
    }     

    int camWdth, camHght, wnWdth, wnHght;
	ofVideoGrabber cam;
	facedmn lftd, rghtd;
		
	ofEasyCam easyCam;
};
