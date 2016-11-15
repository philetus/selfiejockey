/*
 * log card touches to file
 */
#pragma once

#include "ofMain.h"

class TouchLog {
public:
    TouchLog() {}
    ~TouchLog() {}

    void setup(string fn);
    void touch(int pllr, int cd);

    ofFile lgfl;
};
