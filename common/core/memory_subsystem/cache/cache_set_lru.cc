#include "cache_set_lru.h"
#include "log.h"
#include "stats.h"

// Implements LRU replacement, optionally augmented with Query-Based Selection [Jaleel et al., MICRO'10]

CacheSetLRU::CacheSetLRU(
      CacheBase::cache_t cache_type,
      UInt32 associativity, UInt32 blocksize, CacheSetInfoLRU* set_info, UInt8 num_attempts,bool ifLeader)
   : CacheSet(cache_type, associativity, blocksize,ifLeader)
   , m_num_attempts(num_attempts)
   , m_set_info(set_info)
{
   m_lru_bits = new UInt8[m_associativity];
   for (UInt32 i = 0; i < m_associativity; i++)
      m_lru_bits[i] = i;
}

CacheSetLRU::~CacheSetLRU()
{
   delete [] m_lru_bits;
}

UInt32
CacheSetLRU::getReplacementIndex(CacheCntlr *cntlr)
{
   // First try to find an invalid block
   for (UInt32 i = 0; i < m_associativity; i++)
   {
      if (!m_cache_block_info_array[i]->isValid())
      {
         // Mark our newly-inserted line as most-recently used
         moveToMRU(i);
         return i;
      }
   }
//----------------------------------

      UInt32 index = 0;
      UInt8 max_bits = 0;
      for (UInt32 i = 0; i < m_associativity; i++)
      {
	

         if (m_lru_bits[i] > max_bits && isValidReplacement(i))
         {
            index = i;
            max_bits = m_lru_bits[i];
         }
      }
if(m_associativity==8)
{
VERI_LOG("I0=%d I1=%d I2=%d I3=%d I4=%d I5=%d I6=%d I7=%d index:%d=%d",m_lru_bits[0],m_lru_bits[1],m_lru_bits[2],m_lru_bits[3],m_lru_bits[4],m_lru_bits[5],m_lru_bits[6],m_lru_bits[7],index,m_lru_bits[index]);
}
else if(m_associativity==4)
{
	VERI_LOG("I0=%d I1=%d I2=%d I3=%d index:%d=%d",m_lru_bits[0],m_lru_bits[1],m_lru_bits[2],m_lru_bits[3],index,m_lru_bits[index]);
}
	moveToMRU(index);

if(m_associativity==8)
{
VERI_LOG("I0=%d I1=%d I2=%d I3=%d I4=%d I5=%d I6=%d I7=%d index:%d=%d",m_lru_bits[0],m_lru_bits[1],m_lru_bits[2],m_lru_bits[3],m_lru_bits[4],m_lru_bits[5],m_lru_bits[6],m_lru_bits[7],index,m_lru_bits[index]);
}
else if(m_associativity==4)
{
	VERI_LOG("I0=%d I1=%d I2=%d I3=%d index:%d=%d",m_lru_bits[0],m_lru_bits[1],m_lru_bits[2],m_lru_bits[3],index,m_lru_bits[index]);
}

	return index;

//-------------------------------------
   LOG_PRINT_ERROR("Should not reach here");
}

