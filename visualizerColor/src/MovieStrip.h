//
//  Movie.h
//  visualizerColor
//
//  Created by Nestor Rubio Garcia on 05/01/2017.
//
//

#pragma once

#include "ofMain.h"

//#define FRAME_HEIGHT 100
//#define FRAME_WIDTH 5

class MovieStrip {

public:
    MovieStrip(){
        name = "";
        numFrames = 0;
        colorsPerFrame = 0;
    }
    
    
    
    //void setup(string dataPath, float width, float height){
    void setup(string dataPath, int height, int chunkWidth = 5){
        // TODO: check that path exists
        
        ofBuffer buffer = ofBufferFromFile(dataPath);
        for(auto& line : buffer.getLines()){
            if(line.size() > 0)
                this->lines.push_back(line);
        }
        this->lines.erase(lines.begin()); // the first line is the table's header
        
        //
        colorsPerFrame = 8; // TODO: extract from file
        numFrames = lines.size() / colorsPerFrame;
        
        // draw
        drawStrip(chunkWidth, height);
    };
    
    
    
    void resize(float chunkWidth, float height){
        drawStrip(chunkWidth, height);
    }
    
    
    
    void draw(float x, float y){
        string label = name + " (" + ofToString(numFrames) + " frames)";
        ofDrawBitmapStringHighlight(label, x+5, y);
        fboStrip.draw(x, y+10);
    }
    
    
    
    int getNumFrames() { return numFrames; }
    
    
    const ofTexture& getTexture() { return fboStrip.getTexture(); }
    
    
    
    string name; // TODO: extract from file
    
    
    
protected:
    //ofBuffer buffer;
    vector<string> lines;
    
    ofFbo fboStrip;
    
    int numFrames;
    int colorsPerFrame;
    
    
    
    //void drawStrip(float width, float height){
    void drawStrip(int chunkWidth, int height){
        // draw individual chunks
        vector<ofFbo> fbos;
        
        for(int i = 0; i < numFrames; i++){
            ofFbo fbo;
            fbo.allocate(20, 200, GL_RGBA8);
            fbo.begin();
                ofClear(0);
            fbo.end();
            
            float x = 0;
            float y = 0;
            float w = chunkWidth;
            float h = height;
            
            for(int j = 0; j < colorsPerFrame; j++){
                string line = lines[i * colorsPerFrame + j];
                vector<string> data = ofSplitString(line, ",");
                ofColor color;
                color.r = ofToFloat(data[3]); // NOTE: we need to flip colors from opencv's BGR color space
                color.g = ofToFloat(data[2]);
                color.b = ofToFloat(data[1]);
                float pct = ofToFloat(data[4]);
                float rectHeight = h * pct;
                fbo.begin();
                {
                    ofPushStyle();
                    ofSetColor(color);
                    ofDrawRectangle(x, y, w, rectHeight);
                    ofPopStyle();
                    y += rectHeight;
                }
                fbo.end();
            }
            fbos.push_back(fbo);
        }
        
        // draw the strip
        int stripWidth = chunkWidth * numFrames;
        fboStrip.allocate(stripWidth, height, GL_RGBA8);
        fboStrip.begin();
        {
            ofClear(0);
            int x = 0;
            for(auto& fbo : fbos){
                fbo.draw(x,0);
                x += chunkWidth;
            }
        }
        fboStrip.end();
    }
};