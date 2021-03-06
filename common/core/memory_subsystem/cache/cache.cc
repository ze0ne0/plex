#include "simulator.h"
#include "cache.h"
#include "log.h"

// Cache class
// constructors/destructors
Cache::Cache(
   String name,
   String cfgname,
   core_id_t core_id,
   UInt32 num_sets,
   UInt32 associativity,
   UInt32 cache_block_size,
   String replacement_policy,
   cache_t cache_type,
   hash_t hash,
   FaultInjector *fault_injector,
   AddressHomeLookup *ahl)
:
   CacheBase(name, num_sets, associativity, cache_block_size, hash, ahl),
   m_enabled(false),
   m_num_accesses(0),
   m_num_hits(0),
   m_cache_type(cache_type),
   m_fault_injector(fault_injector)
{
	PRAK_LOG("Allocating cache.cc by core=%d cach.cfg=%s",core_id,cfgname.c_str());
	print_count=0;
//
//----------------PRAK-LOG---------------
	if(cache_type==CacheBase::SHARED_CACHE)
	{
		PRAK_LOG("Allocating num_sets=%d assoc=%d block_size=%d",num_sets,associativity,cache_block_size);
		W_min=2;
		ALPHA=5;
		BETA=55;

		p_num_modules=-1;
		p_sampling_ratio=64;

		m_num_sets=num_sets;

		if(m_num_sets==4096)
		{
			p_num_modules=8;
		}
		else if(m_num_sets==8192)
		{
			p_num_modules=16;
		}
		else if(m_num_sets==16384)
		{
			p_num_modules=32;
		}
		else
		{
			PRAK_LOG("Num of modules not assigned error");
		}

		if(p_num_modules!=-1)
		{
			isSubWayOn = new bool *[p_num_modules];
			L2Hits	   = new UInt64 * [p_num_modules];

			p_module_size=m_num_sets/p_num_modules;
			p_total_atds=m_num_sets/p_sampling_ratio;	//Note 64 is sampling ratio
			p_atds_per_module=p_total_atds/p_num_modules;

			for (int i = 0; i < p_num_modules; i++)
			{
				isSubWayOn[i] = new bool[m_associativity];
				L2Hits[i]     = new UInt64[m_associativity];
			
				for (UInt32 j = 0; j < m_associativity; j++)
				{
					isSubWayOn[i][j]=true;
					L2Hits[i][j]=0;
				}
				
			}
			PRAK_LOG("is Subway On initialized");
		}
	}
	else
	{
		PRAK_LOG("Creating set info");
	}	
//----------------PRAK-LOG--ENDS-HERE----



   m_set_info = CacheSet::createCacheSetInfo(name, cfgname, core_id, replacement_policy, m_associativity);
	PRAK_LOG("Calling cache set constrcutor");

   m_sets = new CacheSet*[m_num_sets];

   int a_index=1; 
   for (UInt32 i = 0; i < m_num_sets; i++)
   {
	if(a_index <= p_total_atds && i==(a_index*64-1))
	{
m_sets[i] = CacheSet::createCacheSet(cfgname, core_id, replacement_policy, m_cache_type, m_associativity, m_blocksize, m_set_info,true);
	 a_index+=1;
		//PRAK_LOG("creating leader set @i:%d",i);
	}
	else
	{
		m_sets[i] = CacheSet::createCacheSet(cfgname, core_id, replacement_policy, m_cache_type, m_associativity, m_blocksize, m_set_info,false);
	}
     
   }

//------------------------------------------------------------


   #ifdef ENABLE_SET_USAGE_HIST
   m_set_usage_hist = new UInt64[m_num_sets];
   for (UInt32 i = 0; i < m_num_sets; i++)
      m_set_usage_hist[i] = 0;
   #endif

	PRAK_LOG("leaving ---cache-constructor");
}

Cache::~Cache()
{
   #ifdef ENABLE_SET_USAGE_HIST
   printf("Cache %s set usage:", m_name.c_str());
   for (SInt32 i = 0; i < (SInt32) m_num_sets; i++)
      printf(" %" PRId64, m_set_usage_hist[i]);
   printf("\n");
   delete [] m_set_usage_hist;
   #endif

   if (m_set_info)
      delete m_set_info;

   for (SInt32 i = 0; i < (SInt32) m_num_sets; i++)
      delete m_sets[i];
   delete [] m_sets;
}

