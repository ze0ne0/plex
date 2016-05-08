#ifndef __PRAK_STATS_H
#define __PRAK_STATS_H


#include "fixed_types.h"
#include "shmem_perf_model.h"
#include "log.h"
#include "subsecond_time.h"
#include "cache_block_info.h"
#include "cache.h"


class PrakStats
{
	private:

		UInt64 L2_hits;
		UInt64 L2_access;
		UInt64 Dram_access;
	

		UInt64 l2_hits;
		UInt64 l2_access;
		UInt64 dram_access;

		UInt64 m_blk_tx,m_last_blk_tx,num_reconf;

		UInt64 p_instruction_count;

		CacheCntlr *cache_cntlr;
		Cache *m_cache;

		SubsecondTime t_now,t_prev;

	        ShmemPerfModel* m_shmem_perf_model;

	public:
		PrakStats(CacheCntlr *cc,ShmemPerfModel *sh,Cache *cache);
		~PrakStats();
		void incrementL2(bool cache_hit);
		void incrementDram();
		void setInitTime();
		void incrementCount();
		void reconfigure();
		void reset_stats(UInt64 **L2Hits,int p_num_modules,UInt32 m_associativity);
		int getActiveWays(bool **isSubWayOn,int p_num_modules,UInt32 m_associativity);
};
#endif
