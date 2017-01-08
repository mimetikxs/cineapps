#include "ofApp.h"

using namespace cv;


// http://stackoverflow.com/questions/10167534/how-to-find-out-what-type-of-a-mat-object-is-with-mattype-in-opencv
string type2str(int type) {
    string r;
    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);
    switch ( depth ) {
        case CV_8U:  r = "8U"; break;
        case CV_8S:  r = "8S"; break;
        case CV_16U: r = "16U"; break;
        case CV_16S: r = "16S"; break;
        case CV_32S: r = "32S"; break;
        case CV_32F: r = "32F"; break;
        case CV_64F: r = "64F"; break;
        default:     r = "User"; break;
    }
    r += "C";
    r += (chans+'0');
    return r;
}


bool compareWeight( const Color& a, const Color& b ) {
    return a.weight < b.weight;
}


bool compareSaturation( const Color& a, const Color& b ) {
    return a.color.getSaturation() < b.color.getSaturation();
}

bool compareHue( const Color& a, const Color& b ) {
    return a.color.getHue() < b.color.getHue();
}

bool compareBrightness( const Color& a, const Color& b ) {
    return a.color.getBrightness() < b.color.getBrightness();
}

bool compareLightness( const Color& a, const Color& b ) {
    return a.color.getLightness() < b.color.getLightness();
}

bool compareR( const Color& a, const Color& b ) {
    return a.color.r < b.color.r;
}

bool compareB( const Color& a, const Color& b ) {
    return a.color.b < b.color.b;
}

bool compareG( const Color& a, const Color& b ) {
    return a.color.g < b.color.g;
}



void ofApp::extractColors(string path){
    image.load(path);
    
    Mat src = ofxCv::toCv(image);
    //cvtColor(src, src, CV_RGB2BGR); // flip channels (oCV is BGR)
    
    // color clustering with opencv k-means
    // http://stackoverflow.com/questions/10240912/input-matrix-to-opencv-kmeans-clustering/10242156#10242156
    
    // one row per sample:
    // reshape the source matrix manually
    // we can also use Mat::reshape()
    Mat samples(src.rows * src.cols, 3, CV_32F);
    for( int y = 0; y < src.rows; y++ )
        for( int x = 0; x < src.cols; x++ )
            for( int z = 0; z < 3; z++)
                samples.at<float>(y + x*src.rows, z) = src.at<Vec3b>(y,x)[z];
    
    // run kmeans
    int clusterCount = numColors;
    Mat labels;
    int attempts = 5;
    Mat centers;
    kmeans(samples, clusterCount, labels, TermCriteria(), attempts, KMEANS_RANDOM_CENTERS, centers );
    
    //
    //    Mat new_image( src.size(), src.type() );
    //    for( int y = 0; y < src.rows; y++ ){
    //        for( int x = 0; x < src.cols; x++ )
    //        {
    //            int cluster_idx = labels.at<int>(y + x*src.rows,0);
    //            new_image.at<Vec3b>(y,x)[0] = centers.at<float>(cluster_idx, 0);
    //            new_image.at<Vec3b>(y,x)[1] = centers.at<float>(cluster_idx, 1);
    //            new_image.at<Vec3b>(y,x)[2] = centers.at<float>(cluster_idx, 2);
    //        }
    //    }
    //    imshow( "clustered image", new_image );
    
    //
    // cluster histogram
    //
    
    // number of bins
    int histSize = clusterCount;
    
    // ranges
    float maxRange = clusterCount;
    float range[] = { 0, maxRange } ;
    const float* histRange = { range };
    
    bool uniform = true; bool accumulate = false;
    
    // calculate histogram
    labels.convertTo(labels, CV_8U);
    
    Mat hist;
    calcHist( &labels, 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate );
    
    // populate color objects
    colors.clear();
    
    float numPixels = samples.rows; // 1 sample = 1 pixel
    for (int bin = 0; bin < histSize; bin++) {
        Color color;
        color.color.r = centers.at<float>(bin, 0);
        color.color.g = centers.at<float>(bin, 1);
        color.color.b = centers.at<float>(bin, 2);
        color.weight = hist.at<float>(bin) / numPixels;
        colors.push_back(color);
    }
    
    ofSort(colors, compareWeight);
}


