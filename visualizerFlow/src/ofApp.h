#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "MovieStrip.h"


const float STRIP_HEIGHT = 200;

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    
    void loadData();
    
    void takeSnapshot();
    
    vector<MovieStrip> strips;
    int maxStripWidth;
    
    bool bDrawGui;
    ofxPanel gui;
    ofParameter<bool> fitInWindow;
    
    // click and drag
    ofVec2f screenOffset;
    ofVec2f offsetOnClick;
    ofVec2f clickPos;
    ofVec2f overflow;
    
    // angle legend
    ofFbo guide;
    void createAngleGuide();

    void keyPressed(int key);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
};
