#include "ofApp.h"

using namespace ofxCv;
using namespace cv;



void ofApp::setup() {
	ofSetVerticalSync(true);
	//ofSetFrameRate(25);
    
    createAngleGuide();
	
    loadVideo("../../../common/movies/monstruo.mp4");
    
	stepSize = 8;
	ySteps = video.getHeight() / stepSize;
	xSteps = video.getWidth() / stepSize;
    
    //
    chunkStep = 2;
    
    //fboCanvas.allocate(1440, 900-video.getHeight());
    fboCanvas.allocate(totalNumFrames*chunkStep, 400);
    fboCanvas.begin();
        ofClear(0);
    fboCanvas.end();
    
    wrap = true;
    
    bPaused = true;
}



void ofApp::update() {
//    if(ofGetFrameNum() % 2 == 0)
//        return;
    
    if(bPaused)
        return;
    
    video.update();
    
	if(video.isFrameNew()) {
		flow.setWindowSize(stepSize);
		flow.calcOpticalFlow(video);
        
        avgIntensity = 0;
        for(int y = 1; y < ySteps; y++) {
            for(int x = 0; x < xSteps; x++) {
                float intensity = flow.getFlowOffset(x, y).length();
                avgIntensity += intensity;
            }
        }
        avgIntensity /= xSteps*ySteps;
        avgFrameFlow = flow.getAverageFlow();
        
        // store
        instensityValues[video.getCurrentFrame()] = avgIntensity;
        flowValues[video.getCurrentFrame()] = avgFrameFlow;
        
        // draw preview in canvas
        fboCanvas.begin();
        {
            int i = video.getCurrentFrame();
            int x = (i*chunkStep) % (totalNumFrames*chunkStep);
            //int y = (wrap) ? (100 + ((i*step) / 1440) * 100) : 100;
            int y = fboCanvas.getHeight() / 2;
            
            // map angle to color
            ofVec2f normalizedDir(avgFrameFlow.getNormalized());
            normalizedDir *= 0.5;
            normalizedDir += 0.5;
            ofColor angleColor;
            angleColor.r = normalizedDir.x * 255;
            angleColor.g = normalizedDir.y * 255;
            angleColor.b = 0;
            
            ofPushStyle();
            {
                ofSetLineWidth(chunkStep);
                ofSetColor(ofColor::white);
                ofDrawLine(x, y, x, y - avgIntensity * 10);
                ofSetColor(angleColor);
                ofDrawLine(x, y, x, y + avgFrameFlow.length() * 10);
            }
            ofPopStyle();
        }
        fboCanvas.end();
        
        video.nextFrame();
	}
}



void ofApp::draw() {
	ofBackground(100);
    ofSetFullscreen(true);
	
    video.draw(0,0);
    flow.draw(0,0);
    
    guide.draw(900, 50);
    
    // ---
    // draw avg flow
    float flowAngle = avgFrameFlow.angle(ofVec2f(0,1));
    float flowLength = avgFrameFlow.length();
    ofPushStyle();
    {
        ofNoFill();
        ofPushMatrix();
        {
            ofTranslate(video.getWidth() + 20, 20);
            ofPushMatrix();
            {
                ofTranslate(25, 50);
                ofRotate(flowAngle);
                ofSetColor(ofColor::lightGrey);
                ofDrawCircle(0, 0, 25);
                ofDrawLine(0, 0, 0, -25);
                ofSetColor(ofColor::magenta);
                ofSetLineWidth(2);
                ofDrawLine(0, 0, 0, -flowLength * 50);
                ofSetLineWidth(1);
            }
            ofPopMatrix();
            ofSetColor(ofColor::white);
            ofTranslate(0, 100);
            ofDrawBitmapString("FLOW (AVG.)", 0, 0);
            ofTranslate(0, 20);
            ofDrawBitmapString("angle: " + ofToString(flowAngle, 2), 0, 0);
            ofTranslate(0, 20);
            ofDrawBitmapString("length: " + ofToString(flowLength, 2), 0, 0);
        }
        ofPopMatrix();
    }
    ofPopStyle();
    ofDrawBitmapStringHighlight(ofToString(video.getCurrentFrame()) + "/" + ofToString(totalNumFrames), 20, 20);
    
    // ----
    // draw intensity
    ofPushStyle();
    {
        ofPushMatrix();
        {
            ofTranslate(video.getWidth() + 200, 70);
            ofNoFill();
            ofDrawRectangle(0, 0, 200, 5);
            ofFill();
            ofDrawRectangle(0, 0, 20 * avgIntensity, 5);
            ofTranslate(0, 50);
            ofDrawBitmapString("MOVEMENT INTENSITY", 0, 0);
        }
        ofPopMatrix();
    }
    ofPopStyle();
    
    // ---
    // draw strip
    float headPosition = video.getCurrentFrame() * chunkStep;
    float pct = float(video.getCurrentFrame()) / totalNumFrames;
    
    // scale when overflowing
    /*
    float canvasWidth;
    if(headPosition > ofGetWidth()){
        canvasWidth = (1.f / pct) * ofGetWidth();
    }else{
        canvasWidth = fboCanvas.getWidth();
    }
    fboCanvas.draw(0, video.getHeight(), canvasWidth, fboCanvas.getHeight());
    */
    
    // scroll when overflowing
    float overflowed = headPosition - ofGetWidth();
    float x = 0;
    if(overflowed > 0){
        x = -overflowed;
    }
    fboCanvas.draw(x, video.getHeight(), fboCanvas.getWidth(), fboCanvas.getHeight());
}



