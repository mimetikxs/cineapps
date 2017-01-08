#pragma once

#include "ofMain.h"

#include "MovieStrip.h"


const float STRIP_HEIGHT = 100;
const float MARGIN = 20;


class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    
    void takeSnapshot();
    
    vector<MovieStrip> strips;
    int maxChunks;
    int chunckWidth;

    void keyPressed(int key);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
		
};
