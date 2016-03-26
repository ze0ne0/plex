track memroy component initialization

./common/core/memory_subsystem/parametric_dram_directory_msi/cache_cntlr.cc
./common/core/memory_subsystem/parametric_dram_directory_msi/cache_cntlr.h



base_memory_manager - > parametric_dram_directory_msi/ memory_mamanger


open core.cc
memory_mamanger_base

track this function : initiateMemoryAccess
coreInitiateMemoryAccess


1)	core.cc 		coreinitiatememoryAccess
2)	memory_manager.cc = coreinitiateMemoryAccess
3)	coreInitiateMemoryAccess  calls either accesstlb(itlb or dtlb)
4)	processMemOpFromCore    1) call -> operationPermissibleinCache
				2) IF CACHE hit then update counter else
				3) call next level cache controllers funct
					processShmemReqFromPrevCache
				3.1) processShmemReqFromPrevCache calls operationPermissibleinCache function in cache_cntlr.cc/parametric_dsi
				3.2) operationPermissibleinCache calls getCacheBlockInfo(address) if return NULL means block is not in cache
				3.3) getCacheBlockInfo calls peekSingleLine function in cache.cc
				3.4) peekSingleLine function splits address into tag and index and calls find function in cache_set.cc
				     using tag as argument and line_index+NULL as default arg.
				3.5)
		
				4) processShmemReqFromPrevCache CALLS ITSELF TILL HIT OR 
					IF LLC IT ACCESSES DRAM
				5) accessDRAM CALLS file (dram_cntlr_interface  abstract class=virtual)
					I)  getdataFROMDRAM if read OPeration
					II) putDatatoDRAM if write operation
			virtual function in dram_cntlr_interface class are defined in dram/dram_cache.cc
			or in DRAM_CNTLR.H AS DRAM_CNTLR CLASS INHERITS IT.. IN OUR CASE DRAM_CNTLR CLASS
			in class dram_cache.

		

/home/nrs/Desktop/sniper_flexiway/common/core/memory_subsystem/cache/cache.cc has allocations
in cache constructor Cache
1) m_set_info creates one copy of set information and it used for all sets created (since identical information)
2) m_sets is array of CacheSet pointers hence it creates "m_num_sets" cache sets with parameters "m_set_info" and other as well 
	using function "createCacheSet"

3) createCacheSet calls "CacheSetLRU" if policy is lru in config files  
	Note "CacheSet" is class in cache_set.cc files which is inherited by "CacheSetLRU" so its contains
	m_cache_block_info_array a array of pointer to cache_block info_array
	in "CacheSet" constructor it allocates memory for each block in cache set
	also it creates cache_block info for each block which contains "tag"

4) "CacheBlockInfo " is virtual class and it calls the "SharedCacheBlockInfo" class
5) 


NumberofATDs for 4MB cache-> 8192 sets
sampling ratio of 64 	means 8192/64=128 atds
modules 16 means modules size=8192/16=512

num_atd_in_each_module=128/16=8

atds numbers are
63->127->191->255->319->383->447->511

In order to collect profiling information we need change set info which will decide leader set
and leader set won't be turned off;

"CreateCacheSetInfo"   calls cache_set.h file 
CacheSetInfo is defined solely in "cache_set.h" file

IMP we will add bool isLeaderSet=true if leader set otherwise false for follower set


cache_set.cc-----createCacheSetInfo  ---calls --> CacheSetLRUInfo

chnage all declaration where we are including boolean argument "ifLeader"
which will be assigned to "isLeader" in "CacheSetInfo" class constructor


next objective is to:
			create non-inclusize cache
			remove writebacks