void ofApp::loadVideo(string path){
    video.load(path);
    video.setLoopState(OF_LOOP_NONE);
    video.setPaused(true);
    video.firstFrame();
    
    totalNumFrames = video.getTotalNumFrames();
    
    flowValues = new ofVec2f[totalNumFrames];
    instensityValues = new float[totalNumFrames];
    
    flow.resetFlow();
    
    fboCanvas.begin();
        ofClear(0);
    fboCanvas.end();
}



void ofApp::saveData(){
    string pathToData = "../../../common/data/flow/";
    string filePrefix = "flow_";
    string fileExtension = ".csv";
    string filePath = pathToData + filePrefix + clipName + fileExtension;
    ofFile file(filePath, ofFile::WriteOnly); // file does not exists
    file.create(); // now file exists
    // write the table header
    file << "flowX" << "," << "flowY" << "," << "intensity" << endl;
    for(int i=0; i < totalNumFrames; i++){
        string flowX = ofToString(flowValues[i].x);
        string flowY = ofToString(flowValues[i].y);
        string instensity = ofToString(instensityValues[i]);
        file << flowX << "," << flowY << "," << instensity << endl;
    }
    file.close();
}



void ofApp::keyPressed(int key) {
    switch (key) {
        case OF_KEY_RIGHT:
            break;
        case OF_KEY_LEFT:
            break;
        case OF_KEY_RETURN:
            bPaused = !bPaused;
            break;
        case 's':
        case 'S':
            saveData();
            break;
        default:
            break;
    }
}



void ofApp::dragEvent(ofDragInfo dragInfo){
    if (dragInfo.files.size() == 1){
        string path = dragInfo.files[0];
        
        ofFile file(path);
        if (file.isDirectory()){
            return;
        }
        
        clipName = ofSplitString(path, "/").back();
        clipName = ofSplitString(clipName, ".")[0];
        
        loadVideo(path);
    } else {
        ofLog(OF_LOG_ERROR, "drop only one file at a time");
    }
}



void ofApp::createAngleGuide(){
    int size = 60;
    ofVec2f center(size/2, size/2);
    
    //    guide.allocate(size, size, OF_IMAGE_COLOR);
    //    for (int y = 0; y < size; y++) {
    //        for (int x = 0; x < size; x++) {
    //            ofColor c;
    //            c.r = (float)x/size * 255;
    //            c.g = (float)y/size * 255;
    //            c.b = 0;
    //            guide.setColor(x,y,c);
    //            cout << c << endl;
    //        }
    //    }
    //
    //    for (int y = 0; y < size; y++) {
    //        for (int x = 0; x < size; x++) {
    //            float angle = atan2(y-25, x-25);
    //            ofVec2f dir;
    //            dir.x = cos(float(angle));
    //            dir.y = sin(float(angle));
    //            ofColor col;
    //            col.r = dir.x * 255;
    //            col.g = dir.y * 255;
    //            col.b = 0;
    //            guide.setColor(x,y,col);
    //        }
    //    }
    //    guide.update();
    
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