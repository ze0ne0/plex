#include "simulator.h"
#include "cache_cntlr.h"
#include "log.h"
#include "lock.h"
#include "fixed_types.h"
#include "subsecond_time.h"
#include "shmem_perf_model.h"

class PrakStats
{
	private:

		UInt64 L2_hits;
		UInt64 L2_access;
		UInt64 Dram_access;
	

		UInt64 l2_hits;
		UInt64 l2_access;
		UInt64 dram_access;

		UInt64 p_instruction_count;

		CacheCntlr *cache_cntlr;

		SubsecondTime t_now,t_prev;

	        ShmemPerfModel* m_shmem_perf_model;

	public:
		PrakStats(CacheCntlr *cc,ShmemPerfModel *sh );
		~PrakStats();
		void incrementL2(bool cache_hit);
		void incrementDram();
		void setInitTime();
		void incrementCount();

		void reset_stats(UInt64 **L2Hits,int p_num_modules,UInt32 m_associativity);
		int getActiveWays(bool **isSubWayOn,int p_num_modules,UInt32 m_associativity);
};
