#include "cache_insert_policy.h"

class CacheInsertEndpoint: public CacheInsertPolicy
{
public:

	CacheInsertEndpoint() :
			CacheInsertPolicy()
	{
	}

	void insert(OverlayID &key, HostAddress &value)
	{
		DLLNode *n;
		HostAddress ha;
		if (hm->lookup(key, &n) || rt->lookup(key, &ha) || key == myOID)
		{
			return;
		}
		OverlayID oid(key.GetOverlay_id(), key.GetPrefix_length());
		HostAddress host_address(string(value.GetHostName().c_str()),
				value.GetHostPort());
		DLLNode *new_node = new DLLNode(oid, host_address);
		dll->append2Head(new_node);
		hm->add(key, new_node);
		(*cache_size)++;
	}
};
