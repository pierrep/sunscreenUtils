#include "ofApp.h"



//--------------------------------------------------------------
ofApp::~ofApp(){
    if (faceDetector) {
        delete faceDetector;
        faceDetector = NULL; 
    }
    
    if(camSdk == EDSDK) {
        edsdkGrabber.close();
        //edsdkGrabberUV.close();
    }
}

//--------------------------------------------------------------
void ofApp::setup(){
    
    // Setup the face detector and video grabbers for each `Mirror`.
    //
    // TODO: This should be updated once we discover the actual
    // device names for the cameras on each mirror.
    // Detector params.
    const unsigned long MAX_FACES = 2;
    const unsigned int FACE_MODE = FACE_MODE_LARGE_FACES; //FACE_MODE_SMALL_FACES;
    
    // Video grabber params.
    int rgbDeviceID = 0;

    camSdk = EDSDK;
    
    if(camSdk == EDSDK) {
        edsdkGrabber.setDeviceId(1);
        edsdkGrabber.setup();
        edsdkGrabberUV.setDeviceId(0);
        edsdkGrabberUV.setup();
    } else {
        rgbGrabber.listDevices();
        // rgbGrabber.setGrabber(std::make_shared<ofxPS3EyeGrabber>());

        rgbGrabber.setDeviceID(rgbDeviceID);
        rgbGrabber.setDesiredFrameRate(30);
        rgbGrabber.initGrabber(CAM_WIDTH, CAM_HEIGHT);
    }
    
    setupDetector(MAX_FACES, FACE_MODE);

    // Now that the grabbers and detectors are initialised,
    // we're ready to start detection.
    startFaceDetector();

}

//--------------------------------------------------------------
void ofApp::update(){
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    updateDetector();
}

//--------------------------------------------------------------
// Helper function for drawing an emotion value.
void drawEmotion(string emotion, float value, ofVec2f emotionPos, float dataPadding, int num) {
    const float MAX_WIDTH = 100.0;
    const float RECT_HEIGHT = 10.0;
    ofVec2f rectOffset = ofVec2f(0.0, -7.0);
    ofVec2f pos = emotionPos + ofVec2f(0.0, dataPadding * (float)num);
    ofSetColor(128);
    ofNoFill();
    ofDrawRectangle(pos + rectOffset, MAX_WIDTH, RECT_HEIGHT);
    ofSetColor(200, 100, 100);
    ofFill();
    ofDrawRectangle(pos + rectOffset, value * MAX_WIDTH / 100.0, RECT_HEIGHT);
    ofSetColor(255);
    ofDrawBitmapString(emotion, pos);
}

//--------------------------------------------------------------
// Only draw test GUI stuff in here.
//
// To draw to LED display, use `ofApp::drawLedWindow` method.
// To draw to Mirror displays, use `Mirror::draw` method.
void ofApp::draw(){

    ofBackground(0, 0, 0);
    ofSetColor(255);



    //ofDrawBitmapString("Mirror 1", pos.x, pos.y);
    
    // Draw the video grabbers.
    drawRGB();
    drawUV();
 
}

