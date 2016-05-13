#pragma once

#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxMidi.h"
#include "ofxPostGlitch.h"
#include "ofxGui.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"
#include "ofxEasyFft.h"
#include "ofxVideoRecorder.h"

// osc port setting
#define PORT 8007
//#define NUM_MSG_STRINGS 20


//class ofApp : public ofBaseApp{
class ofApp : public ofBaseApp, public ofxMidiListener {

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
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    //midi
    void exit();
    void newMidiMessage(ofxMidiMessage& eventArgs);
    
    //syphon
    ofxSyphonServer mainOutputSyphonServer;
    ofxSyphonServer individualTextureSyphonServer;
    ofxSyphonClient mClient;
    
    ofEasyCam cam;
    ofLight light;
    
//    ofBoxPrimitive box;
    
    // GUI„ÅÆ„Éë„É©„É°„Éº„Çø„Éº
    ofxPanel gui;
//    ofxFloatSlider tunnelDiff;
    ofxFloatSlider moveSpeed;
    ofxFloatSlider cameraDepth;
    ofxFloatSlider tunnelSpinSpeedX;
    ofxFloatSlider tunnelSpinSpeedY;
    ofxFloatSlider tunnelSpinSpeedZ;
//    ofxFloatSlider cameraSpinSpeedX;
//    ofxFloatSlider cameraSpinSpeedY;
//    ofxFloatSlider cameraSpinSpeedZ;
    float cameraSpinSpeedX;
    float cameraSpinSpeedY;
    float cameraSpinSpeedZ;
    ofxFloatSlider ambient;
    ofxFloatSlider diffuse;
    ofxFloatSlider specular;
//    ofxIntSlider lineWidth;
    ofxColorSlider color;
    ofxVec2Slider position;
    ofxToggle cameraRotateX;
    ofxToggle cameraRotateY;
    ofxToggle cameraRotateZ;
    ofxToggle tunnelRotateX;
    ofxToggle tunnelRotateY;
    ofxToggle tunnelRotateZ;

    //osc
    ofxOscReceiver receiver;
    
    /*Glitch*/
    ofFbo			myFbo;
    ofTexture		texture;
    ofxPostGlitch	myGlitch;
    
    //fft
    ofxEasyFft fft; // ofxEasyFftインスタンス
    
    ofImage myImage;
    
    //midi
    stringstream text;
    
    ofxMidiIn midiIn;
    ofxMidiIn midiIn2;
    ofxMidiMessage midiMessage;
    
};
