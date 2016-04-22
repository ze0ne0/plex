#ifndef CACHE_STATE_H
#define CACHE_STATE_H

#include <cassert>

#include "fixed_types.h"
#include "log.h"

class CacheState
{
   public:
      enum cstate_t
      {
         CSTATE_FIRST = 0,
         INVALID = CSTATE_FIRST,
         SHARED,
         SHARED_UPGRADING,
         EXCLUSIVE,
         OWNED,
         MODIFIED,
         NUM_CSTATE_STATES,
         /* Below are special states, used only for reporting */
         INVALID_COLD = NUM_CSTATE_STATES,
         INVALID_EVICT,
         INVALID_COHERENCY,
         NUM_CSTATE_SPECIAL_STATES
      };

      CacheState(cstate_t state = INVALID) : cstate(state) {}
      ~CacheState() {}

      bool readable()
      {
	 bool op=(cstate == MODIFIED) || (cstate == OWNED) || (cstate == SHARED) || (cstate == EXCLUSIVE);
		//VERI_LOG("READABLE:%d",op);
         return op; 
      }
      bool writable()
      {
	bool op=(cstate == MODIFIED);
	//VERI_LOG("WRITABLE:%d",op);
         return op;
      }

   private:
      cstate_t cstate;

};

#endif /* CACHE_STATE_H */
