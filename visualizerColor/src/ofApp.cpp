#include "ofApp.h"


void ofApp::setup(){
    string path = "../../../common/data/color/"; // relative to data folder
    
    string fileNames[] = {
        "bourne.csv",
        "mascotas.csv",
        "monstruo.csv",
        "tarzan.csv",
        "xmen.csv"
    };
    
    for(auto& fileName : fileNames){
        MovieStrip movie;
        movie.setup(path + fileName, STRIP_HEIGHT);
        movie.name = ofSplitString(fileName, ".")[0];
        strips.push_back(movie);
    }
    
    // calculate chunksize
    maxChunks = 0;
    for(auto& strip : strips){
        int stripFrames = strip.getNumFrames();
        if(stripFrames > maxChunks)
            maxChunks = stripFrames;
    }
    
    // resize strips
    chunckWidth = (float)(ofGetWidth() - MARGIN*2) / maxChunks;
    for(auto& strip : strips){
        strip.resize(chunckWidth, STRIP_HEIGHT);
    }
}


void ofApp::update(){
    
}


void ofApp::draw(){
    float y = 30;
    for(auto& strip : strips){
        strip.draw(MARGIN, y);
        y += 150;
    }
}

void ofApp::takeSnapshot(){
    string outputPath = "../../../common/footprints/color/";
    
    ofFbo outFbo;
    ofPixels outPix;
    
    // export all footprints as one image
    outFbo.allocate(ofGetWidth(), ofGetHeight());
    outFbo.begin();
    {
        ofClear(100);
        float y = 20;
        for(auto& strip : strips){
            strip.draw(0, y);
            y += 150;
        }
    }
    outFbo.end();
    outFbo.readToPixels(outPix);
    ofSaveImage(outPix, outputPath + "_color_all.png");
    
    // export each footprint as an individual image
    for(auto& strip : strips){
        const ofTexture& tex = strip.getTexture();
        tex.readToPixels(outPix);
        ofSaveImage(outPix, outputPath + "color_" + strip.name + ".png");
    }
}


void ofApp::keyPressed(int key){
    switch(key){
        case 's':
            takeSnapshot();
            break;
        case 'f':
            ofToggleFullscreen();
            break;
        default:
            break;
    }
}


void ofApp::mouseDragged(int x, int y, int button){

}


void ofApp::mousePressed(int x, int y, int button){

}


void ofApp::mouseReleased(int x, int y, int button){

}


void ofApp::windowResized(int w, int h){
    int chunkWidth = (float)(ofGetWidth() - MARGIN*2) / maxChunks;
    cout << chunkWidth << endl;
    for(auto& strip : strips){
        strip.resize(chunkWidth, STRIP_HEIGHT);
    }
}


void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
