#pragma once

#include "ofMain.h"
#include "ofxTrueTypeFontUC.h"
#include "ofxSyphon.h"
#include "ofxOsc.h"

#include "ofxOpenCv.h"
#include "ofxQuadWarp.h"
#define PORT 3000
#define tfNum 100

class TextFlow {
public:
    ofxTrueTypeFontUC font;
    ofPoint pos;
    ofPoint v;
    int width;
    int height;
    string text;
    string fontName;
    bool isEnd;
    TextFlow(){
        font.loadFont("MS Mincho.ttf", 15);
        v.x   = -3 - ofRandom(2);
        v.y   = 0;
        init();
        isEnd = true;
    }
    void init(){
        pos.x = width + 5;
        pos.y = ofRandom(1.) * (height-font.getFontSize()) + font.getFontSize();
        isEnd = false;
    }
    void load(string _fontName){
        font.loadFont(_fontName, 30);
        fontName = _fontName;
    }
    void setText(string str) {
        text = str;
    }
    ofPoint getPos(){
        return pos;
    }
    void update(){
        if(isEnd) return;
        pos += v;
        
        if(pos.x + font.stringWidth(text) < -30 ) isEnd = true;

    }
    void draw(){
        if(isEnd) return;
        ofSetColor(255);
        font.drawString(text, pos.x, pos.y);
        //ofSetColor(255,0,0);
        //ofNoFill();
        //font.drawString(text, pos.x, pos.y);
//        vector<ofPath> path = font.getStringAsPoints(text);
//        ofSetLineWidth(20);
//        for(ofPath p : path) {
//            auto line = p.getOutline();
//            for(auto l : line)
//                l.draw();
//        }
//        
    }
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
    void resetComment();
    ofxSyphonClient client;
    string serverName;
    string appName;
    
    ofxOscReceiver receiver;
    
    string fontName = "ヒラギノ角ゴシック W8.ttc";
    ofxTrueTypeFontUC font;
    TextFlow tf[tfNum];
    int setCount = 0;
    
    ofFbo fbo;
    ofxQuadWarp warper;
    
    bool isHide = false;
    bool isRemote = false;
    bool isCam    = true;
    bool showComment = true;
    
    ofVideoPlayer video;
    vector<ofVideoPlayer> videos;
    vector<string> videoFileName;
    int nowPlay;
    
    ofVideoGrabber cam;
    int degree;
    
    ofPoint offset, oldOffset;
    ofPoint pressed;
    float zoom = 1, oldZoom;
    
};
