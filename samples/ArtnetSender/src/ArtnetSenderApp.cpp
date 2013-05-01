#include "cinder/app/AppBasic.h"
#include "CinderLibArtnet.h"

using namespace ci;
using namespace ci::app;

class ArtnetSenderApp : public AppBasic {
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
    node.setup("10.0.0.10",true);//need to enable sendRaw if sending to multiple artnet devices
}


void ArtnetSenderApp::update()
{
    //create some test data
    int length = 512;
    unsigned char data[length];
    float val = (float)getMousePos().x/ getWindowWidth();
    for (int i=0; i<length; i++) data[i] = val * 255;
    
    gl::clear( Color( val, val, val ) );

    node.updateData(data, length);//if only using one universe
    //node.updateDataByIndex(index, data, length);
    //node.updateDataByUniverse(universe, data, length);

    node.send();
}

void ArtnetSenderApp::draw()
{
    
}

CINDER_APP_BASIC( ArtnetSenderApp, RendererGl )
