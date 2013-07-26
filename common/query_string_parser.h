#ifndef QUERY_STRING_PARSER_H_
#define QUERY_STRING_PARSER_H_

#include "../plnode/ds/lookup_table.h"
#include<vector>
#include"util.h"

using namespace std;

class QueryStringParser
{
	LookupTable<string, string> *key_value_store;
public:
	QueryStringParser(){
		key_value_store = new LookupTable<string, string>();
	}	
	
	void parse(const string query_string){
		vector<string> kvps = split(query_string, '&');
		for(vector<string>::iterator it = kvps.begin(); it != kvps.end(); ++it) {
			vector<string> kv = split(*it, '=');
			key_value_store->add(urlDecode(kv[0]), urlDecode(kv[1]));
		}
	}
	
	bool get_value(string key, string &value){
		return key_value_store->lookup(key, &value);
	}
	
	vector<string> &split(const string &s, char delim, vector<string> &elems) {
		stringstream ss(s);
		ss << std::noskipws;
		string item;
		while(getline(ss, item, delim)) {
			elems.push_back(item);
			printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
			printf(">>>>>>>>>  %s  ", item.c_str());
			printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
			item.clear();
		}
		return elems;
	}

	vector<string> split(const string &s, char delim) {
		vector<string> elems;
		return split(s, delim, elems);
	}
	
	int size(){
		return key_value_store->size();
	}
};

#endif
