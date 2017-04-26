#include "packet.h"

namespace Packet {
  std::tuple<std::string, int, int> decode(std::string packet) 
  {
    std::string ip = "";
    for(size_t i = 0; i < 3; i++) ip += std::to_string( (packet[i] >= 0 ? packet[i] : 256+packet[i]) ) + ".";
    ip += std::to_string(packet[3]);
    int netmask = packet[4];
    unsigned int dist = ( (packet[6] >= 0 ? packet[6] : 256+packet[6]) ) + 256 * ( (packet[5] >= 0 ? packet[5] : 256+packet[5]) );
    std::tuple<std::string, int, int> answer(ip, netmask, dist);
    return answer;
  }

  std::string encode(const Network& network)
  {
    return Packet::encode(network.getWebAddress(), network.getNetmask(), network.getDistance());
    
  }
  std::string encode(std::string ip, char netmask, unsigned int dist)
  {
    auto octets = Network::OctetsFromIP(ip);
    std::string solution(" ", 7);

    for(size_t i = 0; i < 4; i++)
      solution[i] = octets[i];

    solution[4] = netmask;
    solution[5] = dist / 256;
    solution[6] = dist % 256;
    return solution;
  }
}
