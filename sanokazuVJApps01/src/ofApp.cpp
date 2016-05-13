#include "ofApp.h"

int i;
int colorR[100];
int colorG[100];
int colorB[100];
int colorMaintain = 500;

float oscFader1[10];
float oscFader2[10];
float oscFader3[10];
float oscFader4[10];
float oscXY[2];
Boolean oscToggle1[10];
Boolean oscToggle2[10];
Boolean oscToggle3[10];

float tunnelDiff;
float tunnelDiffStart = 100;
int tunnelNum = 50;
float keyDiff = 0;
float forMove = 0;
float moveSpeedStart = 20;
float cameraDepthStart = -1000;
int lineWidthStart = 5;
float lineWidth[100];
float lineZ[100];
float cameraRotateParamX, cameraRotateParamY, cameraRotateParamZ;
float cameraSpinSpeedParamX, cameraSpinSpeedParamY, cameraSpinSpeedParamZ;
float tunnelRotateParamX, tunnelRotateParamY, tunnelRotateParamZ;
float cameraSpinSpeedStart = 1;
float tunnelSpinSpeedStart = 0.05;

Boolean cameraSwitch = false;
float camDist = 100;

//midi
float midiParam[30];
float midiToggle[20];
Boolean midiFlag[20];
Boolean tunnelDiffAuto;
Boolean tunnelZAuto;
Boolean camDistAuto;
Boolean tunnelDiffBigger;
float TDBParam;
float autoSize;
float scaledVol;

float alpha;
Boolean reset;

Boolean glitchToggle;
float glitchThreshold[10];

float phase;

