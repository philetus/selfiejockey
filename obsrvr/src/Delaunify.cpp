#include "Delaunify.h"

using namespace ofxCv;
using namespace cv;

void Delaunify::setup(int w, int h) {
    wdth = w;
    hght = h;
    stride.set("stride", 17, 11, 500);
    noise.set("noise", 100, 0, 1000);
    edgeThresh.set("edgeThresh", 30, 0, 255);
    alpha.set("alpha", 63, 0, 255);
}

void Delaunify::update(ofPixels & edgePx, ofPixels & colorPx, const std::vector<ofPolyline> & fgrs) {

    delaunay.reset();
    mesh.clear();

    int up = stride;
    for (int i=0; i<wdth*hght; i+=up) {
        if (edgePx[i] < edgeThresh){ continue;}
        else{
            int coordY = floor(i/wdth);
            delaunay.addPoint(glm::vec3(i-wdth*coordY, coordY, 0));
        }
    }

    for (int i=0; i<noise; i++) {
        delaunay.addPoint(glm::vec3(ofRandom(0, wdth), ofRandom(0, hght), 0));
    }

    // add figures
    for(int i = 0; i < fgrs.size(); i++) {
        for(int j = 0; j < fgrs[i].size(); j++) delaunay.addPoint(fgrs[i][j]);
    }

    // add frame & triangulate
    delaunay.addPoint(ofPoint(0,0));
    delaunay.addPoint(ofPoint(0,hght));
    delaunay.addPoint(ofPoint(wdth,hght));
    delaunay.addPoint(ofPoint(wdth,0));
    delaunay.triangulate();

    // sample colors
    for (int i=0; i<delaunay.triangleMesh.getNumIndices()/3; i++) {
        int idx1 = delaunay.triangleMesh.getIndex(i*3);
        int idx2 = delaunay.triangleMesh.getIndex(i*3+1);
        int idx3 = delaunay.triangleMesh.getIndex(i*3+2);
         
        glm::vec3 v1 = delaunay.triangleMesh.getVertex(idx1);
        glm::vec3 v2 = delaunay.triangleMesh.getVertex(idx2);
        glm::vec3 v3 = delaunay.triangleMesh.getVertex(idx3);
         
        glm::vec3 gp = (v1+v2+v3)/3.0;
         
        ofColor color = colorPx.getColor((int)gp.x, (int)gp.y);
        color.a = alpha;

        // if center is in a figure set alpha to 255
        for(int i = 0; i < fgrs.size(); i++) {
            if(fgrs[i].inside(gp)) color.a = 255;
        }

        mesh.addVertex(v1);
        mesh.addVertex(v2);
        mesh.addVertex(v3);
         
        mesh.addColor(color);
        mesh.addColor(color);
        mesh.addColor(color);
    }
}

void Delaunify::draw() {
    mesh.draw();    
}