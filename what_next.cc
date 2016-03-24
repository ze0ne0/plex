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