//--------------------------------------------------------------
void ofApp::setup(){
    
    //ウィンドウ初期位置
    ofSetWindowPosition(1441,0);
//    //フルスクリーンにする
//    ofSetFullscreen(true);
    
    //    ofBackground(0,0,0);
    ofSetBackgroundAuto(false);
    ofEnableAlphaBlending();
//    ofEnableDepthTest();
    ofSetFrameRate(60);
    
    
        //ÂÖâ
        light.enable();
        light.setPointLight();
        light.setPosition(ofGetWidth()/2, ofGetHeight()/2, cameraDepth);

    
    //Ëâ≤Ë®≠ÂÆö
    for(i=0; i<tunnelNum; i++) {
        colorR[i] = int(ofRandom(0,255));
        colorG[i] = int(ofRandom(0,colorMaintain-colorR[0]));
        if(colorMaintain - colorR[i] - colorG[i] > 255){
            colorB[i] = int(ofRandom(0,colorMaintain-colorR[i]-colorG[i]));
        } else {
            colorB[i] = colorMaintain - colorR[i] - colorG[i];
        }
    }
    
    
    cam.setDistance(100);
    
    //syphon
    mainOutputSyphonServer.setName("Screen Outputh");
    mClient.setup();
    mClient.setApplicationName("Simple Serverh");
    mClient.setServerName("");
    
    

    
    
    ofxGuiSetFont("Helvetica.ttf",10,true,true);
    ofxGuiSetTextPadding(4);
    ofxGuiSetTextColor(255);
    ofxGuiSetDefaultWidth(300);
    ofxGuiSetDefaultHeight(18);
    

    // GUI„ÇíË®≠ÂÆö
    gui.setup();
//    gui.add(tunnelDiff.setup("tunnelDiff", tunnelDiffStart, 0, 200));
    gui.add(moveSpeed.setup("moveSpeed", moveSpeedStart, 0, 100));
    gui.add(cameraDepth.setup("cameraDepth", cameraDepthStart, -10000, 0));
    gui.add(ambient.setup("ambient", 1.0, 0, 1.0));
    gui.add(diffuse.setup("diffuse", 1.0, 0, 1.0));
    gui.add(specular.setup("specular", 1.0, 0, 1.0));
//    gui.add(lineWidth.setup("lineWidth", lineWidthStart, 1, 20));
    //    gui.add(position.setup("position", initPos, minPos, maxPos));
    gui.add(cameraRotateX.setup("cameraRotateX", false));
    gui.add(cameraRotateY.setup("cameraRotateY", false));
    gui.add(cameraRotateZ.setup("cameraRotateZ", false));
    gui.add(tunnelRotateX.setup("tunnelRotateX", false));
    gui.add(tunnelRotateY.setup("tunnelRotateY", false));
    gui.add(tunnelRotateZ.setup("tunnelRotateZ", false));
//    gui.add(cameraSpinSpeedX.setup("cameraSpinSpeedX", cameraSpinSpeedStart, -1, 1));
//    gui.add(cameraSpinSpeedY.setup("cameraSpinSpeedY", cameraSpinSpeedStart, -1, 1));
//    gui.add(cameraSpinSpeedZ.setup("cameraSpinSpeedZ", cameraSpinSpeedStart, -1, 1));
    gui.add(tunnelSpinSpeedX.setup("tunnelSpinSpeedX", tunnelSpinSpeedStart, -0.2, 0.2));
    gui.add(tunnelSpinSpeedY.setup("tunnelSpinSpeedY", tunnelSpinSpeedStart, -0.2, 0.2));
    gui.add(tunnelSpinSpeedZ.setup("tunnelSpinSpeedZ", tunnelSpinSpeedStart, -0.2, 0.2));
    
    //osc
    cout << "listening for oscmessages on port " << PORT << "\n";
    receiver.setup(PORT);
    
    //fft
    fft.setup(pow(2.0, 10.0));
    
    //glitch
    myFbo.allocate(ofGetWidth(), ofGetHeight());
    myGlitch.setup(&myFbo);
    
    //midi
    ofSetLogLevel(OF_LOG_VERBOSE);
    midiIn.listPorts();
    midiIn.openPort(0);
    midiIn2.openPort(1);
    midiIn.ignoreTypes(false, false, false);
    midiIn2.ignoreTypes(false, false, false);
    midiIn.addListener(this);
    midiIn2.addListener(this);
    midiIn.setVerbose(true);
    midiIn2.setVerbose(true);
    
    for(i=0;i<10;i++){
        glitchThreshold[i] = 1;
    }
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    //fft
    fft.update();
    vector<float> buffer;
    buffer = fft.getBins();
    
    
    phase += 0.01;
    
    
    //osc
    while(receiver.hasWaitingMessages()){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(m);
        
        for(i=0;i<6;i++){
            if(m.getAddress() == "/page1/fader"+ofToString(i+1)){
                oscFader1[i] = m.getArgAsFloat(0);
            }
            if(m.getAddress() == "/page1/toggle"+ofToString(i+1)){
                oscToggle1[i] = m.getArgAsBool(0);
            }
        }
        
        for(i=0;i<4;i++){
            if(m.getAddress() == "/page2/fader"+ofToString(i+1)){
                oscFader2[i] = m.getArgAsFloat(0);
            }
            if(m.getAddress() == "/page2/toggle"+ofToString(i+1)){
                oscToggle2[i] = m.getArgAsBool(0);
            }
        }
        
        for(i=0;i<5;i++){
            if(m.getAddress() == "/page3/fader"+ofToString(i+1)){
                oscFader3[i] = m.getArgAsFloat(0);
            }
            if(m.getAddress() == "/page3/toggle"+ofToString(i+1)){
                oscToggle3[i] = m.getArgAsBool(0);
            }
        }
        
        for(i=0;i<10;i++){
            if(m.getAddress() == "/page4/fader"+ofToString(i+1)){
                oscFader4[i] = m.getArgAsFloat(0);
            }
//            if(m.getAddress() == "/page4/toggle"+ofToString(i+1)){
//                oscToggle4[i] = m.getArgAsBool(0);
//            }
        }
        
        if(m.getAddress() == "/page2/fader1"){
            oscFader1[2] = m.getArgAsFloat(0);
        }

        if(m.getAddress() == "/page2/xy1"){
            oscXY[0] = m.getArgAsFloat(0);
            oscXY[1] = m.getArgAsFloat(1);
        }
    }
    
    cameraRotateParamX = ofMap(oscXY[0], 0, 1, -180, 180);
    cameraRotateParamY = ofMap(oscXY[1], 0, 1, -180, 180);
    cameraRotateParamZ = ofMap(oscFader1[2], 0, 1, -180, 180);
    cameraSpinSpeedX = ofMap(oscFader1[0], 0, 1, -1, 1);
    cameraSpinSpeedY = ofMap(oscFader1[1], 0, 1, -1, 1);
    tunnelSpinSpeedX = ofMap(oscFader1[3], 0, 1, -0.1, 0.1);
    tunnelSpinSpeedY = ofMap(oscFader1[4], 0, 1, -0.1, 0.1);
    tunnelSpinSpeedZ = ofMap(oscFader1[5], 0, 1, -0.1, 0.1);
    cameraSpinSpeedZ = ofMap(oscFader2[0], 0, 1, -180, 180);
    tunnelDiff = ofMap(oscFader2[1], 0, 1, 0, 200);
    moveSpeed = ofMap(oscFader2[2], 0, 1, 0, 100);
    cameraDepth = ofMap(oscFader2[3], 0, 1, 0, -10000);
    ambient = ofMap(oscFader3[0], 0, 1, 0, 1);
    diffuse = ofMap(oscFader3[1], 0, 1, 0, 1);
    specular = ofMap(oscFader3[2], 0, 1, 0, 1);
    camDist = ofMap(oscFader3[3], 0, 1, 0, 5000);
    alpha = ofMap(oscFader3[4], 0, 1, 0, 255);
    
    for(i=0; i<10; i++){
        glitchThreshold[i] = ofMap(oscFader4[i], 0, 1, 0, 1);
    }
    
    cameraRotateX = oscToggle1[0];
    cameraRotateY = oscToggle1[1];
    cameraRotateZ = oscToggle2[0];
    tunnelRotateX = oscToggle1[3];
    tunnelRotateY = oscToggle1[4];
    tunnelRotateZ = oscToggle1[5];
    
    cameraSwitch = oscToggle3[0];
    
    if(glitchToggle == true){
        for(i=0;i<8;i++){
            if(midiMessage.control == i) midiParam[i+20] = midiMessage.value;
        }
    } else {
        for (i=0;i<8;i++){
            if(midiMessage.control == i) midiParam[i] = midiMessage.value;
        }
    }
    for (i=16;i<24;i++){
        if(midiMessage.control == i) midiParam[8+i-16] = midiMessage.value;
    }
    for (i=32;i<40;i++){
        if(midiMessage.control == i) {
            if(midiMessage.value == 127)  midiToggle[i-32] = 1;
            else midiToggle[i-32] = 0;
        }
    }
    if(midiMessage.control == 71) { //camera
        if(midiMessage.value == 127)  midiToggle[8] = 1;
        else midiToggle[8] = 0;
    }
    if(midiMessage.control == 64) { //glitch
        if(midiMessage.value == 127)  midiToggle[9] = 1;
        else midiToggle[9] = 0;
    }
    if(midiMessage.control == 43) { //reset
        if(midiMessage.value == 127)  midiToggle[10] = 1;
        else midiToggle[10] = 0;
    }
    if(midiMessage.control == 48) { //tunnelDiffAuto
        if(midiMessage.value == 127)  midiToggle[11] = 1;
        else midiToggle[11] = 0;
    }
    if(midiMessage.control == 52) { //tunnelZAuto
        if(midiMessage.value == 127)  midiToggle[12] = 1;
        else midiToggle[12] = 0;
    }
    if(midiMessage.control == 55) { //camDistAuto
        if(midiMessage.value == 127)  midiToggle[13] = 1;
        else midiToggle[13] = 0;
    }
    if(midiMessage.control == 38) { //tunnelDiffBigger
        if(midiMessage.value == 127)  midiToggle[14] = 1;
        else midiToggle[14] = 0;
    }
    
    
    //midi assign
    
    cameraRotateX = midiToggle[0];
    cameraRotateY = midiToggle[1];
    //    cameraRotateZ = midiToggle[2];
    tunnelRotateX = midiToggle[2];
    tunnelRotateY = midiToggle[3];
    tunnelRotateZ = midiToggle[4];
    
    cameraSwitch = midiToggle[8];
    glitchToggle = midiToggle[9];
    reset = midiToggle[10];
    tunnelDiffAuto = midiToggle[11];
    tunnelZAuto = midiToggle[12];
    camDistAuto = midiToggle[13];
    tunnelDiffBigger = midiToggle[14];
    
    //    cameraRotateParamX = ofMap(midiParam[0], 0, 127, -180, 180);
    //    cameraRotateParamY = ofMap(midiParam[1], 0, 127, -180, 180);
    //    cameraRotateParamZ = ofMap(midiParam[2], 0, 127, -180, 180);
    cameraSpinSpeedX = ofMap(midiParam[0], 0, 127, 1, -1);
    cameraSpinSpeedY = ofMap(midiParam[1], 0, 127, -1, 1);
    tunnelSpinSpeedX = ofMap(midiParam[2], 0, 127, -0.1, 0.1);
    tunnelSpinSpeedY = ofMap(midiParam[3], 0, 127, -0.1, 0.1);
    tunnelSpinSpeedZ = ofMap(midiParam[4], 0, 127, -0.1, 0.1);
    camDist = ofMap(midiParam[7], 0, 127, 0, 5000);
    
    for(i=0;i<8;i++){
        glitchThreshold[i] = ofMap(midiParam[i+20], 0, 127, 0, 1);
    }
    
    
    tunnelDiff = ofMap(midiParam[8], 0, 127, 0, 200);
    moveSpeed = ofMap(midiParam[9], 0, 127, 0, 100);
    cameraDepth = ofMap(midiParam[10], 0, 127, -10000, 0);
    ambient = ofMap(midiParam[11], 0, 127, 0, 1);
//    diffuse = ofMap(midiParam[12], 0, 127, 0, 1);
//    specular = ofMap(midiParam[13], 0, 127, 0, 1);
    diffuse = 0;
    specular = 0;
    alpha = ofMap(midiParam[15], 0, 127, 0, 128);
    
    autoSize = ofMap(midiParam[14], 0, 127, 0, 1);

    
    if(reset == true){
        cameraRotateParamX=0;
        cameraSpinSpeedParamX = 0;
        cameraRotateParamY = 0;
        cameraSpinSpeedParamY = 0;
        cameraRotateParamZ = 0;
        cameraSpinSpeedParamZ = 0;
        tunnelRotateParamX = 0;
        tunnelRotateParamY = 0;
        tunnelRotateParamZ = 0;
        tunnelSpinSpeedX = 0;
        tunnelSpinSpeedY = 0;
        tunnelSpinSpeedZ = 0;
        tunnelDiff = 0;
    }

    if(tunnelDiffAuto == true) {
        tunnelDiff = ofMap(buffer[10], 0, 1, 0, 200*midiParam[8]/127);
    } else {
        if(tunnelDiffBigger == true) {
            tunnelDiff = TDBParam;
            TDBParam += 2;
        }
    }
    if(tunnelZAuto == true) {
        tunnelRotateParamZ = ofMap(buffer[20], 0, 1, 0, 360*midiParam[4]/127);
    }
    if(camDistAuto == true) {
//        camDist = ofMap(buffer[10], 0, 1, 0, 5000*autoSize);
                scaledVol = 0;
                for(i=0;i<10;i++){
                    scaledVol += buffer[i] / 10;
                }
                camDist = ofMap(scaledVol, 0, 0.6, 0, 5000*midiParam[7]/127);
    }
//    if(tunnelDiffBigger != tunnelDiffBigger) {
//        tunnelDiff=0;
//    }
    if(tunnelDiffBigger == false) {
        TDBParam = 0;
    }


    //Glitch
    if (buffer[10] > glitchThreshold[0]) {
        myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE	, true);
    } else {
        myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE	, false);
    }
