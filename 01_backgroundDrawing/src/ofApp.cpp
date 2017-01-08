#include "ofApp.h"


bool compareLightness( const Color& a, const Color& b ) {
    return b.color.getLightness() < a.color.getLightness();
}


void ofApp::loadPalette(string path){
    colors.clear();
    ofBuffer buffer = ofBufferFromFile(path);
    for(auto& line : buffer.getLines()){
        vector<string> columns = ofSplitString(line, ",");
        // remove table header and ignore empty lines
        if(columns[0] != "R"  &&  line.size()) {
            Color c;
            c.color.r = ofToInt(columns[0]);
            c.color.g = ofToInt(columns[1]);
            c.color.b = ofToInt(columns[2]);
            c.weight = ofToFloat(columns[3]);
            colors.push_back(c);
        }
    }
    // sort colors
    ofSort(colors, compareLightness);
    // assign weights (now based on lightness);
    float remaining = 1;
    for(auto& color : colors){
        color.weight = remaining * 0.3;
        remaining -= color.weight;
    }
    // testing
    // remove the darkest color
    colors.pop_back();
    
    // preview
    float w = 20;
    float h = 200;
    float y = 0;
    colorsPreview.allocate(w, h);
    colorsPreview.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    colorsPreview.begin();
    {
        ofClear(0);
        for(int i = 0; i < colors.size(); i++){
            Color& c = colors[i];
            cout << c.color << endl;
            ofSetColor(c.color);
            ofDrawRectangle(0, y, w, h*c.weight);
            y += h*c.weight;
        }
    }
    colorsPreview.end();
    // pixels lookup
    colorsPreview.readToPixels(colorsPalette);
    
    
    reset();
}


void ofApp::setup(){
    ofBackground(100);
    ofEnableAlphaBlending();
    
    // load mask
    ofImage src("poster_mask_tarzan2.png");
    // transform brightness into opacity (black pixels will be transparent)
    mask.allocate(src.getWidth(), src.getHeight(), OF_IMAGE_COLOR_ALPHA);
    const ofPixels& pix = src.getPixels();
    for(int y=0; y<pix.getHeight(); y++){
        for(int x=0; x<pix.getWidth(); x++){
            ofColor c = pix.getColor(x, y);
            c.set(backColor, c.r);
            mask.getPixels().setColor(x, y, c);
        }
    }
    mask.update();
    
    // create canvas
    canvas.allocate(mask.getWidth(), mask.getHeight(), GL_RGB8);
    
    // create gui
    gui.setup();
    gui.add(numBrushes.set("num brushes", 5, 1, 50));
    gui.add(brushOpacity.set("brush opacity", 5, 1, 255));
    gui.add(brushSpeed.set("brush speed", 2, 1, 10));
    gui.add(bDrawMask.set("draw mask", true));
    gui.add(bClearCanvas.set("clear canvas", false));
    gui.add(buttonReset.setup("reset"));
    gui.setPosition(canvas.getWidth() + 50, 20);
    buttonReset.addListener(this, &ofApp::reset);
    
    // kick it off
    loadPalette("color_tarzan_avg.csv");
    reset();
}


void ofApp::reset(){
    // begin from the top
    bounds.set(0, 0, canvas.getWidth(), 100);
    
    brushes.clear();
    for (int i = 0; i < numBrushes; i++) {
        //int colorIndex = ofRandom(colors.size());
        //Brush brush(colors[colorIndex].color, bounds);
        
        // hack to avoid black color
        ofColor color(0,0,0,0);
        while(color.a == 0){
            int rand = ofRandom( colorsPalette.getHeight() );
            color = colorsPalette.getColor(10, rand); // the tallest the color, the highest the chance
        }
        
        Brush brush(color, bounds);
        brush.opacity = brushOpacity;
        brushes.push_back(brush);
    }
    
    canvas.begin();
    ofClear(backColor);
    canvas.end();
}


void ofApp::update(){
    for(auto& brush : brushes){
        brush.opacity = brushOpacity;
    }
    
    // testing
    if (bounds.height < canvas.getHeight())
        bounds.height += 2;
}


void ofApp::draw(){
    canvas.begin();
    {
//        if(bounds.height < canvas.getHeight()){
        if(bClearCanvas){
            ofClear(backColor);
        }
        for(auto& brush : brushes) {
            brush.paint(bounds);
        }
//        }
    }
    canvas.end();
    canvas.draw(0,0);
    
    if(bDrawMask){
        mask.draw(0,0);
    }
    
    colorsPreview.draw(canvas.getWidth()+20, 20);
    
//    ofPushStyle();
//        ofNoFill();
//        ofSetColor(ofColor::red);
//        ofDrawRectangle(bounds);
//    ofPopStyle();
    
    gui.draw();
}


void ofApp::keyPressed(int key){
    switch(key){
        case 'f':
            ofToggleFullscreen();
            break;
        default:
            break;
    }
}


void ofApp::keyReleased(int key){

}


void ofApp::dragEvent(ofDragInfo dragInfo){ 
    if (dragInfo.files.size() == 1){
        string path = dragInfo.files[0];
        
        ofFile file(path);
        if (file.isDirectory()){
            return;
        }
        
        loadPalette(path);
        reset();
    } else {
        ofLog(OF_LOG_ERROR, "drop only one file at a time");
    }
}
