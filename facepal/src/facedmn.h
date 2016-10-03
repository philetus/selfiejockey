#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxFaceTracker.h"
#include "ofxDelaunay.h"

class facedmn {
public:

	ofRectangle subd; // subdomain rect for this demon to watch
	ofImage subimg;
	ofxFaceTracker tracker;
    ofxDelaunay delaunay;
    ofVboMesh dmesh;


	void setup(ofRectangle sbd) {
		tracker.setup();
		subd = ofRectangle(sbd);
		subimg.allocate(subd.width, subd.height, OF_IMAGE_COLOR);
	}

	void update(ofImage img) {
		subimg.cropFrom(img, subd.x, subd.y, subd.width, subd.height);
		tracker.update(ofxCv::toCv(subimg));

		if (tracker.getFound()) {

			//ofLogNotice() << "found face!/n";

			delaunay.reset();
			dmesh.clear();

			vector<glm::vec3> imgPnts(tracker.size());

			for (int i = 0; i < tracker.size(); i++) {
				delaunay.addPoint(tracker.getObjectPoint(i));
				glm::vec2 ip = tracker.getImagePoint(i);
				imgPnts[i] = glm::vec3(ip.x, ip.y, 0);
			}
			delaunay.triangulate();

	        for (int i=0; i<delaunay.triangleMesh.getNumIndices()/3; i++) {
	            int idx1 = delaunay.triangleMesh.getIndex(i*3);
	            int idx2 = delaunay.triangleMesh.getIndex(i*3+1);
	            int idx3 = delaunay.triangleMesh.getIndex(i*3+2);
	             
	            glm::vec3 v1 = imgPnts[idx1];
	            glm::vec3 v2 = imgPnts[idx2];
	            glm::vec3 v3 = imgPnts[idx3];
	             
	            glm::vec3 gp = (v1+v2+v3)/3.0;
	             
	            ofColor color = subimg.getColor((int)gp.x, (int)gp.y);
	             
	            dmesh.addVertex(delaunay.triangleMesh.getVertex(idx1));
	            dmesh.addVertex(delaunay.triangleMesh.getVertex(idx2));
	            dmesh.addVertex(delaunay.triangleMesh.getVertex(idx3));
	             
	            dmesh.addColor(color);
	            dmesh.addColor(color);
	            dmesh.addColor(color);
	        }

		}
	}

	void draw() {
		//subimg.draw(0, 0);

		//if (tracker.getFound()) {
			//tracker.draw();

			dmesh.draw();
		//}
	}

	bool hasFace() {
		return tracker.getFound();
	}

	vector<glm::vec3> getPoints() const {
		return tracker.getObjectPoints();
	}

protected:


};