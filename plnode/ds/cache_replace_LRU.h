#include "cache_replace_policy.h"

class CacheReplaceLRU: public CacheReplacePolicy
{
public:

	CacheReplaceLRU()
	{
	}

	void evict()
	{
		DLLNode *tail = dll->getTail();
		if (tail != NULL)
		{
			OverlayID *key = &(tail->key);
			dll->remove(tail);
			if (key != NULL)
			{
				hm->remove(*key);
			}
		}
	}

	void processHit(OverlayID key)
	{
		DLLNode *node;
		if (hm->lookup(key, &node))
		{
			dll->move2Head(node);
		}
	}

};
