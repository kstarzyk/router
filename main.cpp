#include "router.h"

int main()
{
  /*
  Router r;
  r.add("10.0.1.2/32", 5);
  r.add("192.168.1.2/32", 5);
  */
  Router r = Router::initFromIO();
  r.listen(54321);
  return 0;
}