UInt32
CacheSetLRU::getReplacementIndexFlex(CacheCntlr *cntlr,bool * isSubWay)
{

//	VERI_LOG("getreplacementflex");
 // First try to find an invalid block
   for (UInt32 i = 0; i < m_associativity; i++)
   {
      if(isSubWay[i]==false)
      {	
		break; 
      }
      if (!m_cache_block_info_array[i]->isValid())
      {
         // Mark our newly-inserted line as most-recently used
         moveToMRUFlex(i,isSubWay);
         return i;
      }
   }
//----------------------------------

      UInt32 index = 0;
      UInt8 max_bits = 0;
      for (UInt32 i = 0; i < m_associativity; i++)
      {
	  if(!isSubWay[i])
		continue;

         if (m_lru_bits[i] > max_bits && isValidReplacement(i))
         {
            index = i;
            max_bits = m_lru_bits[i];
         }
      }

	if(m_associativity==8)
	{
	VERI_LOG("I0=%d I1=%d I2=%d I3=%d I4=%d I5=%d I6=%d I7=%d index:%d=%d",m_lru_bits[0],m_lru_bits[1],m_lru_bits[2],m_lru_bits[3],m_lru_bits[4],m_lru_bits[5],m_lru_bits[6],m_lru_bits[7],index,m_lru_bits[index]);
	}
	else if(m_associativity==4)
	{
		VERI_LOG("I0=%d I1=%d I2=%d I3=%d index:%d=%d",m_lru_bits[0],m_lru_bits[1],m_lru_bits[2],m_lru_bits[3],index,m_lru_bits[index]);
	}
		moveToMRUFlex(index,isSubWay);

	if(m_associativity==8)
	{
	VERI_LOG("I0=%d I1=%d I2=%d I3=%d I4=%d I5=%d I6=%d I7=%d index:%d=%d",m_lru_bits[0],m_lru_bits[1],m_lru_bits[2],m_lru_bits[3],m_lru_bits[4],m_lru_bits[5],m_lru_bits[6],m_lru_bits[7],index,m_lru_bits[index]);
	}
	else if(m_associativity==4)
	{
		VERI_LOG("I0=%d I1=%d I2=%d I3=%d index:%d=%d",m_lru_bits[0],m_lru_bits[1],m_lru_bits[2],m_lru_bits[3],index,m_lru_bits[index]);
	}

	return index;

//-------------------------------------

   LOG_PRINT_ERROR("Should not reach here");
}

UInt32
CacheSetLRU::findTransReplacemnt(UInt32 min_way,bool *isSubWay)
{
	UInt32 index=0;
	 UInt8 max_bits=0;
	for(UInt32 v=0;v < min_way;v++)
	{
		if(m_cache_block_info_array[v]->getCState() != CacheState::MODIFIED)
		{
			return v;
		}
	}

	for(UInt32 v=0;v < min_way ;v++)
	{
		if(m_lru_bits[v] > max_bits)
		{
			max_bits=m_lru_bits[v];
			index=v;
		}
	}
	return index;
	PRAK_LOG("Should not reach here");
}


void
CacheSetLRU::updateReplacementIndex(UInt32 accessed_index)
{
   m_set_info->increment(m_lru_bits[accessed_index]);
   moveToMRU(accessed_index);
}

void
CacheSetLRU::moveToMRU(UInt32 accessed_index)
{
   for (UInt32 i = 0; i < m_associativity; i++)
   {
      if (m_lru_bits[i] < m_lru_bits[accessed_index])
         m_lru_bits[i] ++;
   }
   m_lru_bits[accessed_index] = 0;
}

void
CacheSetLRU::moveToMRUFlex(UInt32 accessed_index,bool * isSubWay)
{
   for (UInt32 i = 0; i < m_associativity; i++)
   {
	if(isSubWay[i]==false)
		continue;

      if (m_lru_bits[i] < m_lru_bits[accessed_index])
         m_lru_bits[i] ++;
   }
   m_lru_bits[accessed_index] = 0;
}

CacheSetInfoLRU::CacheSetInfoLRU(String name, String cfgname, core_id_t core_id, UInt32 associativity, UInt8 num_attempts)
   :m_associativity(associativity)
   , m_attempts(NULL)
{
   m_access = new UInt64[m_associativity];
   for(UInt32 i = 0; i < m_associativity; ++i)
   {
      m_access[i] = 0;
      		registerStatsMetric(name, core_id, String("access-mru-")+itostr(i), &m_access[i]);
   }

   if (num_attempts > 1)
   {
      m_attempts = new UInt64[num_attempts];
      for(UInt32 i = 0; i < num_attempts; ++i)
      {
         m_attempts[i] = 0;
        	 registerStatsMetric(name, core_id, String("qbs-attempt-")+itostr(i), &m_attempts[i]);
      }
   }
};


CacheSetInfoLRU::~CacheSetInfoLRU()
{
   delete [] m_access;
   if (m_attempts)
      delete [] m_attempts;
}
