#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);

    
    receiver.setup(PORT);
    client.setup();
    serverName = "";
    appName    = "TCPSyphonClient";
    client.set(serverName, appName);
    
    
    fbo.allocate(640, 480);
    warper.setup();
    ofRectangle rect = ofRectangle(0, 0, fbo.getWidth(), fbo.getHeight());
    warper.setSourceRect(rect);
    warper.setTargetRect(rect);
    warper.load();
    warper.hide();
    
    keyPressed('l');
    
    //tf is text flow.
    for(int i=0; i<tfNum; i++){
        tf[i].load(fontName);
        ofLog() << tf[i].font.getFontSize();
        tf[i].setText("");
        tf[i].width = fbo.getWidth();
        tf[i].height = fbo.getHeight();
    }
    
    //video
    video.load("差し込み映像1.mp4");
    video.setLoopState(OF_LOOP_NONE);
    videoFileName.push_back("差し込み映像1.mp4");
    
    videos.resize(videoFileName.size());
    for(int i = 0; i<videoFileName.size(); i++) {
        videos[i].load(videoFileName[i]);
        videos[i].setLoopState(OF_LOOP_NONE);
        
    }
    nowPlay = -1;
    
    //cam
    cam.setDeviceID(0);
    cam.initGrabber(fbo.getWidth(), fbo.getHeight());
    
    degree = 360;
}

//--------------------------------------------------------------
void ofApp::update(){
    for(int i=0; i<tfNum; i++){
        tf[i].update();
    }
    //osc
	while(receiver.hasWaitingMessages()){
		ofxOscMessage m;
		receiver.getNextMessage(m);
		if(m.getAddress() == "/niconico/text"){
			tf[setCount].setText(m.getArgAsString(0));
            tf[setCount].init();
            setCount++;
            if(setCount >= tfNum) setCount=0;
		}
        if(m.getAddress() == "/niconico/syphon/appName"){
            appName = m.getArgAsString(0);
            client.set(serverName, appName);
        }
        if(m.getAddress() == "/niconico/isHide"){
            int arg = m.getArgAsInt(0);
            if(arg == 1) isHide = true;
            if(arg == 0) isHide = false;
        }
        if(m.getAddress() == "/niconico/movie"){
            int arg = m.getArgAsInt(0);
            
            if(arg < 0) arg == 0;
            if(arg > videoFileName.size()) continue;
            
            if(nowPlay >= 0) videos[nowPlay].stop();
            nowPlay = arg-1;
            //block douga sukosi osoku
            if(nowPlay == 3) videos[nowPlay].setFrame(200);
            
            if(nowPlay >= 0) videos[nowPlay].play();
            
        }
        if(m.getAddress() == "/niconico/remotecam"){
            int arg = m.getArgAsInt(0);
            if(arg == 1) isRemote = true;
            if(arg == 0) isRemote = false;
        }
        if(m.getAddress() == "/niconico/rotate"){
            degree = 0;
        }
        if(m.getAddress() == "/niconico/cam"){
            int arg = m.getArgAsInt(0);
            if(arg == 1) isCam = true;
            if(arg == 0) isCam = false;
        }
        if(m.getAddress() == "/niconico/showComment"){
            int arg = m.getArgAsInt(0);
            if(arg == 1) showComment = true;
            if(arg == 0) showComment = false;
        }
        if(m.getAddress() == "/niconico/resetComment"){
            int arg = m.getArgAsInt(0);
            resetComment();
        }
	}
    
    warper.update();
    for(auto &v:videos) v.update();
    cam.update();
    if(degree < 360)    degree += 3;
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if(isHide) return;
    
    fbo.begin();
    ofClear(0);
    ofPushMatrix();
    ofTranslate(fbo.getWidth()/2, fbo.getHeight()/2);
    ofRotate(degree);
    ofTranslate(-fbo.getWidth()/2, -fbo.getHeight()/2);

    ofPushMatrix();
    ofTranslate(fbo.getWidth()/2, fbo.getHeight()/2);
    ofScale(zoom, zoom);
    ofTranslate(-fbo.getWidth()/2, -fbo.getHeight()/2);
    if(isCam) cam.draw(offset);
    ofPopMatrix();
    
    if(isRemote) client.draw(0, 0, fbo.getWidth()  ,fbo.getHeight());
    if(nowPlay >= 0 && videos[nowPlay].isFrameNew()) videos[nowPlay].draw(0, 0, fbo.getWidth(), fbo.getHeight());
    
    ofPopMatrix();
    
    
    ofSetColor(255);
    if(showComment){
        for(int i=0; i<tfNum; i++) tf[i].draw();
    }
    fbo.end();
    
    if(warper.isShowing()){
        ofDrawBitmapStringHighlight("send string at /niconico/text via osc localhost 3000", 20,20);
        ofDrawBitmapStringHighlight("width:  " + ofToString(client.getWidth()),  20,40);
        ofDrawBitmapStringHighlight("height: " + ofToString(client.getHeight()), 20,60);
    }
    warper.draw();
    
    ofMultMatrix(warper.getMatrix());
    fbo.draw(0,0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == ' '){
        warper.toggleShow();
    }
    if(key == 's') {
        warper.save();
        ofXml xml;
        xml.addChild("setting");
        xml.setTo("setting");
        xml.addChild("offset");
        xml.setTo("offset");
        xml.setAttribute("x", ofToString(offset.x));
        xml.setAttribute("y", ofToString(offset.y));
        xml.setToParent();
        xml.addChild("zoom");
        xml.setTo("zoom");
        xml.setValue("/", ofToString(zoom));
        //xml.setValue(ofToString(zoom));
        xml.save("setting.xml");
    }
    if(key == 'l') {
        warper.load();
        ofXml xml;
        if(!xml.load("setting.xml")) return;
        xml.setTo("setting");
        xml.setTo("offset");
        offset.x = ofToInt(xml.getAttribute("x"));
        offset.y = ofToInt(xml.getAttribute("y"));
        xml.setToParent();
        xml.setTo("zoom");
        zoom = ofToFloat(xml.getValue());
        
    }
    if(key == 'f') ofToggleFullscreen();
    
    
    if(key == 'J') offset.x--;
    if(key == 'L') offset.x++;
    if(key == 'I') offset.y--;
    if(key == 'K') offset.y++;
    if(key == 'U') zoom-=0.1;
    if(key == 'O') zoom+=0.1;
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    ofPoint drag;
    if(ofGetKeyPressed(OF_KEY_LEFT_SHIFT)) {
        drag.x = x;
        drag.y = y;
        if(button == 0) offset = oldOffset + drag - pressed;
        if(button == 2) zoom = oldZoom + (drag.y - pressed.y)/100;
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if(ofGetKeyPressed(OF_KEY_LEFT_SHIFT)){
        oldOffset = offset;
        oldZoom = zoom;
        pressed.x = x;
        pressed.y = y;
    }

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::resetComment(){
    for(int i=0; i<tfNum; i++){
        tf[i].setText("");
        tf[i].pos.x = - 100;
    }

}
