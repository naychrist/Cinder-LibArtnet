#pragma once

#include "artnet.h"
#include "packets.h"

/*
 todo:
 - test receiving
 - test other platforms than mac
 - optionally set multiple IPs by joining nodes to enable receiving from multiple devices
*/

namespace ciArtnet
{
    
class Universe;
    
class Node
{
public:

    Node();
    ~Node();

    void    setNumUniverses(int _num);
    void    setUniverseAtIndex(int _index, int _universe);
    bool    setup(std::string _ip_addr, bool _sendRaw = false, uint8_t _subnet_addr = 0);
    
    void    updateData(unsigned char * _data, int _length);
    void    updateDataByIndex(int _index, unsigned char * _data, int _length);
    void    updateDataByUniverse(int _universe, unsigned char * _data, int _length);

    int     getNumUniverses();
    void    send();
    void    close();
    
    
    //Callback for receiving:
    //should work as is if not sending raw, untested...
    
    int artnetReceiver(artnet_node node, void *pp);
    
    static int artnetReceiverWrapper(artnet_node node, void *pp, void *d) {
        Node* n = (Node*) d;
        return n->artnetReceiver(node, pp);
    };
    
    
private:
    
    artnet_node             artnetNode;
    std::string             ip_addr;
    uint8_t                 subnet_addr;
    bool                    sendRaw;
    std::vector<Universe>   unis;
    bool                    didSetup;
    
};//class Node


class Universe
{
    public:

    Universe(int port_id, uint8_t port_addr, int16_t length = 0, uint8_t *data = NULL) :
    port_id(port_id),
    port_addr(port_addr),
    length(length),
    data(data),
    updated(false)
    {}

    int         port_id;    // our id for uni port
    uint8_t     port_addr;  // uni port
    int16_t     length;     // length of DMX data array
    uint8_t     *data;      // DMX data array
    bool        updated;

};//class Universe

    
}//namespace ciArtnet

