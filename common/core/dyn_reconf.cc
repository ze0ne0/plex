#include "dyn_reconf.h"
#include "cache_cntlr.h"


Dyn_reconf::Dyn_reconf(CacheCntlr *c,ShmemPerfModel* s)
{
	p_instruction_count=0;
	p_base_count=0;
	p_last_base_count=0;
	p_base_addr=0;
	p_last_base_addr=0;
	state=STABLE;
//	l2_cache=NULL;
	l2_cache=c;
	m_shmem_perf_model=s;
	t_prev=t_now=SubsecondTime::Zero();
}

Dyn_reconf::~Dyn_reconf()
{
}

void Dyn_reconf:: processAddress(IntPtr address)
{
	p_instruction_count=p_instruction_count;
} 

void Dyn_reconf:: setInitTime()
{
	t_prev =m_shmem_perf_model->getElapsedTime(ShmemPerfModel::_USER_THREAD);
	PRAK_LOG("Init Time=%lld",t_prev.getNS());
} 


void Dyn_reconf:: incrementCount()
{

	p_instruction_count+=1;
	if(p_instruction_count %20000 ==0)
	{
	        t_now =m_shmem_perf_model->getElapsedTime(ShmemPerfModel::_USER_THREAD);
		PRAK_LOG("T_PREV:%llu  t:now:%llu",t_prev.getNS(),t_now.getNS());
		PRAK_LOG("RECONFIGURATION INTERVAL REACHED");
		l2_cache->reconfigure();
		PRAK_LOG("RECONFIGURATION FINISHED");
	}
	t_prev=t_now;
}


UInt64 Dyn_reconf:: getInstructionCount()
{
	return p_instruction_count;
}

Dyn_reconf::Status Dyn_reconf:: getState()
{
	return state;
}

