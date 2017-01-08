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
        
        chunkScale = 5;
        chunkWidth = 1;
    }
    
    
    
    //void setup(string dataPath, float width, float height){
    void setup(string dataPath, int height, int chunkWidth_ = 1){
        // TODO: check that path exists
        
        ofBuffer buffer = ofBufferFromFile(dataPath);
        for(auto& line : buffer.getLines()){
            if(line.size() > 0)
                lines.push_back(line);
        }
        lines.erase(lines.begin()); // the first line is the table's header
        
        //
        numFrames = lines.size();
        chunkWidth = chunkWidth_;
        
        // draw
        drawStrip(chunkWidth, height);
    };
    
    
    
    void resize(float chunkWidth, float height){
        drawStrip(chunkWidth, height);
    }
    
    
    
    void draw(float x, float y, float w = -1, float h = -1){
        string label = name + " (" + ofToString(numFrames) + " frames)";
        ofDrawBitmapStringHighlight(label, x+5, y);
        
        w = (w == -1) ? fboStrip.getWidth() : w;
        h = (h == -1) ? fboStrip.getHeight() : h;
        fboStrip.draw(x, y+10, w, h);
    }
    
    
    
    int getNumFrames() { return numFrames; }
    
    
    int getHeight() { return fboStrip.getHeight() + 30; }
    int getWidth() { return numFrames * chunkWidth; }
    
    
    const ofTexture& getTexture(){ return fboStrip.getTexture(); }
    
    
    string name; // TODO: extract from file
    
    
    
protected:
    vector<string> lines;
    
    ofFbo fboStrip;
    
    int numFrames;
    
    float chunkScale;
    int chunkWidth;
    
    
    //void drawStrip(float width, float height){
    void drawStrip(int chunkWidth, int height){
        if(chunkWidth > 3){
            chunkWidth = 3;
            ofLog(OF_LOG_ERROR) << "MovieStrip::chunkSize can't be greater than 3 to avoid overflowing video memory. Value has been clipped to 3.";
        }
        
        int stripWidth = chunkWidth * numFrames;
        fboStrip.allocate(stripWidth, height, GL_RGB8);
        fboStrip.begin();
        {
            ofClear(50);
            
            //float chunkScale = 15; //(10.f/400.f) * height;
            
            float x = 0;
            float y = height / 2;
            
            for(int i = 0; i < numFrames; i++){
                vector<string> line = ofSplitString(lines[i], ",");
                
                ofVec2f avgFrameFlow(ofToFloat(line[0]), ofToFloat(line[1]));
                float avgIntensity = ofToFloat(line[2]);
                
                // map angle to color
                ofVec2f normalizedDir(avgFrameFlow.getNormalized());
                normalizedDir *= 0.5; // remap from [-1..1] to [0..1]
                normalizedDir += 0.5;
                ofColor angleColor;
                angleColor.r = normalizedDir.x * 255;
                angleColor.g = normalizedDir.y * 255;
                angleColor.b = 0;
                
                ofPushStyle();
                {
                    ofSetLineWidth(chunkWidth);
                    ofSetColor(ofColor::white);
                    ofDrawLine(x, y, x, y - avgIntensity * chunkScale);
                    ofSetColor(angleColor);
                    ofDrawLine(x, y, x, y + avgFrameFlow.length() * chunkScale);
                }
                ofPopStyle();
                
                x += chunkWidth;
            }
        }
        fboStrip.end();
    }
};