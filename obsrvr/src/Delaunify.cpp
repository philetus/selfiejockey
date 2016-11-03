#include "Delaunify.h"

using namespace ofxCv;
using namespace cv;

void Delaunify::setup(int w, int h) {
    wdth = w;
    hght = h;
    stride.set("particles", 4000, 1, 10000);
    noise.set("noise", 100, 0, 1000);
    edgeThresh.set("edgeThresh", 30, 0, 255);
    power.set("power", 8, 0, 10);
    alpha.set("alpha", 127, 0, 255);
}

void Delaunify::update(ofPixels & edgePx, ofPixels & colorPx, const std::vector<ofPolyline> & fgrs, bool flg) {

    delaunay.reset();
    mesh.clear();

    int pcnt = stride;
    while (pcnt > 0) {
        glm::vec3 p(ofRandom(0, wdth), ofRandom(0, hght), 0);
        int b = edgePx.getColor((int)(p.x/2), (int)(p.y/2)).getBrightness();
        if (b > edgeThresh) {
            delaunay.addPoint(p);
            pcnt--;
        }
    }

    // add figures
    for(int i = 0; i < fgrs.size(); i++) {
        for(int j = 0; j < fgrs[i].size(); j++) {
            delaunay.addPoint(glm::vec3(fgrs[i][j].x * 2, fgrs[i][j].y * 2, 0));
        }
    }

    // add frame & triangulate
    delaunay.addPoint(glm::vec3(0, 0, 0));
    delaunay.addPoint(glm::vec3(0, hght, 0));
    delaunay.addPoint(glm::vec3(wdth, hght, 0));
    delaunay.addPoint(glm::vec3(wdth, hght, 0));
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
         
        ofColor color = colorPx.getColor((int)(gp.x/2), (int)(gp.y/2));
        color.a = alpha;

        // if center is in a figure set alpha to 255
        for(int i = 0; i < fgrs.size(); i++) {
            if(fgrs[i].inside(glm::vec3(gp.x/2, gp.y/2, 0))) {
                if(flg) {
                    color.b = 255;
                }
                color.a = 255;
            }
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