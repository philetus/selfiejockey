#include "TouchLog.h"

void TouchLog::setup(string fn) {

    if (!lgfl.open(ofToDataPath(fn), ofFile::Append)) {
        ofLogError() << "couldnt open '" << fn << "'!";
    }
    if (!lgfl.exists()) {
        if (!lgfl.create()) {
            ofLogError() << "couldnt save to '" << fn << "': couldnt create file";
            return;
        }
        
        // generate field names for new file
        lgfl << "pillar" << "\t" << "code" << "\t" << "timestamp" << endl;
    }

    if(!lgfl.canWrite()) {
        ofLogError() << "fail! '" << fn << "': file not writable";
    }
}

void TouchLog::touch(int pllr, int cd){
    lgfl << pllr << "\t" << cd << "\t" << ofGetTimestampString() << endl;
}