//    if (buffer[20] > glitchThreshold[1]) {
//        myGlitch.setFx(OFXPOSTGLITCH_GLOW			, true);
//    } else {
//        myGlitch.setFx(OFXPOSTGLITCH_GLOW			, false);
//    }
    if (buffer[40] > glitchThreshold[1]) {
        myGlitch.setFx(OFXPOSTGLITCH_SHAKER			, true);
    } else {
        myGlitch.setFx(OFXPOSTGLITCH_SHAKER			, false);
    }
    if (buffer[60] > glitchThreshold[2]) {
        myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER		, true);
    } else {
        myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER		, false);
    }
    if (buffer[80] > glitchThreshold[3]) {
        myGlitch.setFx(OFXPOSTGLITCH_TWIST			, true);
    } else {
        myGlitch.setFx(OFXPOSTGLITCH_TWIST			, false);
    }
//    if (buffer[100] > glitchThreshold[5]) {
//        myGlitch.setFx(OFXPOSTGLITCH_OUTLINE		, true);
//    } else {
//        myGlitch.setFx(OFXPOSTGLITCH_OUTLINE		, false);
//    }
    if (buffer[120] > glitchThreshold[4]) {
        myGlitch.setFx(OFXPOSTGLITCH_NOISE			, true);
    } else {
        myGlitch.setFx(OFXPOSTGLITCH_NOISE			, false);
    }
    if (buffer[140] > glitchThreshold[5]) {
        myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN		, true);
    } else {
        myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN		, false);
    }
    if (buffer[160] > glitchThreshold[6]) {
        myGlitch.setFx(OFXPOSTGLITCH_SWELL			, true);
    } else {
        myGlitch.setFx(OFXPOSTGLITCH_SWELL			, false);
    }
    if (buffer[180] > glitchThreshold[7]) {
        myGlitch.setFx(OFXPOSTGLITCH_INVERT			, true);
    } else {
        myGlitch.setFx(OFXPOSTGLITCH_INVERT			, false);
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    

    
    myFbo.begin();
    ofBackground(0, 0, 0);
//    ofBackground(255, 255, 255);

    
    //Áí∞Â¢ÉÂèçÂ∞ÑÂÖâ
    light.setAmbientColor(ofFloatColor(ambient,ambient,ambient,ambient));
    //Êã°Êï£ÂèçÂ∞ÑÂÖâ
    light.setDiffuseColor(ofFloatColor(diffuse,diffuse,diffuse));
    //Èè°Èù¢ÂèçÂ∞ÑÂÖâ
    light.setSpecularColor(ofFloatColor(specular,specular,specular));

    
    //„Ç´„É°„É©
//    cameraDepth = 200 - keyDiff;

    
    
    //ÂàùÊúü‰ΩçÁΩÆ
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2, 0);
//    ofDrawCircle(0, 0, 10);
    
