#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"

class ofApp : public ofBaseApp{

    public:
        void setup();
        void update();
        void draw();
        
        void keyPressed(int key);
        void keyReleased(int key);
        void mouseMoved(int x, int y );
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


        int camWdth, camHght, camPd, wnWdth, wnHght;
        ofVideoGrabber          cam;
        ofxCvColorImage         colorImg;

        ofxCvGrayscaleImage     grayImage;
        ofxCvGrayscaleImage     grayBg;
        ofxCvGrayscaleImage     grayDiff;

        ofxCvContourFinder  contourFinder;

        int                 threshold;
        bool                bLearnBakground;


};

