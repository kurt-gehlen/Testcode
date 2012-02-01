#include <stdio.h>

typedef unsigned long vmk_uint64;
typedef unsigned int vmk_uint32;

typedef struct
{
    vmk_uint64 deviceToken:12;
    vmk_uint64 deviceLba:44;
}__attribute__((__packed__)) keyType;

typedef struct arrayListNode
{
	vmk_uint32 a;
	vmk_uint32 b;
} arrayListNode;

typedef struct
{
    struct
    {
        keyType key;
        vmk_uint64 arrayListType:3;
        volatile vmk_uint64 pending:1;
        vmk_uint64 duplicate:1;
        vmk_uint64 cacheData:55;
        vmk_uint64 worldId:12;
    } __attribute__((__packed__));
    vmk_uint64 valid[2];
    arrayListNode cacheList;
    vmk_uint32 cacheEntry;
    vmk_uint32 hashChain;
}__attribute__((__packed__)) cacheEntry;

struct Regis
{
	unsigned long a:40;
	unsigned long b:12;
} __attribute__((__packed__));

struct Philbin
{
	Regis reg;
	unsigned long c:3;
	unsigned long d:12;
	unsigned long e:29;
} __attribute__((__packed__));

typedef struct
{
	struct
	{
		unsigned long a:40;
		unsigned long b:12;
	} __attribute__((__packed__)) key;

	unsigned long c:3;
	unsigned long d:12;
	unsigned long e:29;
} __attribute__((__packed__)) regis;



typedef struct
{
    unsigned long deviceToken:16;
    unsigned long deviceLba:48;
}keyTip;


struct DeviceEntry;

struct TokenLookup
{
	void * a	;
	void * b;
	unsigned int c;
} __attribute__((__packed__));

struct TokenEntry
{
	TokenLookup		lookup;
	DeviceEntry *	device;
//	struct {
	int				token:16;
	int				disabled :1;
	unsigned long hits: 47;
//	};// __attribute__((__packed__));
} __attribute__((__packed__)) TokenEntry;



struct jim
{
	int a;
	int b;
};

struct jimbo
{
	long a;
	long b;
};


int
main(int argc,char ** argv)
{
	cacheEntry a;
	a.key.deviceToken = 77;
	a.key.deviceLba = 3333;
	a.worldId = 444;

	printf("int = %d, long = %d, long long = %d\n", sizeof(int), sizeof(long), sizeof(long long));
	printf("int * = %d, void * = %d\n", sizeof(int *),sizeof(void*));
	printf("jim = %d, jimbo = %d\n", sizeof(jim),sizeof(jimbo));
	printf("keytype = %d, keytip = %d\n",sizeof(keyType),sizeof(keyTip));
	printf("te = %d tl = %d\n",sizeof(TokenEntry),sizeof(TokenLookup));
	printf("regis = %d\n", sizeof(regis));
	printf("Regis = %d, Philbin = %d\n", sizeof(Regis), sizeof(Philbin));
	printf("cacheEntry = %d\n",sizeof(cacheEntry));
	printf("a = %ld, %ld, %ld\n",a.key.deviceToken,a.key.deviceLba, a.worldId);
	printf("done\n");
}
