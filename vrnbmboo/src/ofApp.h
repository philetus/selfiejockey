#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxSwpr.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();

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

    int camWdth, camHght;
    ofVideoGrabber cam;
    ofxCvColorImage live;
    ofxCvGrayscaleImage input;
    ofImage output, canny;

    ofParameter<float> sigma1, sigma2, tau, shrink;
    ofParameter<int> samples, noise, cannyParam1, cannyParam2;
    ofParameter<bool> doFDoG, doThresh, doThin, doCanny, neg;
    ofxPanel gui;

    ofColor tntclr;

    static glm::vec3 randInRect(float x, float y, float w, float h) {
        float rx = ofRandom(x, x + w);
        float ry = ofRandom(y, y + h);
        return glm::vec3(rx, ry, 0);
    }
    static glm::vec3 randInRect(const ofRectangle &r) {
        return randInRect(r.x, r.y, r.width, r.height);
    }
    
    ofRectangle bounds;
    ofxSwpr::Swpr swpr;
    std::vector<ofPolyline> bmbs;
};
