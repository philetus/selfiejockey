#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "float.h"

// single-person smile strength detector
// not robust against rotation
class SmileDetector {
protected:
    ofRectangle roi;
    
public:
    ofxCv::ObjectFinder faceFinder, smileFinder;
    bool hasFace;
    float faceX, faceY, faceRadius;
    float smileness;
    int smilebright;
    
    void setup() {
        hasFace = false;
        faceX = 0;
        faceY = 0;
        faceRadius = 0;
        smileness = 0;

        faceFinder.setup("haarcascade_frontalface_default.xml");
        faceFinder.setPreset(ofxCv::ObjectFinder::Accurate);
        faceFinder.setFindBiggestObject(true);
        smileFinder.setup("smiled_05.xml");
        smileFinder.setPreset(ofxCv::ObjectFinder::Sensitive);
        smileFinder.setMinNeighbors(0);
    }
    template <class T>
    void update(T& img) {
        update(ofxCv::toCv(img));
    }
    void update(const cv::Mat& mat) {
        faceFinder.update(mat);
        if(faceFinder.size()) {
            hasFace = true;
            roi = faceFinder.getObject(0);
            float lowerRatio = .35;
            roi.y += roi.height * (1 - lowerRatio);
            roi.height *= lowerRatio;
            cv::Mat faceMat(mat, ofxCv::toCv(roi));
            smileFinder.update(faceMat);

            ofRectangle fRect = faceFinder.getObject(0);
            glm::vec3 fCntr = fRect.getCenter();
            faceX = fCntr.x;
            faceY = fCntr.y;
            faceRadius = fRect.getWidth();

            smileness = smileFinder.size();
            smilebright = int(ofMap(log(smileness), 0, 7, 0, 255, true));
            
        } else {
            hasFace = false;
        }
    }
    void draw() const {
        faceFinder.draw();
        if(faceFinder.size()) {
            ofPushMatrix();
            ofTranslate(roi.position);
            smileFinder.draw();
            ofPopMatrix();
        }
    }
    bool getFaceFound() const {
        return faceFinder.size();
    }
    ofRectangle getFace() const {
        return faceFinder.getObject(0);
    }
    int getSmileAmount() const {
        if(faceFinder.size()) {
            return smileFinder.size();
        } else {
            return 0;
        }
    }
};

class ofApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();
		void drawSpiral(int numdots, ofColor smileColor, ofPixels & pixels);
		
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

		ofVideoGrabber vidGrabber;
		SmileDetector smile;

		int camWidth;
		int camHeight;

		//ofVideoPlayer 		fingerMovie;
		//bool                frameByframe;
};

