#pragma once

#include "ofMain.h"
#include "ofxCv.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
    
    void keyPressed  (int key);
    void dragEvent(ofDragInfo dragInfo);
	
    ofVideoPlayer video;
	ofxCv::FlowFarneback flow;
	int stepSize, xSteps, ySteps;
    int totalNumFrames;
    
    ofVec2f avgFrameFlow;   // avg flow of the frame. accounts for direction.
    float avgIntensity;     // amount of movement of the frame. does not account for direction.
    
    // values storage (one per frame)
    ofVec2f* flowValues;
    float* instensityValues;
    
    ofFbo fboCanvas;
    
    bool wrap;
    
    int chunkStep;
    
    bool bPaused;
    
    void saveData();
    
    string clipName;
    
//    ofImage guide;
    ofFbo guide;
    void createAngleGuide();
    
    void loadVideo(string path);
};