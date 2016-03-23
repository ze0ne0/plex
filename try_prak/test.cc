#include "dyn_reconf.h"
#include <cstdio>
using namespace std;

int main()
{
	UInt64  count=0;
	Dyn_reconf *reconf=new Dyn_reconf();
	reconf->incrementCount();
	count=reconf->getInstructionCount();
	printf("Count=%lld \n" , count);
	delete reconf;
	return 0;
}
