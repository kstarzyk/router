#ifndef _NETWORK_H_
#define _NETWORK_H_ 1

#include <array>
#include <iostream>

class Network
{
  public:
    Network(std::string cidr, int distance);
    Network();
    void pprint() const;

    void update(const std::string& via, int distance, int lifetime);

    std::string getIP() const;
    std::string getCidr() const;
    std::array<int,4> getIPOctets() const;
    std::string getVia() const;
    int getNetmask() const;
    int getDistance() const;
    int getLifetime() const;
    bool isReachable() const;
    std::string getWebAddress() const;
    std::string getBroadcast() const;

    void setVia(const std::string&);
    void setDistance(int);
    void setLifetime(int);

    std::string static GetWebAddress(std::string, int);
    std::string static GetWebAddress(std::array<int,4>, int);

    std::string static GetBroadcast(std::string, int);
    std::string static GetBroadcast(std::array<int,4>, int);

    std::array<int,4> static OctetsFromIP(std::string);

  private:
    std::string ip, via, webAddress, broadcast;
    int distance, netmask, lifetime;
    std::array<int,4> ip_octets;
};

#endif
