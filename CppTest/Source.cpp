#include <math.h>
#include <stdio.h>

struct Base
{
	//virtual ~Base() {}
};

struct Object
{
	~Object() { printf("AAA"); }
};

struct Class : public Base
{
	Object a;
};

void main(char* args)
{
	Base* base = new Class();
	delete base;
	for (;;);
}