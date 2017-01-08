#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Brush.h"

struct Color{
    ofColor color;
    float weight;
};


class ofApp : public ofBaseApp{

public:
    const int backColor = 243;
    
    void setup();
    void update();
    void draw();
    
    void reset();
    void loadPalette(string path);

    ofImage mask;
    ofFbo canvas;
    
    vector<Color> colors;
    vector<Brush> brushes;
    
    ofFbo colorsPreview;
    ofPixels colorsPalette; // color lookup
    
    ofRectangle bounds;
    
    ofTexture tex;
    
    // gui    
    ofxPanel gui;
    ofParameter<int> brushOpacity;
    ofParameter<int> numBrushes;
    ofParameter<float> brushSpeed;
    ofParameter<bool> bDrawMask;
    ofParameter<bool> bClearCanvas;
    ofxButton buttonReset;

    void keyPressed(int key);
    void keyReleased(int key);
    void dragEvent(ofDragInfo dragInfo);
};
