#ifndef CACHE_H
#define CACHE_H

#include "cache_base.h"
#include "cache_set.h"
#include "cache_block_info.h"
#include "utils.h"
#include "hash_map_set.h"
#include "cache_perf_model.h"
#include "shmem_perf_model.h"
#include "log.h"
#include "core.h"
#include "fault_injection.h"

// Define to enable the set usage histogram
//#define ENABLE_SET_USAGE_HIST

class Cache : public CacheBase
{
   private:
      bool m_enabled;

      // Cache counters
      UInt64 m_num_accesses;
      UInt64 m_num_hits;

      // Generic Cache Info
      cache_t m_cache_type;

      CacheSet** m_sets;
      CacheSetInfo* m_set_info;//Follower sets
      FaultInjector *m_fault_injector;

//-----------------PRAK-LOG-----------------
	int p_num_modules;   // flexiway number of modules: Q in paper
	bool ** isSubWayOn;	// specify if any way of particular module is on or off
	UInt64 ** L2Hits;	// Hit count for leader sets
	UInt64  ALPHA;	
	UInt64 BETA;	

	int p_module_size;
	int p_atds_per_module;
	int p_sampling_ratio;
	int p_total_atds;
	UInt32 print_count;
	UInt32 W_min;
	
//----------------PRAK-LOG-ENDS-HERE

      #ifdef ENABLE_SET_USAGE_HIST
      UInt64* m_set_usage_hist;
      #endif

   public:

      // constructors/destructors
      Cache(String name,
            String cfgname,
            core_id_t core_id,
            UInt32 num_sets,
            UInt32 associativity, UInt32 cache_block_size,
            String replacement_policy,
            cache_t cache_type,
            hash_t hash = CacheBase::HASH_MASK,
            FaultInjector *fault_injector = NULL,
            AddressHomeLookup *ahl = NULL);
      ~Cache();

      Lock& getSetLock(IntPtr addr);

      bool invalidateSingleLine(IntPtr addr); //done

      void insertSingleLine(IntPtr addr, Byte* fill_buff,
            bool* eviction, IntPtr* evict_addr,
            CacheBlockInfo* evict_block_info, Byte* evict_buff, SubsecondTime now,bool isShared=false,CacheCntlr *cntlr = NULL);  //done

      CacheBlockInfo* peekSingleLine(IntPtr addr,bool isShared=false,bool isFlexOp=false);	//done

      CacheBlockInfo* accessSingleLine(IntPtr addr,
            access_t access_type, Byte* buff, UInt32 bytes, SubsecondTime now, bool update_replacement);//no need


      CacheBlockInfo* peekBlock(UInt32 set_index, UInt32 way) const { return m_sets[set_index]->peekBlock(way); }//not done


	cache_t getType()
	{
		return m_cache_type;
	}
	int getModuleIndex(UInt32 set_index)
	{
		int m_set_index=(int)set_index;
		for(int i=0;i<p_num_modules;i++)
		{
			if(m_set_index <= p_module_size * (i+1) &&  m_set_index >= p_module_size *i )
			{
				return i;
			}
		}
		return -1;
	}
      // Update Cache Counters
      void updateCounters(bool cache_hit);
      void updateHits(Core::mem_op_t mem_op_type, UInt64 hits);

void insertSingleLineAt(UInt32 set_index,UInt32 insert_index,UInt32 replace_index,Byte* fill_buff,bool* eviction, IntPtr* evict_addr,
            CacheBlockInfo* evict_block_info, Byte* evict_buff);

	CacheSet ** getSetPointer(){return m_sets;}
	int getModuleSize(){return p_module_size;}
	int  getNumModules(){return p_num_modules;}
	UInt32 getAssoc(){return m_associativity;}
	UInt32 getW_min(){return W_min;}
	UInt64 getAlpha(){return ALPHA;}
	UInt64 getBeta(){return BETA;}
	bool ** getSubWayMat(){return isSubWayOn;}
	UInt64 ** getgetL2HitsMat(){return L2Hits;}
	
      void enable() { m_enabled = true; }
      void disable() { m_enabled = false; }
};

template <class T>
UInt32 moduloHashFn(T key, UInt32 hash_fn_param, UInt32 num_buckets)
{
   return (key >> hash_fn_param) % num_buckets;
}

#endif /* CACHE_H */