//--------------------------------------------------------------
void ofApp::drawRGB()
{
    float grabberWidth = 600;
    float ratio;
    float grabberHeight;
    
    // Display the input from the RGB and UV video grabbers
    // owned by each `Mirror` (just for testing and debugging).
    ofVec2f pos(20, 20);
    
    if(camSdk == EDSDK){
        ratio = grabberWidth / edsdkGrabber.getWidth();
        grabberHeight = edsdkGrabber.getHeight() * ratio;
    } else {
        ratio = grabberWidth / edsdkGrabber.getWidth();
        grabberHeight = edsdkGrabber.getHeight() * ratio;
    }
    
    rgbPos = pos + ofVec2f(0.0, 0.0);
    uvPos = rgbPos + ofVec2f(grabberWidth, 0.0);
    ofDrawBitmapString("RGB", rgbPos.x,rgbPos.y-5);
    if(camSdk == EDSDK) {
        edsdkGrabber.draw(rgbPos.x,rgbPos.y,grabberWidth, grabberHeight);
    } else {
        rgbGrabber.draw(rgbPos, grabberWidth, grabberHeight);
    }

    // Draw the face data.
    const float faceDataWidth = grabberWidth;
    const float dataPadding = 12.0;
    ofVec2f facePos = uvPos + ofVec2f(grabberWidth + 10.0, 0.0);
    typedef std::map<FaceId, DetectedFace>::iterator it_type;
    it_type begin = currentlyDetectedFaces.begin();
    it_type end = currentlyDetectedFaces.end();
    for (it_type it = begin; it != end; it++) {
        DetectedFace* face = &it->second;
        
        //cout << "ive found a face " << endl;
        
        // Draw the Face ID.
        ofDrawBitmapString("FACE " + ofToString(face->faceId), facePos);
        
        // Draw the face's bounding rectangle.
        FaceBounds bounds = face->faceBounds;
        ofNoFill();
        ofRectangle faceRect(rgbPos.x + bounds.x * ratio,
                             rgbPos.y + bounds.y * ratio,
                             bounds.width * ratio,
                             bounds.height * ratio);
        ofDrawRectangle(faceRect);
        ofDrawBitmapString(ofToString(face->faceId), faceRect.position);
        
        // Draw the detected Age.
        string ageRange;
        switch (face->appearance.age) {
            case AGE_UNDER_18: ageRange = "Under 18"; break;
            case AGE_18_24: ageRange = "18 to 24"; break;
            case AGE_25_34: ageRange = "25 to 34"; break;
                // Start taking decaded off.
            case AGE_35_44: ageRange = "25 to 34"; break;
            case AGE_45_54: ageRange = "35 to 44"; break;
            case AGE_55_64: ageRange = "45 to 54"; break;
            case AGE_65_PLUS: ageRange = "55 to 64"; break;
            default: ageRange = ""; break;
        }
        ofVec2f agePos = ofVec2f(ofGetWidth()-480,50);// facePos + ofVec2f(0.0, dataPadding);
        ofDrawBitmapString("Age = " + ageRange, agePos);
        
        // Draw the detected Ethnicity.
        string ethnicity;
        switch (face->appearance.ethnicity) {
            case ETHNICITY_UNKNOWN: ethnicity = "Unknown"; break;
            case ETHNICITY_CAUCASIAN: ethnicity = "Caucasian"; break;
            case ETHNICITY_BLACK_AFRICAN: ethnicity = "Black African"; break;
            case ETHNICITY_EAST_ASIAN: ethnicity = "East Asian"; break;
            case ETHNICITY_HISPANIC: ethnicity = "Hispanic"; break;
            case ETHNICITY_SOUTH_ASIAN: ethnicity = "Souht Asian"; break;
            default: ethnicity = ""; break;
        }
        ofVec2f ethnicityPos = agePos + ofVec2f(0.0, dataPadding);
        ofDrawBitmapString("Ethnicity = " + ethnicity, ethnicityPos);
        
        // Draw the detected Gender.
        string gender;
        switch (face->appearance.gender) {
            case GENDER_UNKNOWN: gender = "Unknown"; break;
            case GENDER_FEMALE: gender = "Female"; break;
            case GENDER_MALE: gender = "Male"; break;
            default: gender = ""; break;
        }
        ofVec2f genderPos = ethnicityPos + ofVec2f(0.0, dataPadding);
        ofDrawBitmapString("Gender = " + gender, genderPos);
        
        // Draw the detected Emotions (except for valence as it is -100 to 100).
        ofVec2f emotionPos = genderPos + ofVec2f(0.0, dataPadding + 2.0);
        drawEmotion("anger", face->emotions.anger, emotionPos, dataPadding, 1);
        drawEmotion("contempt", face->emotions.contempt, emotionPos, dataPadding, 2);
        drawEmotion("disgust", face->emotions.disgust, emotionPos, dataPadding, 3);
        drawEmotion("engagement", face->emotions.engagement, emotionPos, dataPadding, 4);
        drawEmotion("fear", face->emotions.fear, emotionPos, dataPadding, 5);
        drawEmotion("joy", face->emotions.joy, emotionPos, dataPadding, 6);
        drawEmotion("sadness", face->emotions.sadness, emotionPos, dataPadding, 7);
        drawEmotion("surprise", face->emotions.surprise, emotionPos, dataPadding, 8);
        
        // Bump the x pos across for the next face.
        facePos.x += faceDataWidth;
    }
    
    // Bump the y pos down for the next mirror.
    pos.y += grabberHeight + 50;
    
    //    ofTexture tex;
    //    ofPixels pixels = blackMagicGrabber.getColorPixels();
    //    tex.loadData(pixels);
    //    tex.draw(rgbPos, grabberWidth, grabberHeight);
    
}

//--------------------------------------------------------------
void ofApp::drawUV()
{
    float grabberWidth = 600;
    float ratio;
    float grabberHeight;

    if(camSdk == EDSDK){
        ratio = grabberWidth / edsdkGrabberUV.getWidth();
        grabberHeight = edsdkGrabberUV.getHeight() * ratio;
    }
    
    uvPos = rgbPos + ofVec2f(grabberWidth, 0.0);
    ofDrawBitmapString("UV", uvPos.x,uvPos.y-5);
    if(camSdk == EDSDK) {
        edsdkGrabberUV.draw(uvPos.x,uvPos.y,grabberWidth, grabberHeight);
    }
}


//--------------------------------------------------------------
void ofApp::setupDetector(unsigned long maximumFaces, unsigned int faceMode) {
    faceDetector = new FaceDetector();
    faceDetector->init(maximumFaces, faceMode);
}

//--------------------------------------------------------------
void ofApp::startFaceDetector() {
    faceDetector->startDetector();
}

//--------------------------------------------------------------
void ofApp::stopFaceDetector() {
    faceDetector->stopDetector();
}

//--------------------------------------------------------------
void ofApp::updateDetector() {
    
    // Update the video grabbers.
    if(camSdk == EDSDK) {
        edsdkGrabber.update();
        edsdkGrabberUV.update();
        
        // If we have a new frame, pass the pixels to the Detector for processing.
        if (edsdkGrabber.isFrameNew()) {
            ofPixels pixels = edsdkGrabber.getLivePixels();
            
            unsigned char* pixelData = pixels.getData();
            if (pixelData != NULL) {
                size_t width = pixels.getWidth();
                size_t height = pixels.getHeight();
                faceDetector->processPixels(pixelData, width, height);
            }
        }
        // If we have a new frame, pass the pixels to the Detector for processing.
        if (edsdkGrabberUV.isFrameNew()) {
            ofPixels pixels = edsdkGrabberUV.getLivePixels();
            
        }
    
    } else {
        rgbGrabber.update();
        
        // If we have a new frame, pass the pixels to the Detector for processing.
        if (rgbGrabber.isFrameNew()) {
            ofPixels pixels = rgbGrabber.getPixels();
            //        ofPixels pixels = preProcesser.process(rgbGrabber.getTexture());
            
            unsigned char* pixelData = pixels.getData();
            if (pixelData != NULL) {
                size_t width = pixels.getWidth();
                size_t height = pixels.getHeight();
                faceDetector->processPixels(pixelData, width, height);
            }
        }
    }
    

    
    // Update our map of currently visible faces.
    currentlyDetectedFaces = faceDetector->currentlyDetectedFaces();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

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
