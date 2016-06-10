#include "cinder/app/App.h"
#include "cinder/CinderMath.h"
#include "CinderLibArtnet.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;

class ArtnetSenderApp : public App {
public:
	void setup();
	void update();
	void draw();

    ciArtnet::Node    node;
};

void ArtnetSenderApp::setup()
{
    //node.setNumUniverses(num_universes);//optional, setup() will create a single universe 1 at index 0 otherwise
    //node.setUniverseAtIndex(index,universe);//optional, otherwise universes start at 1
    node.setup("192.168.0.10",true);//need to enable sendRaw if sending to multiple artnet devices
	//"192.168.42.182"
}


void ArtnetSenderApp::update()
{
    //create some test data
    const int length = 512;
    unsigned char data[length];
    float val = math<float>::clamp( (float)getMousePos().x/ getWindowWidth() -1 );
    for (int i=0; i<length; i++) data[i] = val * 255;
    
    gl::clear( Color( val, val, val ) );

    node.updateData(data, length);//if only using one universe
    //node.updateDataByIndex(0, data, length);
    //node.updateDataByUniverse(universe, data, length);

    node.send();
}

void ArtnetSenderApp::draw()
{
    
}

CINDER_APP( ArtnetSenderApp, RendererGl )
