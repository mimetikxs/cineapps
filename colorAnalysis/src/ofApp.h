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
    void processFrame();
    void extractColors(const ofPixels & in, ofPixels & out);
    void saveThumbs();
    
    // video input
    ofVideoPlayer video;
    ofPixels videoPixCropped;
    
    // color grid
    ofPixels colorGrid;
    ofTexture colorGridPreview;
    ofPixels colorGridScaled;
    ofTexture colorGridScaledPreview;
    vector<ofImage> thumbs;
    
    // video scrubbing
    int stillCount;
    int currentFrame;
    bool isDone;
    bool isExtracting;

    string clipName;
    string pathMovies;
    string pathStills;
    
    // parameters
    ofxPanel gui;
    ofParameter<int> cropX;
    ofParameter<int> cropY;
    ofParameter<int> skipFrames;
    ofParameter<float> thumbScale;
    ofxButton buttonBegin;
    
    void onThumbScaleChange(float& scale);
    void onCropChange(int& val);
};
