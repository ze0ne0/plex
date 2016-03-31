#include "dyn_reconf.h"
#include "cache_cntlr.h"


Dyn_reconf::Dyn_reconf()
{
	p_instruction_count=0;
	p_base_count=0;
	p_last_base_count=0;
	p_base_addr=0;
	p_last_base_addr=0;
	state=STABLE;
	l2_cache=NULL;
}

Dyn_reconf::~Dyn_reconf()
{
}

void Dyn_reconf:: processAddress(IntPtr address)
{
	p_instruction_count=p_instruction_count;
} 


void Dyn_reconf:: incrementCount()
{
	count_lock.acquire();
	p_instruction_count+=1;
	if(p_instruction_count %20000 ==0)
	{
		PRAK_LOG("RECONFIGURATION INTERVAL REACHED");
		l2_cache->reconfigure();
		PRAK_LOG("RECONFIGURATION FINISHED");
	}
	count_lock.release();
}


UInt64 Dyn_reconf:: getInstructionCount()
{
	return p_instruction_count;
}

Dyn_reconf::Status Dyn_reconf:: getState()
{
	return state;
}

