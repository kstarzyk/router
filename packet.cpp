#include "packet.h"
#include <string>
namespace Packet {
  std::tuple<std::string, int, int> decode(std::string packet) 
  {
    std::string ip = "";
    for (size_t i = 0; i < 3; i++) 
      ip += std::to_string( (packet[i] >= 0 ? packet[i] : 256+packet[i]) ) + ".";
    ip += std::to_string(packet[3]);

    int netmask = packet[4];
    unsigned int dist = (int)(packet[8]);
    dist += (int)(packet[7]) * 256;
    dist += (int)(packet[6]) * 256*256;
    dist += (int)(packet[5]) * 256*256*256;

    std::tuple<std::string, int, int> answer(ip, netmask, dist);
    std::cout << "DECODED " << packet << " INTO\nIP: " << ip << " NETMASK: " << netmask << " DISTANCE: " << dist << "\n"; 
    return answer;
  }

  std::string encode(const Network& network)
  {
    return encode(network.getWebAddress(), network.getNetmask(), network.getDistance());
    
  }
  std::string encode(std::string ip, char netmask, unsigned int dist)
  {
    auto octets = Network::OctetsFromIP(ip);
    std::string solution(" ", 9);

    for(size_t i = 0; i < 4; i++)
      solution[i] = octets[i];

    solution[4] = netmask;

    solution[5] = dist / (256*256*256);
    solution[6] = dist / (256*256);
    solution[7] = dist / 256;
    solution[8] = dist % 256;
    std::cout << "ENCODED IP: " << ip << " NETMASK: " << netmask << " DISTANCE: " << dist << " INTO ";
   
    for (int i=0; i<9; i++) std::cout << std::stoi(solution) << " "; 
    std::cout << "\n";
    return solution;
  }
}
