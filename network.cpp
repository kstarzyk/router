#include "network.h"
#include "constants.h"

#include <sstream>

std::array<int,4> Network::getIPOctets() const
{
  return this->ip_octets;
}

std::string Network::getIP() const
{
  return this->ip;
}

std::string Network::getCidr() const 
{
  return this->ip + '/' + std::to_string(this->netmask);
}

std::string Network::getVia() const
{
  return this->via;
}

void Network::setVia(const std::string& via) 
{
  if (!via.size())
    this->via = "";
  if (via == "unreachable")
    this->via = via;
  this->via = "via + " + via;
}

int Network::getNetmask() const
{
  return this->netmask;
}

int Network::getDistance() const
{
  return this->distance;
}

void Network::setDistance(int distance) {
  this->distance = distance;
}

std::string Network::getWebAddress() const
{
  return this->webAddress;
}

std::string Network::getBroadcast() const
{
  return this->broadcast;
}

int Network::getLifetime() const
{
  return this->lifetime;
}

void Network::setLifetime(int lifetime) 
{
  this->lifetime = lifetime;
}

void Network::update(const std::string& via, int distance, int lifetime) 
{
  setVia(via);
  setDistance(distance);
  setLifetime(lifetime);  
}

bool Network::isReachable() const
{
  return this->via != constants::UNREACHABLE;
}

void Network::pprint() const
{
  std::cout << "----------------------------------\n";
  std::cout << "IP = " << this->ip << "\n";
  std::cout << "VIA = " << this->via << "\n";
  std::cout << "NETMASK = " << this->netmask << "\n";
  std::cout << "DISTANCE = " << this->distance << "\n";
}

Network::Network() {
  this->ip = this->via = "";
  this->netmask = this->distance = this->lifetime = 0;
}

/**
 *  Input: 10.0.0.1/27 5
 */
Network::Network(std::string cidr, int distance) {
  this->ip = cidr.substr(0, cidr.find('/'));
  this->netmask = stoi(cidr.substr(cidr.find('/')+1, cidr.size()));
  this->via = "connected directly";
  this->distance = distance;
  this->lifetime = 0;
  this->ip_octets = Network::OctetsFromIP(ip);
  this->webAddress = GetWebAddress(ip_octets, netmask);
  this->broadcast = GetBroadcast(ip_octets, netmask);
}


std::array<int,4> Network::OctetsFromIP(std::string ip) 
{
  for(size_t i = 0; i < ip.size(); i++) 
    if(ip[i] == '.') ip[i] = ' ';

  std::array<int,4> octets;
  std::string response = "", temp;
  std::stringstream stream(ip);
  
  for(size_t i = 0; i < 4; i++) {
    stream >> temp;
    octets[i] = stoi(temp);
  }

  return octets;
}

std::string Network::GetWebAddress(std::string ip, int netmask) {
  auto arr = Network::OctetsFromIP(ip);
  return Network::GetWebAddress(arr, netmask);
}

std::string Network::GetWebAddress(std::array<int,4> octets, int netmask) {
  std::string response = "";
  for(size_t i = 0; i < 4; i++) {
    if(netmask > 8) netmask -= 8;
    else {
      octets[i] >>= (8-netmask);
      octets[i] <<= (8-netmask);
      netmask = 0;
    }
    response += std::to_string(octets[i]);
    if(i != 3) response += ".";
  }

  return response;
}

std::string Network::GetBroadcast(std::string ip, int netmask) {
  auto arr = Network::OctetsFromIP(ip);
  return Network::GetWebAddress(arr, netmask);
}

std::string Network::GetBroadcast(std::array<int,4> octets, int netmask) {
  int bitsTODO = 32-netmask;

  for(int i = 3; i >= 0; i--) {
    if(bitsTODO == 0) break;
    else if(bitsTODO >= 8) {
      octets[i] = 255;
      bitsTODO -= 8;
    }
    else {
      for(size_t j = 0; j < (size_t) bitsTODO; j++) {
        octets[i] |= (1 << j);
      }
      bitsTODO = 0;
    }
  }

  return "" + std::to_string(octets[0]) + "." + std::to_string(octets[1]) + "." + std::to_string(octets[2]) + "." + std::to_string(octets[3]);
}

