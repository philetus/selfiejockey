#pragma once

#include "ofMain.h"
#include "string"

// This openFrameworks example is designed to demonstrate how to access the
// webcam.
//
// For more information regarding this example take a look at the README.md.

class ofApp : public ofBaseApp{

    public:

        void setup();
        void update();
        void draw();

        void keyPressed(int key);
        void keyReleased(int key);
        void mouseMoved(int x, int y);
        void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        void mouseEntered(int x, int y);
        void mouseExited(int x, int y);
        void windowResized(int w, int h);
        void dragEvent(ofDragInfo dragInfo);
        void gotMessage(ofMessage msg);

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

        ofVideoGrabber vidGrabber;
        int camWidth;
        int camHeight;
};
