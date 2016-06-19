#include "CinderLibArtnet.h"
#include "cinder/Log.h"

namespace ciArtnet {

Node::Node() : didSetup( false ) {}

Node::~Node() {} //should call close manually, quits uncleanly if quitting with no connection
     
void Node::setNumUniverses(int _num)
{
    
    if (unis.size()) {
		CI_LOG_E("Error: Must call addUniverses() only once and before setup().");
        return;
    }
    
    for (int i=0; i<_num; i++) {
        unis.push_back( Universe(i,i+1) );
    }
    
}

void Node::setUniverseAtIndex(int _index, int _universe)
{
    if (!didSetup) {
        if (_index < unis.size() ) {
            unis[_index].port_addr = _universe;
        }
        else {
			CI_LOG_E("No universe at that index.");
            return;
        }
    }
    else {
		CI_LOG_E("Must change universe numbers before setup().");
        return;
    }

}

bool Node::setup(std::string _ip_addr, bool _sendRaw, uint8_t _subnet_addr)
{
    ip_addr = _ip_addr;
    subnet_addr = _subnet_addr;
    sendRaw = _sendRaw;
    
    artnetNode  = artnet_new(ip_addr.c_str(), 0);//int = verbose
    //@todo: allow for multiple nodes then artnet_join(artnetNode1, artnetNodeN)
    
    if (!artnetNode) {
		CI_LOG_E(""<<artnet_strerror());
        return false;
    }
    
    artnet_set_long_name(artnetNode, "Cinder-LibArtnet");
    artnet_set_short_name(artnetNode, "CI");
    if (sendRaw) artnet_set_node_type(artnetNode, ARTNET_RAW);
    
    // set the upper 4 bits of the universe address 
    artnet_set_subnet_addr(artnetNode, subnet_addr) ;
    
    if (!unis.size()) {
        setNumUniverses(1);
		CI_LOG_I("Creating single universe with address 1 and ID 0.");
    }
    
    for (int i=0; i<unis.size(); i++) {
        // enable port        
        artnet_set_port_type(artnetNode, unis[i].port_id, ARTNET_ENABLE_OUTPUT, ARTNET_PORT_DMX);
        // bind port to universe
        artnet_set_port_addr(artnetNode, unis[i].port_id, ARTNET_OUTPUT_PORT, unis[i].port_addr); 
    }
    
    artnet_dump_config(artnetNode);
    
    //set receiver callback
    artnet_set_handler(artnetNode, ARTNET_RECV_HANDLER, artnetReceiverWrapper, this); 
    
    if (artnet_start(artnetNode) != 0) {
		CI_LOG_E(""<< artnet_strerror());
        return false;
    }

    didSetup = true;
    return true;
    
};

void Node::updateData(unsigned char * _data, int _length)
{
	if (!didSetup) return;

    unis[0].data = _data;
    unis[0].length = _length;
    unis[0].updated = true;
};

void Node::updateDataByIndex(int _index, unsigned char * _data, int _length)
{
	if (!didSetup) return;

    if (_index < unis.size()) {
        unis[_index].data = _data;
        unis[_index].length = _length;
        unis[_index].updated = true;
    }
};

void Node::updateDataByUniverse(int _universe, unsigned char * _data, int _length)
{
	if (!didSetup) return;

    for (int i=0; i<unis.size(); i++) {
        if (unis[i].port_addr == _universe) {
            unis[i].data = _data;
            unis[i].length = _length;
            unis[i].updated = true;
            break;
        }
    }
};

int Node::getNumUniverses()
{
    return unis.size();
};

void Node::send()
{
	if (!didSetup) return;
    //artnet_send_poll(artnetNode, NULL, ARTNET_TTM_DEFAULT);//to test receiving...
    //printf("arnet_get_sd() => %i\n", artnet_get_sd(artnetNode));
    //printf("artnet_read() => %i\n", artnet_read(artnetNode, 1));//works but slows down thread
    
    for (int i=0; i<unis.size(); i++) {
        if (unis[i].updated) {
            if (!sendRaw) {
                artnet_send_dmx(artnetNode, unis[i].port_id, unis[i].length, unis[i].data);
            } else {
                artnet_raw_send_dmx(artnetNode, unis[i].port_addr, unis[i].length, unis[i].data);
            }
            unis[i].updated = false;
        }
    }
};

void Node::close()
{
    // deallocate memory
    artnet_stop(artnetNode);
    artnet_destroy(artnetNode);
    didSetup = false;
    unis.clear();
};

int Node::artnetReceiver(artnet_node node, void *pp)
{
	CI_LOG_I("Receiving Art-Net data!");
    artnet_packet pack = (artnet_packet) pp;
	CI_LOG_I("Received packet sequence " << pack->data.admx.sequence);
	CI_LOG_I("Received packet type " << pack->type);
	CI_LOG_I("Received packet data " << pack->data.admx.data);
    return 0;
};

}//namespace ciArtnet



