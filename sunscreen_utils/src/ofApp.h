#pragma once

#include "ofMain.h"
#include "Constants.h"
#include "FaceDetector.h"
#include "ofxEdsdk.h"

class RateTimer {
protected:
    float lastTick, averagePeriod, smoothing;
    bool secondTick;
public:
    RateTimer() :
    smoothing(.9) {
        reset();
    }
    void reset() {
        lastTick = 0, averagePeriod = 0, secondTick = false;
    }
    void setSmoothing(float smoothing) {
        this->smoothing = smoothing;
    }
    float getFramerate() {
        return averagePeriod == 0 ? 0 : 1 / averagePeriod;
    }
    void tick() {
        float curTick = ofGetElapsedTimef();
        if(lastTick == 0) {
            secondTick = true;
        } else {
            float curDiff = curTick - lastTick;;
            if(secondTick) {
                averagePeriod = curDiff;
                secondTick = false;
            } else {
                averagePeriod = ofLerp(curDiff, averagePeriod, smoothing);
            }
        }
        lastTick = curTick;
    }
};

class ofApp : public ofBaseApp {

	public:
    
        ~ofApp();

        //////////////////////
        ///// oF Methods /////
        //////////////////////

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


  

        //////////////////
        ///// Fields /////
        //////////////////

        // A handle to each window.
        shared_ptr<ofAppBaseWindow> mainWindow;

        // A `Mirror` for each mirror in the installation.
        //
        // Each mirror contains its own `ofAppBaseWindow`, `ofVideoGrabber`s
        // and list of currently visible `Person`s.
        //Mirror mirror;

    
        //------ ofVideoGrabber
        ofVideoGrabber rgbGrabber;
   
    
        //------ EDSDK camera
    	ofxEdsdk::Camera edsdkGrabber;
        ofxEdsdk::Camera edsdkGrabberUV;
    
        typedef enum CamSDK {BLACKMAGIC,EDSDK} SdkType;
        SdkType camSdk;
        RateTimer timer;
    
        FaceDetector * faceDetector;
    
        ofVec2f rgbPos;
        ofVec2f uvPos;

    // All faces that are currently visible to the detector.
    map<FaceId, DetectedFace> currentlyDetectedFaces;
    
    // Setup the Affectiva Detector.
    void setupDetector(unsigned long maximumFaces, unsigned int faceMode);
    
    // Start the face detection.
    void startFaceDetector();
    
    // Stop the face detection.
    void stopFaceDetector();
    
    // Update the state of the `Mirror`.
    void updateDetector();
    
    void drawRGB();
    void drawUV();

};
