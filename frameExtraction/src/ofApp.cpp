#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    pathStills = "../../../common/stills/";
    
    isDone = false;
    isExtracting = false;
    
    gui.setup();
    gui.add(cropX.set("cropX", 0, 0, 100));
    gui.add(cropY.set("cropY", 0, 0, 100));
    gui.add(skipFrames.set("skip frames", 20, 1, 50));
    gui.add(buttonBegin.setup("extract"));
    
    buttonBegin.addListener(this, &ofApp::extract);
}

//--------------------------------------------------------------
void ofApp::update(){
    video.update();
    
    if(video.isFrameNew() && !isDone && isExtracting) {
        // move head
        video.setFrame(currentFrame);
        
        // save pixels
        video.getPixels().cropTo(pix, cropX/2, cropY/2, video.getWidth()-cropX, video.getHeight()-cropY);
        ofSaveImage(pix, pathStills + clipName + "/" + ofToString(stillCount) + ".png");
        
        stillCount += 1;
        currentFrame += skipFrames;
        isDone = (currentFrame > video.getTotalNumFrames());
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (video.isLoaded()) {
        float height = video.getHeight() - cropY;
        float duration = video.getDuration();
        float totalFrames = video.getTotalNumFrames();
        int totalStills = video.getTotalNumFrames() / skipFrames;
        
        video.getTexture().drawSubsection(0, 0, video.getWidth()-cropX, video.getHeight()-cropY, cropX/2, cropY/2);
        
        ofDrawBitmapStringHighlight(clipName, 20,height+=40);
        ofDrawBitmapStringHighlight("duration: " + ofToString(int(duration/60), 2, '0') + ":" + ofToString(int(duration)%60, 2, '0'), 20,height+=20);
        ofDrawBitmapStringHighlight("frame: " + ofToString(currentFrame) + "/" + ofToString(totalFrames), 20,height+=20);
        ofDrawBitmapStringHighlight( ofToString(stillCount) + "/" + ofToString(totalStills) + " stills extracted", 20,height+=20, ofColor::darkRed);
        if (isDone) ofDrawBitmapStringHighlight( "extranction completed, drop another file", 20,height+=20, ofColor::darkRed);
    } else {
        ofDrawBitmapStringHighlight("drop a video file", 20,300, ofColor::darkRed);
    }
    
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::extract() {
    if (!video.isLoaded())
        return;
    
    stillCount = 0;
    currentFrame = 0;
    isDone = false;
    isExtracting = true;
    // testing
    video.nextFrame();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    // we only need this if the movie gets stuck
    switch (key) {
        case OF_KEY_RETURN:
            video.nextFrame();
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
//    if (!isDone){
//        return;
//    }
    
    if( dragInfo.files.size() == 1 ){
        isExtracting = false;
        
        string path = dragInfo.files[0];
        
        // ignore directories
        ofFile file(path);
        if (file.isDirectory()){
            return;
        }
        
        // get movie file name
        clipName = ofSplitString(path, "/").back();
        
        // clear the folder
        ofFile stillsFolder(pathStills + clipName);
        if (stillsFolder.exists()) stillsFolder.remove(true);
        
        // load the video
        video.load(path);
    } else {
        ofLog(OF_LOG_ERROR, "drop only one file at a time");
    }
}
