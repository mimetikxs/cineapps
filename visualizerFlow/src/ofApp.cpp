#include "ofApp.h"


void ofApp::loadData(){
    string path = "../../../common/data/flow/"; // relative to data folder
    
    string fileNames[] = {
        "bourne.csv",
        "mascotas.csv",
        "monstruo.csv",
        "tarzan.csv",
        "xmen.csv"
    };
    
    strips.clear();
    
    for(auto& fileName : fileNames){
        MovieStrip movie;
        movie.setup(path + fileName, STRIP_HEIGHT, 3);
        movie.name = ofSplitString(fileName, ".")[0];
        strips.push_back(movie);
    }
    
    // find widest stripe
    maxStripWidth = 0;
    for(auto& strip : strips){
        int stripWidth = strip.getWidth();
        if(stripWidth > maxStripWidth)
            maxStripWidth = stripWidth;
    }
}


void ofApp::setup(){
    ofBackground(100);
    
    loadData();
    
    windowResized(ofGetWidth(), ofGetHeight());
    
    gui.setup();
    gui.add(fitInWindow.set("fit in window", false));
    
    bDrawGui = true;
    
    createAngleGuide();
}


void ofApp::update(){
    
}


void ofApp::draw(){
    if(!fitInWindow){
        ofPushMatrix();
        {
            ofTranslate(screenOffset);
            float y = 30;
            for(auto& strip : strips){
                strip.draw(0, y);
                y += strip.getHeight();
            }
        }
        ofPopMatrix();
    }else{
        ofPushMatrix();
        {
            ofTranslate(0, screenOffset.y);
            float scaleFactor = (float)ofGetWidth() / maxStripWidth;
            float y = 30;
            for(auto& strip : strips){
                strip.draw(0, y, strip.getWidth()*scaleFactor);
                y += strip.getHeight();
            }
        }
        ofPopMatrix();
    }
    
    guide.draw(ofGetWidth()-70, 10);
    
    if(bDrawGui){
        gui.draw();
    }
}


void ofApp::takeSnapshot(){
    string outputPath = "../../../common/footprints/flow/";
    
    ofFbo outFbo;
    ofPixels outPix;
    
    outFbo.allocate(maxStripWidth, strips.size() * strips[0].getHeight());
    outFbo.begin();
    {
        ofClear(100);
        
        guide.draw(0,0);
        
        float y = guide.getHeight() + 20;
        for(auto& strip : strips){
            strip.draw(0, y);
            y += strip.getHeight();
        }
    }
    outFbo.end();
    
    outFbo.readToPixels(outPix);
    ofSaveImage(outPix, outputPath + "_flow_all.png");
    
    // export each footprint as an individual image
    for(auto& strip : strips){
        const ofTexture& tex = strip.getTexture();
        tex.readToPixels(outPix);
        ofSaveImage(outPix, outputPath + "flow_" + strip.name + ".png");
    }
}


void ofApp::keyPressed(int key){
    switch (key) {
        case 'f':
            ofToggleFullscreen();
            break;
        case 'r':
            loadData();
            break;
        case 'g':
            bDrawGui = !bDrawGui;
            break;
        case 's':
            takeSnapshot();
            break;
        default:
            break;
    }
}


void ofApp::mouseDragged(int x, int y, int button){
    ofVec2f mouse = ofVec2f(x,y);
    ofVec2f draggedDist = mouse - clickPos;
    
    screenOffset = offsetOnClick + draggedDist;
    screenOffset.x = round(screenOffset.x);
    screenOffset.y = round(screenOffset.y);
    screenOffset.x = ofClamp(screenOffset.x, -overflow.x, 0);
    screenOffset.y = ofClamp(screenOffset.y, -overflow.y, 0);
}


void ofApp::mousePressed(int x, int y, int button){
    clickPos.set(x, y);
    offsetOnClick.set(screenOffset);
}


void ofApp::mouseReleased(int x, int y, int button){

}


void ofApp::windowResized(int w, int h){
    overflow.x = maxStripWidth - ofGetWidth();
    overflow.y = (strips.size() * strips[0].getHeight()) - ofGetHeight();
    overflow.x = (overflow.x > 0) ? overflow.x : 0;
    overflow.y = (overflow.y > 0) ? overflow.y : 0;
}


void ofApp::dragEvent(ofDragInfo dragInfo){ 

}


void ofApp::createAngleGuide(){
    int size = 60;
    ofVec2f center(size/2, size/2);
    
    guide.allocate(size, size, GL_RGBA8);
    guide.begin(); ofClear(0); guide.end();
    
    ofPushStyle();
    ofSetLineWidth(1);
    guide.begin();
    {
        float radius = size/2;
        for(int angle=0; angle<360; angle++){
            ofVec2f dir;
            dir.x = cos(float(angle));
            dir.y = sin(float(angle));
            ofColor col;
            col.r = (0.5 + dir.x / 2) * 255;
            col.g = (1 - (0.5 + dir.y / 2)) * 255;
            col.b = 0;
            ofSetColor(col);
            ofVec2f tip = center + dir * radius;
            ofDrawLine(center, tip);
        }
    }
    guide.end();
    ofPopStyle();
}