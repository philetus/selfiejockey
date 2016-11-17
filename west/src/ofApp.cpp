#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    camWdth = 1280;
    camHght = 720;
    setCam(&cam, "C920"); // try to find external webcam
	cam.setup(camWdth, camHght);
    camWdth = cam.getWidth();
    camHght = cam.getHeight();
    ofLogNotice() << "cam width: " << camWdth << " cam height: " << camHght << endl;

    src = ofRectangle(0, 0, camWdth/2, camHght/2);
    trgt = ofRectangle(0, 0, ofGetWidth(), ofGetHeight());

    bgfltr.setup(src, trgt);

    srl0.setup("/dev/tty.usbmodem14711", 9600);
    srls.push_back(&srl0);
    hallflgs.push_back(false);
    swtchflgs.push_back(false);
    fltr0.setup(src, trgt);
    fltrs.push_back(&fltr0);

    srl1.setup("/dev/tty.usbmodem14721", 9600);
    srls.push_back(&srl1);
    hallflgs.push_back(false);
    swtchflgs.push_back(false);
    fltr1.setup(src, trgt);
    fltrs.push_back(&fltr1);

    srl2.setup("/dev/tty.usbmodem14731", 9600);
    srls.push_back(&srl2);
    hallflgs.push_back(false);
    swtchflgs.push_back(false);
    fltr2.setup(src, trgt);
    fltr2.samples = 8000;
    fltrs.push_back(&fltr2);

    srl3.setup("/dev/tty.usbmodem147411", 9600);
    srls.push_back(&srl3);
    hallflgs.push_back(false);
    swtchflgs.push_back(false);
    fltr3.setup(src, trgt);
    fltr3.flip = false;
    fltr3.stroke = 2;
    fltr3.power = 4;
    fltr3.samples = 8000;
    fltrs.push_back(&fltr3);

    srl4.setup("/dev/tty.usbmodem147421", 9600);
    srls.push_back(&srl4);
    hallflgs.push_back(false);
    swtchflgs.push_back(false);
    fltr4.setup(src, trgt);
    fltr4.neg = false;
    fltr4.doCanny = true;
    fltrs.push_back(&fltr4);

    swtchdx = -1;

    ofBackground(255);

    inpt.allocate(src.width, src.height, OF_IMAGE_GRAYSCALE);
    cnny.allocate(src.width, src.height, OF_IMAGE_GRAYSCALE);

    // touchlog
    tchlg.setup("card_touch_log.tsv");

    // gui
    gui.setup();
    gui.add(obs.minArea);
    gui.add(obs.figErode);
    gui.add(obs.figDilate);
    
    gui.add(cannyParam1.set("cannyParam1", 300, 0, 1024));
    gui.add(cannyParam2.set("cannyParam2", 150, 0, 1024));

    gui.add(bgfltr.samples);
    gui.add(bgfltr.bgAlpha);
    gui.add(bgfltr.ghostAlpha);

    gui.add(fltr0.samples);
    gui.add(fltr1.samples);
}

void ofApp::checkSerial() {
    for (std::size_t i = 0; i < srls.size(); i++) {
        if (srls[i]->isInitialized() && srls[i]->available() > 0) {
            int b = srls[i]->readByte();

            ofLogNotice() << "pillar " << i << " read: " << b << " at " << ofGetTimestampString();
            tchlg.touch(i, b); // log touch to file

            if (b == 97) { // 97 -> 'a'
                hallflgs[i] = false;
            }
            if (b == 98) { // 98 -> 'b'
                hallflgs[i] = true;
            }
            if (b == 99) { // 99 -> 'c'
                swtchflgs[i] = false;
                if (swtchdx == i) swtchdx = -1; // if current serial switches off set to index to -1
            }
            if (b == 100) { // 100 -> 'd'
                swtchflgs[i] = true;
                swtchdx = i;
            }

            // ofLogNotice() << "pillar " << i << " flags are now: h - " 
            //     << hallflgs[i] << ", s - " << swtchflgs[i];

        }
    }
}

void ofApp::update() {

    checkSerial(); // update pillar flags

	cam.update();
	if(cam.isFrameNew()) {
        scld = cam.getPixels();
        scld.mirror(false, true);
        scld.resize(src.width, src.height);
        obs.update(scld);
        const std::vector<ofPolyline> & fgrs = obs.getFigures();

        if (swtchdx == -1) {
            if (obs.getBackgroundModel(bgmdl)) {
                ofPixels bgpxls = bgmdl.getPixels();
                ofxCv::copyGray(bgpxls, inpt);
                Canny(inpt, cnny, cannyParam1 * 2, cannyParam2 * 2, 5);
                cnny.update();

                ofPixels edgpxls = cnny.getPixels();
                bgfltr.update(bgpxls, edgpxls, fgrs);
            }
        } else {
            ofxCv::copyGray(scld, inpt);
            Canny(inpt, cnny, cannyParam1 * 2, cannyParam2 * 2, 5);
            cnny.update();

            ofPixels edgpxls = cnny.getPixels();

            fltrs[swtchdx]->update(scld, edgpxls, fgrs);
        }
	}
}

void ofApp::draw() {

    if (swtchdx == -1) {
        bgfltr.draw();
    } else {
        fltrs[swtchdx]->draw();
    }

    // draw parameter sliders
    //gui.draw();

    // draw framerate
    // std::stringstream strm;
    // strm << "fps -> " << ofGetFrameRate();
    // ofDrawBitmapString(strm.str(), trgt.width - 256, 32);
}
