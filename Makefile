Flags = -std=c++11 -Wundef -Wall -W -Wextra -g -Werror -Wreturn-type -pedantic -pedantic-errors -O2
CPP = g++

router : Makefile router.o packet.o network.o 
	$(CPP) $(Flags) main.cpp -o router router.o packet.o network.o 

router.o : Makefile router.cpp packet.o network.o
	$(CPP) -c $(Flags) router.cpp -o  router.o

network.o : Makefile network.cpp network.h packet.o
	$(CPP) -c $(Flags) network.cpp -o network.o	

packet.o : Makefile packet.cpp packet.h 
	$(CPP) -c $(Flags) packet.cpp -o packet.o	

clean:
	rm -rf *.o

distclean:
	rm -rf *.o router .*.swp