Lock&
Cache::getSetLock(IntPtr addr)
{
   IntPtr tag;
   UInt32 set_index;

   splitAddress(addr, tag, set_index);
   assert(set_index < m_num_sets);

   return m_sets[set_index]->getLock();
}

bool
Cache::invalidateSingleLine(IntPtr addr)
{
   IntPtr tag;
   UInt32 set_index;

   splitAddress(addr, tag, set_index);
   assert(set_index < m_num_sets);

   return m_sets[set_index]->invalidate(tag);
}

CacheBlockInfo*
Cache::accessSingleLine(IntPtr addr, access_t access_type,
      Byte* buff, UInt32 bytes, SubsecondTime now, bool update_replacement)
{
   //assert((buff == NULL) == (bytes == 0));

   IntPtr tag;
   UInt32 set_index;
   UInt32 line_index = -1;
   UInt32 block_offset;

   splitAddress(addr, tag, set_index, block_offset);

   CacheSet* set = m_sets[set_index];
   CacheBlockInfo* cache_block_info = set->find(tag, &line_index);

   if (cache_block_info == NULL)
   {
	//	PRAK_LOG("CAN'T ACCESS SINGLE LINE");
	   return NULL;
   }	

   if (access_type == LOAD)
   {
      // NOTE: assumes error occurs in memory. If we want to model bus errors, insert the error into buff instead
      if (m_fault_injector)
         m_fault_injector->preRead(addr, set_index * m_associativity + line_index, bytes, (Byte*)m_sets[set_index]->getDataPtr(line_index, block_offset), now);

      set->read_line(line_index, block_offset, buff, bytes, update_replacement);
   }
   else
   {
      set->write_line(line_index, block_offset, buff, bytes, update_replacement);

      // NOTE: assumes error occurs in memory. If we want to model bus errors, insert the error into buff instead
      if (m_fault_injector)
         m_fault_injector->postWrite(addr, set_index * m_associativity + line_index, bytes, (Byte*)m_sets[set_index]->getDataPtr(line_index, block_offset), now);
   }

   return cache_block_info;
}

void
Cache::insertSingleLine(IntPtr addr, Byte* fill_buff,
      bool* eviction, IntPtr* evict_addr,
      CacheBlockInfo* evict_block_info, Byte* evict_buff,
      SubsecondTime now,bool isShared,CacheCntlr *cntlr)
{
   IntPtr tag;
   UInt32 set_index;
   int module_index;
   bool isLeader;

   splitAddress(addr, tag, set_index);

   CacheBlockInfo* cache_block_info = CacheBlockInfo::create(m_cache_type);
   cache_block_info->setTag(tag);
	

	if(isShared )
	{

		 module_index=getModuleIndex(set_index);
		 isLeader=m_sets[set_index]->isLeaderSet();

		if(!isLeader)
		{

		 m_sets[set_index]->insertFlex(cache_block_info,fill_buff,eviction, evict_block_info, evict_buff,isSubWayOn[module_index],isLeader,isShared,cntlr);
		 VERI_LOG("FLEXOP_insert: %d  module:%d ",set_index,module_index);
		*evict_addr = tagToAddress(evict_block_info->getTag());

		   if (m_fault_injector) 
		  {
		      // NOTE: no callback is generated for read of evicted data
		      UInt32 line_index = -1;
		      __attribute__((unused)) CacheBlockInfo* res = m_sets[set_index]->find(tag, &line_index);
		      LOG_ASSERT_ERROR(res != NULL, "Inserted line no longer there?");

		    m_fault_injector->postWrite(addr, set_index * m_associativity + line_index, m_sets[set_index]->getBlockSize(), (Byte*)m_sets[set_index]->getDataPtr(line_index, 0), now);
		   }

		   #ifdef ENABLE_SET_USAGE_HIST
		   ++m_set_usage_hist[set_index];
		   #endif

		   delete cache_block_info;

		}
		else
		{
		   m_sets[set_index]->insert(cache_block_info,fill_buff,eviction, evict_block_info, evict_buff,cntlr);
		   *evict_addr = tagToAddress(evict_block_info->getTag());

		   if (m_fault_injector) 
		  {
		      // NOTE: no callback is generated for read of evicted data
		      UInt32 line_index = -1;
		      __attribute__((unused)) CacheBlockInfo* res = m_sets[set_index]->find(tag, &line_index);
		      LOG_ASSERT_ERROR(res != NULL, "Inserted line no longer there?");

		    m_fault_injector->postWrite(addr, set_index * m_associativity + line_index, m_sets[set_index]->getBlockSize(), (Byte*)m_sets[set_index]->getDataPtr(line_index, 0), now);
		   }

		   #ifdef ENABLE_SET_USAGE_HIST
		   ++m_set_usage_hist[set_index];
		   #endif

		   delete cache_block_info;
		}

		 
	
		//-------------------------------------------------------
	}
	else
	{

		   m_sets[set_index]->insert(cache_block_info,fill_buff,eviction, evict_block_info, evict_buff,cntlr);

		   *evict_addr = tagToAddress(evict_block_info->getTag());

		   if (m_fault_injector) {
		      // NOTE: no callback is generated for read of evicted data
		      UInt32 line_index = -1;
		      __attribute__((unused)) CacheBlockInfo* res = m_sets[set_index]->find(tag, &line_index);
		      LOG_ASSERT_ERROR(res != NULL, "Inserted line no longer there?");

		      m_fault_injector->postWrite(addr, set_index * m_associativity + line_index, m_sets[set_index]->getBlockSize(), (Byte*)m_sets[set_index]->getDataPtr(line_index, 0), now);
		   }

		   #ifdef ENABLE_SET_USAGE_HIST
		   ++m_set_usage_hist[set_index];
		   #endif

		   delete cache_block_info;

	}

}