//    ofSetColor(255);
//    ofDrawBitmapString("scaledVol : "+ofToString(scaledVol), 0, 0);
    
    if (cameraSwitch == true) {
        //cameraSwitchがついているとき 原点中心に回る
//        ofTranslate(cameraDepth, 0, 0);
        

//        ofTranslate(0, 0, tunnelDiff*tunnelNum/2);
        cam.setPosition(0, 0, camDist);
        cam.lookAt(ofVec3f(0,0,0));
        cam.begin();
//        ofTranslate(0, 0, -tunnelDiff*tunnelNum/2);
        
//        ofTranslate(-cameraDepth, 0, 0);
        
    } else {
        //cameraSwitchがついていないとき トンネルの中から見る
        ofTranslate(0, 0, cameraDepth);

        cam.setPosition(0, 0, 0);
        cam.begin();
    }

    ofRotateX(cameraRotateParamX + cameraSpinSpeedParamX);
    ofRotateY(cameraRotateParamY + cameraSpinSpeedParamY);
    ofRotateZ(cameraRotateParamZ + cameraSpinSpeedParamZ);
    if(cameraRotateX == true) {
        if (cameraSpinSpeedX >0) {
            cameraSpinSpeedParamX += cameraSpinSpeedX*cameraSpinSpeedX;
        } else {
            cameraSpinSpeedParamX -= cameraSpinSpeedX*cameraSpinSpeedX;
        }
    }
    if(cameraRotateY == true) {
        if (cameraSpinSpeedY >0) {
            cameraSpinSpeedParamY += cameraSpinSpeedY*cameraSpinSpeedY;
        } else {
            cameraSpinSpeedParamY -= cameraSpinSpeedY*cameraSpinSpeedY;
        }
    }
    if(cameraRotateZ == true) {
        if (cameraSpinSpeedZ >0) {
            cameraSpinSpeedParamZ += cameraSpinSpeedZ*cameraSpinSpeedZ;
        } else {
            cameraSpinSpeedParamZ -= cameraSpinSpeedZ*cameraSpinSpeedZ;
        }
    }

    if (cameraSwitch != true) {
        ofTranslate(0, 0, -cameraDepth);
    }
    
    
    //    ofSetColor(255, 255, 0);
    ofNoFill();

    
 
    
    
    //Á∑ö„ÅÆÊèèÁîª
    for (i=0; i<tunnelNum; i++){
        
        ofSetColor(colorR[i], colorG[i], colorB[i]);
        
        //Á∑ö„ÅÆÂ§™„Åï„ÇíÂ§â„Åà„Çã
        lineZ[i] = -tunnelDiff * tunnelNum + i * tunnelDiff + forMove;
        
      lineWidth[i] = ofMap(abs(lineZ[i] - cameraDepth), 0, 500, 8, 1);
        
//        if (lineZ[i] - cameraDepth < 1000 && lineZ[i] - cameraDepth > -1000 ) {
//            lineWidth[i] = 1000;
//        } else {
//            lineWidth[i] = 1;
//        }
        
        ofSetLineWidth(lineWidth[i]);
//        ofSetLineWidth(lineWidth);
        
        //Á∑ö„ÅÆÊèèÁîª
        ofDrawBox(0, 0, lineZ[i], ofGetWidth()*0.2, ofGetHeight()*0.2, 1);
        if(i>0) {
            ofDrawLine(-ofGetWidth()*0.2, -ofGetHeight()*0.2, lineZ[i], -ofGetWidth()*0.2, -ofGetHeight()*0.2, lineZ[i-1]);
            ofDrawLine(ofGetWidth()*0.2, -ofGetHeight()*0.2, lineZ[i], ofGetWidth()*0.2, -ofGetHeight()*0.2, lineZ[i-1]);
            ofDrawLine(-ofGetWidth()*0.2, ofGetHeight()*0.2, lineZ[i], -ofGetWidth()*0.2, ofGetHeight()*0.2, lineZ[i-1]);
            ofDrawLine(ofGetWidth()*0.2, ofGetHeight()*0.2, lineZ[i], ofGetWidth()*0.2, ofGetHeight()*0.2, lineZ[i-1]);
            
            if(tunnelRotateX == true) {
                ofRotateX(tunnelRotateParamX);
                //二乗で変化量をゆっくりに
                if(tunnelSpinSpeedX>0){
                    tunnelRotateParamX += tunnelSpinSpeedX*tunnelSpinSpeedX;
                } else {
                    tunnelRotateParamX -= tunnelSpinSpeedX*tunnelSpinSpeedX;
                }
            }
            if(tunnelRotateY == true) {
                ofRotateY(tunnelRotateParamY);
                if(tunnelSpinSpeedY>0){
                    tunnelRotateParamY += tunnelSpinSpeedY*tunnelSpinSpeedY;
                } else {
                    tunnelRotateParamY -= tunnelSpinSpeedY*tunnelSpinSpeedY;
                }
            }
            if(tunnelRotateZ == true || tunnelZAuto == true
               ) {
                ofRotateZ(tunnelRotateParamZ);
                if(tunnelSpinSpeedZ>0){
                    tunnelRotateParamZ += tunnelSpinSpeedZ*tunnelSpinSpeedZ;
                } else {
                    tunnelRotateParamZ -= tunnelSpinSpeedZ*tunnelSpinSpeedZ;
                }
            }
        }
    }
    

    
    //„Éà„É≥„Éç„É´„ÇíÈÄ≤„ÇÄ
    if(forMove < tunnelDiff) {
        forMove += moveSpeed;
    } else {
        forMove = 0;
        
        //Ëâ≤„ÅÆÂèó„ÅëÊ∏°„Åó
        for (i=0; i<tunnelNum; i++) {
            colorR[tunnelNum-i] = colorR[tunnelNum-i-1];
            colorG[tunnelNum-i] = colorG[tunnelNum-i-1];
            colorB[tunnelNum-i] = colorB[tunnelNum-i-1];
        }
        //Ëâ≤„ÅÆÂÜçÁîüÊàê
        colorR[0] = int(ofRandom(0,255));
        colorG[0] = int(ofRandom(0,colorMaintain-colorR[0]));
        if(colorMaintain - colorR[0] - colorG[0] > 255){
            colorB[0] = int(ofRandom(0,colorMaintain-colorR[0]-colorG[0]));
        } else {
            colorB[0] = colorMaintain - colorR[0] - colorG[0];
        }
    }

    cam.end();
    
    myFbo.end();


    
