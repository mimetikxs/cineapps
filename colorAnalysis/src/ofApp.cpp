#include "ofApp.h"


bool compareSaturation( const ofColor& a, const ofColor& b ) {
    return a.getSaturation() < b.getSaturation();
}

bool compareHue( const ofColor& a, const ofColor& b ) {
    return a.getHue() < b.getHue();
}

bool compareBrightness( const ofColor& a, const ofColor& b ) {
    return a.getBrightness() < b.getBrightness();
}

bool compareR( const ofColor& a, const ofColor& b ) {
    return a.r < b.r;
}

bool compareB( const ofColor& a, const ofColor& b ) {
    return a.b < b.b;
}

bool compareG( const ofColor& a, const ofColor& b ) {
    return a.g < b.g;
}

// https://www.eembc.org/techlit/datasheets/yiq_consumer.pdf
// https://en.wikipedia.org/wiki/YIQ
//bool compareY( const ofColor& a, const ofColor& b ) {
//    return 0.299*float(a.r)/255 + 0.587*float(a.g)/255 + 0.114*float(a.b)/255  <  0.299*float(b.r)/255 + 0.587*float(b.g)/255 + 0.114*float(b.b)/255;
//}
//bool compareI( const ofColor& a, const ofColor& b ) {
//    return 0.596*float(a.r)/255 - 0.275*float(a.g)/255 - 0.321*float(a.b)/255  <  0.596*float(b.r)/255 - 0.275*float(b.g)/255 - 0.321*float(b.b)/255;
//}
//bool compareQ( const ofColor& a, const ofColor& b ) {
//    return 0.212*float(a.r)/255 - 0.523*float(a.g)/255 + 0.311*float(a.b)/255  <  0.212*float(b.r)/255 - 0.523*float(b.g)/255 + 0.311*float(b.b)/255;
//}



//--------------------------------------------------------------
void ofApp::setup(){
    pathStills = "../../../common/stills-color/";
    
    currentFrame = 0;
    isDone = false;
    isExtracting = false;
    
    gui.setup();
    gui.add(cropX.set("cropX", 0, 0, 100));
    gui.add(cropY.set("cropY", 0, 0, 100));
    gui.add(skipFrames.set("skip frames", 20, 1, 50));
    gui.add(thumbScale.set("thumb scale", 1.0, 0.0, 1.0));
    gui.add(buttonBegin.setup("extract"));
    
    cropX.addListener(this, &ofApp::onCropChange);
    cropY.addListener(this, &ofApp::onCropChange);
    thumbScale.addListener(this, &ofApp::onThumbScaleChange);
    buttonBegin.addListener(this, &ofApp::extract);
    
    colorGridPreview.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
}