void ofApp::setup(){
    gui.setup();
    gui.add(numColors.set("number of colors", 8, 1, 20));
    gui.add(buttonRefresh.setup("refresh"));
    gui.add(buttonSaveData.setup("save data"));
    gui.setPosition(20, 50);
    
    buttonSaveData.addListener(this, &ofApp::saveData);
    buttonRefresh.addListener(this, &ofApp::onRefreshClicked);
    
    imagePath = "";
}


void ofApp::update(){
    
}


void ofApp::draw(){
    ofDrawBitmapStringHighlight("drop an image", 20, 20, ofColor::red);
    
    image.draw(0,0);
    
    float x = 0;
    float y = image.getHeight() + 20;
    float w = ofClamp(image.getWidth(), 0, 800);
    float h = 80;
    ofPushStyle();
    for(int i = 0; i < colors.size(); i++) {
        float rectWidth = colors[i].weight * w;
        ofSetColor(colors[i].color);
        ofDrawRectangle(x, y, rectWidth, h);
        x += rectWidth;
    }
    ofPopStyle();
    
    gui.draw();
}


void ofApp::saveImage(){
    float x = 0;
    float y = 0;
    float w = ofClamp(image.getWidth(), 0, 800);
    float h = 80;
    ofFbo fbo;
    fbo.allocate(w, h);
    fbo.begin();
    {
        for(int i = 0; i < colors.size(); i++) {
            float rectWidth = colors[i].weight * w;
            ofSetColor(colors[i].color);
            ofDrawRectangle(x, y, rectWidth, h);
            x += rectWidth;
        }
    }
    fbo.end();
    
    ofPixels pix;
    fbo.readToPixels(pix);
    ofSaveImage(pix, "");
}


void ofApp::saveData(){
    string pathToData = ""; // points to data folder
    string filePath = pathToData + "colors.csv";
    ofFile file(filePath, ofFile::WriteOnly); // file does not exists
    file.create(); // now file exists
    // write the table header
    file << "R" << "," << "G" << "," << "B" << "," << "weight" << endl;
    for(int i=0; i < colors.size(); i++){
        ofColor c = colors[i].color;
        string r = ofToString((int)c.r);
        string g = ofToString((int)c.g);
        string b = ofToString((int)c.b);
        string weight = ofToString(colors[i].weight);
        file << r << "," << g << "," << b << "," << weight << endl;
    }
    file.close();
}


void ofApp::onRefreshClicked(){
    if(imagePath == "")
        return;
    extractColors(imagePath);
}


void ofApp::keyPressed(int key){
    switch(key){
        case 'f':
            ofToggleFullscreen();
            break;
        case 's':
            saveImage();
            saveData();
            break;
        default:
            break;
    }
}


void ofApp::keyReleased(int key){

}


void ofApp::mouseMoved(int x, int y ){

}


void ofApp::mouseDragged(int x, int y, int button){

}


void ofApp::mousePressed(int x, int y, int button){

}


void ofApp::mouseReleased(int x, int y, int button){

}


void ofApp::mouseEntered(int x, int y){

}


void ofApp::mouseExited(int x, int y){

}


void ofApp::windowResized(int w, int h){

}


void ofApp::gotMessage(ofMessage msg){

}


void ofApp::dragEvent(ofDragInfo dragInfo){ 
    if (dragInfo.files.size() == 1){
        imagePath = dragInfo.files[0];
        
        // ignore directories
        ofFile file(imagePath);
        if (file.isDirectory()){
            return;
        }
        
        // get movie file name
        //clipName = ofSplitString(path, "/").back();
        
        extractColors(imagePath);
    } else {
        ofLog(OF_LOG_ERROR, "drop only one file at a time");
    }
}
