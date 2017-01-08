#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void dragEvent(ofDragInfo dragInfo);
    
    void extract();
    
    ofVideoPlayer video;
    ofPixels pix;
    
    int stillCount;
    int currentFrame;
    bool isDone;
    bool isExtracting;

    string clipName;
    string pathMovies;
    string pathStills;
    
    ofxPanel gui;
    ofParameter<int> cropX;
    ofParameter<int> cropY;
    ofParameter<int> skipFrames;
    ofxButton buttonBegin;
};