//--------------------------------------------------------------
void ofApp::update(){
    video.update();
    
    if (video.isFrameNew()) {
        processFrame();
        
        isDone = (currentFrame > video.getTotalNumFrames());
        
        if(isExtracting && !isDone) {
            ofImage thumb(colorGridScaled);
            thumbs.push_back(thumb);
            
            stillCount += 1;
            currentFrame += skipFrames;
            
            // IMPORTANT:
            // ofVideoPlayer::setFrame() won't work when skipFrames < 20, we need more keyframes (see: https://github.com/openframeworks/openFrameworks/issues/4651)
            // H264 is problematic, is best to have a video format like photoJPG with keyframes every frame
            video.setFrame(currentFrame);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (video.isLoaded()) {
        float width = video.getWidth() - cropX;
        float height = video.getHeight() - cropY;
        float duration = video.getDuration();
        float totalFrames = video.getTotalNumFrames();
        int totalStills = video.getTotalNumFrames() / skipFrames;
        
        // main previews
        video.getTexture().drawSubsection(0, 0, width, height, cropX/2, cropY/2); // we use drawDubsection to avoid having to upload cropped pixels
        colorGridPreview.draw(width, 0, width, height);
        
        ofDrawBitmapStringHighlight(clipName + " (" + ofToString(int(duration/60), 2, '0') + ":" + ofToString(int(duration)%60, 2, '0') + ")", 20,height+=20);
        ofDrawBitmapStringHighlight("frame: " + ofToString(video.getCurrentFrame()) + "/" + ofToString(totalFrames), 20,height+=20);
        ofDrawBitmapStringHighlight( ofToString(stillCount) + "/" + ofToString(totalStills) + " frames processed", 20,height+=20, ofColor::darkRed);
        if (isDone) ofDrawBitmapStringHighlight( "completed! drop another video file", 20,height+=20, ofColor::darkRed);
        
        // thumbnails preview
        colorGridScaledPreview.draw(20, height+=20);
        
        float x = 0;
        float y = height;
        float w = colorGridScaled.getWidth();
        float h = colorGridScaled.getHeight();
        for (int i = 0; i < thumbs.size(); i++) {
            thumbs[i].draw(x, y);
            x += w;
            if (x > ofGetWidth()) {
                x = 0;
                y += h;
            }
        }
    } else {
        ofDrawBitmapStringHighlight("drop a video file", 20,300, ofColor::darkRed);
    }
    
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::processFrame() {
    // crop
    video.getPixels().cropTo(videoPixCropped, cropX/2, cropY/2, video.getWidth()-cropX, video.getHeight()-cropY);
    
    // extract color
    extractColors(videoPixCropped, colorGrid);
    
    // create thumbnail
    colorGridScaled.allocate(colorGrid.getWidth() * thumbScale, colorGrid.getHeight() * thumbScale, colorGrid.getNumChannels());
    colorGrid.resizeTo(colorGridScaled);//, OF_INTERPOLATE_BICUBIC);
    
    // update previews
    colorGridPreview.allocate(colorGrid.getWidth(), colorGrid.getHeight(), GL_RGB8); // force texture realocation to avoid artifacts
    colorGridPreview.loadData(colorGrid);
    colorGridScaledPreview.allocate(colorGridScaled.getWidth(), colorGridScaled.getHeight(), GL_RGB8); // force texture realocation
    colorGridScaledPreview.loadData(colorGridScaled);
}

//--------------------------------------------------------------
void ofApp::extractColors(const ofPixels & in, ofPixels & out) {
    int step = 1;
    int numChannels = 3;
    int widthIn = in.getWidth();
    int heightIn = in.getHeight();
    int widthOut = float(widthIn) / float(step);
    int heightOut = float(heightIn) / float(step);
    
    // sort
    vector<ofColor> colors;
    for (int y = 0; y <= heightIn-step; y += step) {
        for (int x = 0; x <= widthIn-step; x += step) {
            int i = (y*widthIn+x) * numChannels;
            ofColor c(in[i], in[i+1], in[i+2]);
            colors.push_back(c);
        }
    }
    
    // sort by HSB
    ofSort(colors, compareHue);
    ofSort(colors, compareSaturation);
    ofSort(colors, compareBrightness);
    
    // sort by RGB
//    ofSort(colors, compareR);
//    ofSort(colors, compareG);
//    ofSort(colors, compareB);
    
    // out
    out.allocate(widthOut, heightOut, numChannels);
    
    for (int i = 0; i < colors.size(); i += 1) {
        const ofColor& c = colors[i];
        int j = i * numChannels;
        out[j] = c.r;
        out[j+1] = c.g;
        out[j+2] = c.b;
    }
}

//--------------------------------------------------------------
void ofApp::extract() {
    if (!video.isLoaded())
        return;
    
    stillCount = 0;
    currentFrame = 0;
    isDone = false;
    isExtracting = true;
    
    thumbs.clear();

    video.firstFrame();
}

//--------------------------------------------------------------
void ofApp::saveThumbs() {
    for (int i = 0; i < thumbs.size(); i++) {
        ofSaveImage(thumbs[i].getPixels(), pathStills + clipName + "/" + ofToString(i) + ".png");
    }
}

//--------------------------------------------------------------
void ofApp::onThumbScaleChange(float &scale) {
    thumbs.clear();
    processFrame();
    
    float w = colorGridScaled.getWidth();
    float h = colorGridScaled.getHeight();
    cout << "thumb size = " << w*h << " - " << w << " x " << h << endl;
}

//--------------------------------------------------------------
void ofApp::onCropChange(int &val) {
    thumbs.clear();
    processFrame();
    
    float w = colorGridScaled.getWidth();
    float h = colorGridScaled.getHeight();
    cout << "thumb size = " << w*h << " - " << w << " x " << h << endl;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case OF_KEY_RIGHT:
            video.nextFrame();
            //currentFrame += skipFrames; //not working when skipFrames < 20, we need more keyframes (see: https://github.com/openframeworks/openFrameworks/issues/4651)
            break;
        case OF_KEY_LEFT:
            video.previousFrame();
            //currentFrame -= skipFrames;
            break;
        case OF_KEY_RETURN:
            
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    if (dragInfo.files.size() == 1){
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
        video.setLoopState(OF_LOOP_NONE);
        video.setPaused(true);
        video.firstFrame();
        video.update();
    } else {
        ofLog(OF_LOG_ERROR, "drop only one file at a time");
    }
}
