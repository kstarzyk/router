#ifndef _SOCKET_H_
#define _SOCKET_H_ 1

#include "network.h"


namespace Socket {

  sockaddr_in Init(int &sockfd, Network &network, int port)
  {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
      std::cerr << "Socket creation exception.\n";
      throw new std::runtime_error("Socket creation exception.");
    }

    struct sockaddr_in server_address;
    bzero (&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    inet_pton(AF_INET, Network::GetBroadcast(network.getIPOctets(), network.getNetmask()).c_str(), &server_address.sin_addr);
    int broadcastPermission = 1;
    setsockopt (sockfd, SOL_SOCKET, SO_BROADCAST, (void *)&broadcastPermission, sizeof(broadcastPermission));

    std::cout << "IP = " << network.getIP() << " NETMASK = " << network.getNetmask() << " BROADCAST = " << Network::GetBroadcast(network.getIPOctets(), network.getNetmask()) << "\n";

    if (bind (sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
      std::cerr << "Bind error : " << strerror(errno) << "\n";
      throw new std::runtime_error("Bind error."); 
    }

    return server_address;
  }

  std::tuple<std::string, std::string> Receive(int &sockdf)
  {
    struct sockaddr_in sender; 
    socklen_t sender_len = sizeof(sender);
    char buffer[IP_MAXPACKET+1];
    ssize_t datagram_len = recvfrom (sockdf, buffer, IP_MAXPACKET, MSG_DONTWAIT , (struct sockaddr*)&sender, &sender_len);
    if (datagram_len < 0) 
    {
      datagram_len = 0;
    }

    char sender_ip[20];
    inet_ntop(AF_INET, &(sender.sin_addr), sender_ip, sizeof(sender_ip));

    return std::make_pair(std::string(sender_ip), std::string(buffer));
  }

  int SendTo(int &target, std::string msg, sockaddr_in addr)
  {
    return sendto(target, msg.c_str(), msg.size(), 0, (struct sockaddr*) &addr, sizeof(addr)) != (int) msg.size();
  }

}


#endif
