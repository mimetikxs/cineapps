//
//  Brush.h
//  01_backgroundDrawing
//
//  Created by Nestor Rubio Garcia on 07/01/2017.
//
//

#pragma once

#include "ofMain.h"


class Brush {
    
public:
    float angle;
    int* components;
    float x, y;
    ofColor clr;
    
    int opacity;
    
    Brush(ofColor color, ofRectangle bounds) {
        angle = ofRandom(TWO_PI);
        x = ofRandom(bounds.width);
        y = ofRandom(bounds.height);
        
        opacity = 5;        
        clr.set(color);
        
        components = new int[2];
        for (int i = 0; i < 2; i++) {
            components[i] = int(ofRandom(1, 3));
        }
    }
    
    //void paint(float width, float height) {
    void paint(ofRectangle bounds) {
        float a = 0;
        float r = 0;
        float x1 = x;
        float y1 = y;
        float u = 1; //ofRandom(0.5, 1.5); // random pulsation per frame (controls the overall scale)
        
        ofPushStyle();
        {
            clr.a = opacity;
            ofSetColor(clr);
            
            ofBeginShape();
            while (a < TWO_PI) {
                ofVertex(x1, y1);
                float v = ofRandom(0.85, 1); // random radius (creates spikes)
                x1 = x + r * cos(angle + a) * u * v;
                y1 = y + r * sin(angle + a) * u * v;
                a += PI / 180;
                for (int i = 0; i < 2; i++) {
                    r += sin(a); // single componet brush
//                    r += sin(a * components[i]); // creates a brush with variable components
                }
            }
            ofEndShape(true);
        }
        ofPopStyle();
        
        if (x < bounds.x || x >= bounds.width || y < bounds.y || y >= bounds.height) {
            angle += HALF_PI;
        }
        
//        if(y < bounds.y){
//            y = bounds.y;
//        }
        
        
        float speed = 2;
        x += speed * cos(angle);
        y += speed * sin(angle);
        angle += ofRandom(-0.15, 0.15);
    }
};
