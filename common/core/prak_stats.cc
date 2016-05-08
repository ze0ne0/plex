#include "prak_stats.h"
#include "simulator.h"
#include "log.h"



PrakStats::PrakStats(CacheCntlr *cc,ShmemPerfModel *sh,Cache *cache )
{
	cache_cntlr=cc;
	m_shmem_perf_model=sh;
	m_cache=cache;
	p_instruction_count=0;

	L2_hits=0;
	L2_access=0;
	Dram_access=0;

	l2_hits=0;
	l2_access=0;
	dram_access=0;

}

int
PrakStats:: getActiveWays(bool **isSubWayOn,int p_num_modules,UInt32 m_associativity)
{	int count=0;

	for(int x=0; x< p_num_modules;x++)
	{
		for(UInt32 v=0; v< m_associativity; v++)
		{
			if(isSubWayOn[x][v]==true)
			{
				count++;
			}
		}
	}
	return count;
}
		
PrakStats::~PrakStats()
{
	t_now = m_shmem_perf_model->getElapsedTime(ShmemPerfModel::_USER_THREAD);	

	PRAK_LOG("###----STATS DESTRUCTOR---_###");
	PRAK_LOG("NUM OF L2 ACCESS:%lld",L2_access);
	PRAK_LOG("NUM OF L2 Hits:%lld",L2_hits);
	PRAK_LOG("NUM OF Dram ACCESS:%lld",Dram_access);

	bool ** isSubWayOn=m_cache->getSubWayMat();
	int p_num_modules=m_cache->getNumModules();
	UInt32 m_associativity=m_cache->getAssoc();


	STAT_LOG("%d;%lld;%lld;%lld;%lld;%d;%d",num_reconf,t_now.getNS()-t_prev.getNS(),l2_hits,l2_access-l2_hits,dram_access,
	m_last_blk_tx,getActiveWays(isSubWayOn,p_num_modules,m_associativity));

}
void
PrakStats:: incrementL2(bool cache_hit)
{
	l2_access+=1;
	L2_access+=1;
	if(cache_hit)
	{
		l2_hits+=1;
		L2_hits+=1;
	}
	
}

void
PrakStats:: incrementDram()
{
	Dram_access+=1;
	dram_access+=1;
}

void
PrakStats::setInitTime()
{
	t_prev =m_shmem_perf_model->getElapsedTime(ShmemPerfModel::_USER_THREAD);
	PRAK_LOG("init T_PREV:%lld",t_prev.getNS());
}

void
PrakStats:: reset_stats(UInt64 **L2Hits,int p_num_modules,UInt32 m_associativity)
{
	
	l2_hits=0;
	l2_access=0;
	dram_access=0;

	for(int x=0; x< p_num_modules;x++)
	{
		for(UInt32 v=0; v< m_associativity; v++)
		{
			L2Hits[x][v]=0;
		}
	}
}


void
PrakStats:: print_stats(UInt64 **L2Hits,int p_num_modules)
{

	for(int x=0; x< p_num_modules;x++)
	{
		PRAK_LOG("mod:%d w0:%lld w1:%lld w2:%lld w3:%lld w4:%lld w5:%lld w6:%lld w7:%lld",x,L2Hits[x][0],
		L2Hits[x][1],
		L2Hits[x][2],
		L2Hits[x][3],
		L2Hits[x][4],
		L2Hits[x][5],
		L2Hits[x][6],
		L2Hits[x][7]
		);
			
	}

}

void PrakStats:: incrementCount()
{
	p_instruction_count+=1;
	if(p_instruction_count %20000 ==0)
	{	
	      //  t_now =m_shmem_perf_model->getElapsedTime(ShmemPerfModel::_USER_THREAD);
	      //	PRAK_LOG("T_PREV:%llu  t:now:%llu",t_prev.getNS(),t_now.getNS());
	    	PRAK_LOG("RECONFIGURATION INTERVAL REACHED");
		reconfigure();
		PRAK_LOG("RECONFIGURATION FINISHED");
		p_instruction_count=0;
	}
	//t_prev=t_now;
}

