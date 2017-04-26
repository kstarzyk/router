#ifndef _PACKET_H_
#define _PACKET_H_ 1

#include <tuple>
#include "network.h"

namespace Packet {
  std::tuple<std::string, int, int> decode(std::string packet);
  std::string encode(const Network&);
  std::string encode(std::string ip, int netmask, unsigned int dist);
}

#endif