// Single line cache access at addr
CacheBlockInfo*
Cache::peekSingleLine(IntPtr addr,bool isShared,bool isFlexOp)
{
   IntPtr tag;
   UInt32 set_index,line_index;
   int module_index;	
   splitAddress(addr, tag, set_index);
   bool isLeader;
   CacheBlockInfo* block_info=NULL;

	if(isShared & isFlexOp)
	{

		 module_index=getModuleIndex(set_index);
		 isLeader=m_sets[set_index]->isLeaderSet();

		 VERI_LOG("FLEXOP_peek_single: %d  module:%d ",set_index,module_index);

		 block_info=m_sets[set_index]->findFlex(tag,isSubWayOn[module_index],isLeader,isShared,isFlexOp,module_index,&line_index);

		//-----------------INCREMENT HITS
			if(block_info!=NULL)
			{
				L2Hits[module_index][line_index]=L2Hits[module_index][line_index] + 1;
			}
		
		//---------------------------------------

		return block_info;
		
	}
	else if(isShared==true && isFlexOp==false)
	{
		 module_index=getModuleIndex(set_index);
		 isLeader=m_sets[set_index]->isLeaderSet();
		 VERI_LOG("FLEXOP_peek_print: %d  module:%d ",set_index,module_index);

		 block_info=m_sets[set_index]->findFlex(tag,isSubWayOn[module_index],isLeader,isShared,isFlexOp,module_index,&line_index);

		return block_info;
		
	}
	else
	{
		   return m_sets[set_index]->find(tag);
	}
       return m_sets[set_index]->find(tag);
}

void
Cache::updateCounters(bool cache_hit)
{
   if (m_enabled)
   {
      m_num_accesses ++;
      if (cache_hit)
         m_num_hits ++;
   }
}

void
Cache::updateHits(Core::mem_op_t mem_op_type, UInt64 hits)
{
   if (m_enabled)
   {
      m_num_accesses += hits;
      m_num_hits += hits;
   }
}

void 
Cache:: insertSingleLineAt(UInt32 set_index,UInt32 insert_index,UInt32 replace_index,Byte* fill_buff,
bool* eviction, IntPtr* evict_addr,
            CacheBlockInfo* evict_block_info, Byte* evict_buff)
{
      CacheBlockInfo **cache_block_info_array=m_sets[set_index]->getBlock_info_array();

   CacheBlockInfo* cache_block_info= CacheBlockInfo::create(m_cache_type);

   cache_block_info->clone(cache_block_info_array[insert_index]);

   m_sets[set_index]->insertAt(replace_index,cache_block_info,fill_buff,eviction, evict_block_info, evict_buff);

*evict_addr = tagToAddress(evict_block_info->getTag());
delete cache_block_info;

	
}


//---------------------------------------------------------------

