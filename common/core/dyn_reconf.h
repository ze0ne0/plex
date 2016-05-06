#ifndef __DYN_RECONF_H
#define __DYN_RECONF_H

class CacheCntlr;

#include "fixed_types.h"
#include "subsecond_time.h"
#include "shmem_perf_model.h"
#include "lock.h"

class Dyn_reconf
{
	
public:
		enum Status
	      	{
		 STABLE = 0,
		 FIRST_CHANGE,
		 RE_CHANGE
	      	};

		Dyn_reconf(CacheCntlr *c,ShmemPerfModel* s);
		~Dyn_reconf();
		void incrementCount();
		void processAddress(IntPtr );
		void setInitTime();

		UInt64 getInstructionCount();

		Status getState();
	private:
		UInt64 p_instruction_count;
		UInt64 p_base_count;
		UInt64 p_last_base_count;
		IntPtr p_base_addr;
		IntPtr p_last_base_addr;
		Status state;				
		Lock count_lock;
		CacheCntlr *l2_cache;
		SubsecondTime t_now,t_prev;
	        ShmemPerfModel* m_shmem_perf_model;
		
	
//	protected:
};

#endif