//    //GUI
//    ofSetColor(255, 255, 255);
//    gui.draw();
    
    
//    glitch
    myGlitch.generateFx();
    ofSetColor(255, 255, 255, alpha);
    myFbo.draw(0, 0);

    
    //syphon
        mClient.draw(50, 50);
        mainOutputSyphonServer.publishScreen();
    

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == '1') myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE	, true);
    if (key == '2') myGlitch.setFx(OFXPOSTGLITCH_GLOW			, true);
    if (key == '3') myGlitch.setFx(OFXPOSTGLITCH_SHAKER			, true);
    if (key == '4') myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER		, true);
    if (key == '5') myGlitch.setFx(OFXPOSTGLITCH_TWIST			, true);
    if (key == '6') myGlitch.setFx(OFXPOSTGLITCH_OUTLINE		, true);
    if (key == '7') myGlitch.setFx(OFXPOSTGLITCH_NOISE			, true);
    if (key == '8') myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN		, true);
    if (key == '9') myGlitch.setFx(OFXPOSTGLITCH_SWELL			, true);
    if (key == '0') myGlitch.setFx(OFXPOSTGLITCH_INVERT			, true);
    if(key == 'x'){
        //保存
        myImage.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
        myImage.saveImage("graphic.jpg", OF_IMAGE_QUALITY_BEST);
    }
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == '1') myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE	, false);
    if (key == '2') myGlitch.setFx(OFXPOSTGLITCH_GLOW			, false);
    if (key == '3') myGlitch.setFx(OFXPOSTGLITCH_SHAKER			, false);
    if (key == '4') myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER		, false);
    if (key == '5') myGlitch.setFx(OFXPOSTGLITCH_TWIST			, false);
    if (key == '6') myGlitch.setFx(OFXPOSTGLITCH_OUTLINE		, false);
    if (key == '7') myGlitch.setFx(OFXPOSTGLITCH_NOISE			, false);
    if (key == '8') myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN		, false);
    if (key == '9') myGlitch.setFx(OFXPOSTGLITCH_SWELL			, false);
    if (key == '0') myGlitch.setFx(OFXPOSTGLITCH_INVERT			, false);

    
}
//--------------------------------------------------------------
void ofApp::exit() {
    
    //midi
    midiIn.closePort();
    midiIn.removeListener(this);

}

//----------midi----------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {
    
    // make a copy of the latest message
    midiMessage = msg;
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
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