void
PrakStats:: reconfigure()
{
	int p_num_modules=m_cache->getNumModules();
	UInt32 m_associativity=m_cache->getAssoc();
	UInt32 W_min=m_cache->getW_min();

//	UInt64 ALPHA=m_cache->getAlpha();
//	UInt64 BETA=m_cache->getBeta();
	bool ** isSubWayOn=m_cache->getSubWayMat();
	UInt64 ** L2Hits=m_cache->getgetL2HitsMat();

	UInt32 max=0,min=m_associativity-1;

	int active_ways=getActiveWays(isSubWayOn,p_num_modules,m_associativity);
	t_now =m_shmem_perf_model->getElapsedTime(ShmemPerfModel::_USER_THREAD);
	PRAK_LOG("In reconfiguration active ways:%d total_ways:%d t_now:%lld t_prev:%lld tot:%lld",active_ways,p_num_modules*m_associativity,t_now.getNS(),t_prev.getNS());
	VERI_LOG("In reconfiguration active ways:%d total_ways:%d t_now:%lld t_prev:%lld tot:%lld",active_ways,p_num_modules*m_associativity,t_now.getNS(),t_prev.getNS());

	print_stats(L2Hits,p_num_modules);

	for(int x=0; x< p_num_modules;x++)
	{	
//-------------------------------------------------------------
		max=0;
		min=m_associativity-1;
		bool didChangeHappen=false;
		bool tx=false;
//-------------------------------------------------------------

		//---------------------------------------------------------------------	
		for(UInt32 v=W_min;v< m_associativity; v++)
		{
			if(isSubWayOn[x][v]==false)
			{
				if(L2Hits[x][v] > 70 )
				{
					didChangeHappen=true;
					isSubWayOn[x][v]=true;
					for(UInt32 e=2;e<=v-1;e++)
					{
						if(isSubWayOn[x][e]==false)
						{
							isSubWayOn[x][e]=true;
						}	
					}
				}
		
			}
		}	
		//---------------------------------------------------------------------
		if(didChangeHappen==false)
		{
			for(UInt32 v=m_associativity-1 ;v >= W_min; v--)
			{
				if(isSubWayOn[x][v]==true)
				{
					if(L2Hits[x][v] < 20 )
					{
						isSubWayOn[x][v]=false;
						//VERI_LOG("turned off mod:%d way :%d  max:%d min:%d",x,v,max,min);
						didChangeHappen=true;tx=true;
						//block_transfer(x,v,isSubWayOn[x]);
						if(max < v) 
							max=v;

						if(min > v)
							min=v;
					}
					else
					{
						break;
					}
				}
			}
						
		}
		//---------------------------------------------------------------------
		//for(UInt32 v=0 ;v < m_associativity; v++)
		//{
		//	VERI_LOG("mod:%d max:%d min:%d way:%d isubway:%d",x,max,min,v,isSubWayOn[x][v]);			
		//	//PRAK_LOG("mod:%d max:%d min:%d way:%d isubway:%d",x,max,min,v,isSubWayOn[x][v]);			
		//}
		if(didChangeHappen==true && tx==true)
		{
			STAT_LOG("OFF-MOD:%d max:%d min:%d ",x,max,min);
			m_blk_tx=cache_cntlr->block_transfer(x,max,min,isSubWayOn[x]);	
		}
		else if(didChangeHappen==true && tx==false)	
		{
			STAT_LOG("ON/NO CHNAGE MOD:%d max:%d min:%d ",x,max,min);
		}	
	}

	if(m_blk_tx>0)
	{
		PRAK_LOG("BLOCK TX:%lld",m_blk_tx);
		VERI_LOG("BLOCK TX:%lld",m_blk_tx);
	}
	STAT_LOG("%d;%lld;%lld;%lld;%lld;%d;%d",num_reconf,t_now.getNS()-t_prev.getNS(),l2_hits,l2_access-l2_hits,dram_access,m_last_blk_tx,active_ways);

	m_last_blk_tx=m_blk_tx;
	m_blk_tx=0;

//	reset_stats(L2Hits,p_num_modules,m_associativity);

	num_reconf+=1;
	t_prev=t_now;
}
