#include "router.h"
#include "socket.h"
#include "constants.h"
#include "timer.h"

Router::Router() 
{
  lifetime = 0;
}

Router::~Router() 
{
  for(size_t i = 0; i < sockets.size(); i++) close(sockets[i]);
}

Router Router::initFromIO() 
{
  auto r = Router();
  std::cin >> r.numberOfNetworks; 
  std::cin.ignore();
  r.sockets.resize(r.numberOfNetworks);

  for(size_t i = 0; i < r.numberOfNetworks; i++) 
  {
    std::string cidr, keyword;
    int distance;
    std::cin >> cidr >> keyword >> distance; 
    r.neighbours.emplace_back(cidr, distance);
    std::cout << "ADDED " << cidr << " " << keyword << " " << distance << std::endl;
    std::cout << r.neighbours[i].getWebAddress() << std::endl;
    r.distanceVector[r.neighbours[i].getWebAddress()] = r.neighbours[i];
  }
  std::cout << "INITIALIZED FROM IO " << std::endl;
  return r;
}


// Example :
//10.0.0.0/8 unreachable
//192.168.2.0/24 distance 4 via 192.168.5.5
// 192.168.5.0/24 distance 2 connected directly

void Router::printDistanceVector() const
{
  std::cout << " _________________\n| DISTANCE VECTOR |\n--------------------------------------\n";
  for (auto it = distanceVector.begin(); it!=distanceVector.end(); ++it) 
  {
    auto currentNetwork = it->second;
    if (!currentNetwork.isReachable() || 
        abs(currentNetwork.getLifetime() - lifetime) >= constants::DELETE_TIMEOUT )
      continue;
    else if(!currentNetwork.isReachable() || 
            abs(currentNetwork.getLifetime() - lifetime) >= constants::UNREACHABLE_TIMEOUT )
      std::cout << currentNetwork.getCidr() << " unreachable, received at lifetime " << currentNetwork.getLifetime() << "\n";
    else {
      std::cout << currentNetwork.getCidr() << " distance " << currentNetwork.getDistance() << ", " << currentNetwork.getVia() << " received at lifetime " << currentNetwork.getLifetime() << "\n";
    }
  }
  std::cout << "--------------------------------------\n\n";
}
/**
 * Removes timeouted (after DELETE_TIMEOUT) networks
 */
void Router::clean() 
{
  std::vector<std::string> toDelete;
  for (auto it = distanceVector.begin(); it!=distanceVector.end(); ++it) 
  {
    if (!it->second.isReachable() && abs(it->second.getLifetime() - lifetime) >= constants::DELETE_TIMEOUT )
      toDelete.push_back(it->second.getWebAddress());
    else if (!it->second.isReachable() &&
              abs(it->second.getLifetime() - lifetime) >= constants::UNREACHABLE_TIMEOUT ) 
      it->second.setDistance(constants::INF);
  }

  for (size_t i = 0; i < toDelete.size(); i++)
    distanceVector.erase(toDelete[i]);
}

std::string Router::stringifyToPackets() const
{
  std::string response = "";
  for (auto it = distanceVector.begin(); it!=distanceVector.end(); ++it)
    response += Packet::encode(it->second);
  return response;
}

void Router::propagate() {
  std::string message = stringifyToPackets();

  for (size_t i = 0; i < neighbours.size(); i++) 
  {
    auto currentWebAddress = neighbours[i].getWebAddress();
    if (Socket::SendTo(sockets[i], message, &addresses[i])) 
    {
      distanceVector[currentWebAddress].update(constants::UNREACHABLE, constants::INF, lifetime);
    } else {
      auto iter = distanceVector.find(currentWebAddress);

      if (iter == distanceVector.end() || 
          distanceVector[currentWebAddress].getDistance() >= neighbours[i].getDistance() ) 
      {
        distanceVector[currentWebAddress] = neighbours[i];
        distanceVector[currentWebAddress].setLifetime(lifetime);
      }
    }
  }
}

void Router::receiveAndUpdate() 
{
  for(size_t i = 0; i < neighbours.size(); i++) 
  {
    auto msg = Socket::Receive(sockets[i]);
    std::string senderIP = std::get<0>(msg);
    std::string buffer = std::get<1>(msg);
    if (!senderIP.size()) continue;
    for(size_t j = 0; j < buffer.size(); j += 7) 
    {
      std::string str_packet = "";
      for(size_t k = 0; k < 7; k++) str_packet += buffer[j+k];
      std::tuple<std::string, int, int> packet = Packet::decode(str_packet);
      update(senderIP, std::get<0>(packet), std::get<1>(packet), std::get<2>(packet));
      std::cout << " GET : IP =" << std::get<0>(packet) << " NETMASK = " << std::get<1>(packet) << " DIST = " << std::get<2>(packet) << "\n";
      //printf("GET : IP = %s NETMASK = %s DIST = %s \n", std::get<0>(packet).c_str(), std::get<1>(packet).c_str(), std::get<2>(packet).c_str());
    }
  }
}


void Router::updateSenderLifetime(std::string senderIP)
{
  for (size_t i = 0; i < neighbours.size(); i++) 
  {
    if (senderIP == neighbours[i].getIP()) 
    {
      distanceVector[Network::GetWebAddress(senderIP, neighbours[i].getNetmask())].setLifetime(lifetime);
      return;
    }
  }
}

void Router::update(std::string senderIP, std::string ip, int netmask, int distance)
{
  auto it = distanceVector.find( Network::GetWebAddress(ip, netmask) );
  auto iter = distanceVector.end();
  int senderNetmask = 32;

  updateSenderLifetime(senderIP);

  // find sender netmask
  while (iter == distanceVector.end()) 
  {
    iter = distanceVector.find( Network::GetWebAddress(senderIP, senderNetmask) );
    senderNetmask--;
  }

  int senderDistance = iter->second.getDistance();
  
  if (it != distanceVector.end()) 
  {
    if (it->second.getDistance() >= (distance + senderDistance))
      it->second.setLifetime(lifetime);
    
    if (it->second.getDistance() > (distance + senderDistance)) 
    {
      it->second.setDistance(distance + senderDistance);
      it->second.setVia(senderIP);
    }
  }
  else {
    distanceVector[Network::GetWebAddress(ip, netmask)].update(senderIP, distance+senderDistance, lifetime);
//    Network n(ip, senderIP, netmask, distance + senderDistance, lifetime);
//    distanceVector.erase(Network::GetWebAddress(ip, netmask));
  }

}


void Router::listen(int port) 
{
  std::cout << "Listen on port " << port << "\n";
  try {
    for (size_t i=0;i<sockets.size();i++)
      addresses.push_back(Socket::Init(sockets[i], neighbours[i], port));

    printDistanceVector();
    clean();
    while(++lifetime) 
    {
      std::cout << "Lifetime: " << lifetime << "\n";
      propagate();
      auto start = Timer::now();
      while (Timer::wait(start,5*1000)) 
        receiveAndUpdate();
      printDistanceVector();
      clean();
    }
  } catch(...) {
    std::cerr << "Program terminated.\n";
  }
}
