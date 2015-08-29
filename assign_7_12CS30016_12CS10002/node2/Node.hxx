#ifndef __NODE_HXX
#define __NODE_HXX

#include <iostream>
#include <string>
using namespace std;

#define MOD 8

int hash(string p)
{
    int len=p.length();
    unsigned long long h = 0;
    int i;

    for (i = 0; i < len; i++)
    {
        h += p[i];
        h += (h << 10);
        h ^= (h >> 6);
    }

    h += (h << 3);
    h ^= (h >> 11);
    h += (h << 15);

    return (int)(h%MOD);
}

class Node
{
public:
	Node* succ;
	Node* pre;
	string ip,port;
	int ID;
	Node(string x="",string y="",int id=-1)
	{
		ip=x;
		port=y;
		ID=id;
	}
	inline Node* successor()
	{
		return this->succ;
	}
	inline Node* predecessor()
	{
		return this->pre;
	}

	~Node(){}
};

#endif
