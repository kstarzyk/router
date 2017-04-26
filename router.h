#ifndef _ROUTER_H_
#define _ROUTER_H_ 1


#include <map>
#include <vector>
#include <cstring>

#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include "network.h"
#include "packet.h"


class Router
{
  public:
    Router static initFromIO();
    void printDistanceVector() const;
    void listen(int port);


    Router();
   ~Router();

  private:
    std::vector<Network> neighbours;
    std::vector<int> sockets;
    std::vector<struct sockaddr_in> addresses;
    std::map<std::string, Network> distanceVector;
    
    void propagate();
    void receiveAndUpdate();

    void setVia(const std::string&);
    void setDistance(int distance);

    size_t numberOfNetworks;
    int lifetime;

    std::string stringifyToPackets() const;

    void clean();
    void update(std::string senderIP, std::string ip, int netmask, int distance);
    void updateSenderLifetime(std::string senderIP);
};

#endif
