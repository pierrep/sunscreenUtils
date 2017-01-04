#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    using namespace ps3eye;
    
    camFrameCount = 0;
    camFpsLastSampleFrame = 0;
    camFpsLastSampleTime = 0;
    camFps = 0;
    
    // list out the devices
    std::vector<PS3EYECam::PS3EYERef> devices( PS3EYECam::getDevices() );
    
    ofLogNotice() << "Num devices: " << devices.size() << endl;
    if(devices.size())
    {
        eye = devices.at(0);
        bool res = eye->init(640, 480, 60);
        eye->start();
        
        cout << "width = " << eye->getWidth() << endl;
        videoFrame 	= new unsigned char[eye->getWidth()*eye->getHeight()*3];
        //videoTexture.allocate(eye->getWidth(), eye->getHeight(), GL_BGR_EXT);
        videoTexture.allocate(eye->getWidth(), eye->getHeight(), GL_RGB);
    }
}
void ofApp::exit(){
    // You should stop before exiting
    // otherwise the app will keep working
    if(eye) eye->stop();
	delete[] videoFrame;
}

//--------------------------------------------------------------
void ofApp::update()
{
    if(eye)
    {
		eye->getFrame(videoFrame);
        videoTexture.loadData(videoFrame, eye->getWidth(),eye->getHeight(), GL_BGR_EXT);

        camFrameCount++;
        float timeNow = ofGetElapsedTimeMillis();
        if( timeNow > camFpsLastSampleTime + 1000 ) {
            uint32_t framesPassed = camFrameCount - camFpsLastSampleFrame;
            camFps = (float)(framesPassed / ((timeNow - camFpsLastSampleTime)*0.001f));
            
            camFpsLastSampleTime = timeNow;
            camFpsLastSampleFrame = camFrameCount;
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofSetHexColor(0xffffff);
    videoTexture.draw(0,0,eye->getWidth(),eye->getHeight());
    
    string str = "app fps: ";
	str += ofToString(ofGetFrameRate(), 2);
    str += "\neye fps: " + ofToString(camFps, 2);
    ofDrawBitmapString(str, 10, 15);
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
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
