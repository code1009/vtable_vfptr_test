#include "stdafx.h"
#include <string>



/////////////////////////////////////////////////////////////////////////////
//==========================================================================
class my_parent
{
public:
	         my_parent()     {}
	virtual ~my_parent()     { printf ("my_parent::~my_parent()\r\n"); }
	virtual void print(void) { printf ("my_parent::print()\r\n");      }
};



/////////////////////////////////////////////////////////////////////////////
//==========================================================================
class my_object : public my_parent
{
public:
	unsigned int _member_variable;

public:
	         my_object()    { _member_variable=0xFFFFFFFFu;                                               }
	virtual ~my_object()    { printf ("my_object::~my_object()\r\n");                                     }
	virtual void print(void){ printf ("my_object::print(): _member_variable=0x%x\r\n", _member_variable); }
};



/////////////////////////////////////////////////////////////////////////////
//==========================================================================
typedef void ( __stdcall * vfunction_t0_t)(void);
typedef unsigned int*      address_pointer_t;


//==========================================================================
typedef struct _my_object_vtable_t
{
	vfunction_t0_t dtor ;
	vfunction_t0_t print;
}
my_object_vtable_t;

//==========================================================================
typedef struct _my_object_instance_t
{
	address_pointer_t __vfptr;

	int _member_variable;
} my_object_instance_t;




/////////////////////////////////////////////////////////////////////////////
//===========================================================================
my_object_vtable_t _my_object_vtable; // vtable 복사본

//===========================================================================
void my_object_vtable_update (address_pointer_t __vfptr)
{
	address_pointer_t vfunction_address_pointer; // virtual function
	address_pointer_t vfunction_address;
	unsigned int      vfunction_index;
	unsigned int      vfunction_count;


	vfunction_address_pointer = __vfptr;

	vfunction_count = 2u;
	for (vfunction_index=0; vfunction_index<vfunction_count; vfunction_index++)
	{
		vfunction_address = address_pointer_t(*vfunction_address_pointer);

		switch (vfunction_index)
		{
		case 0:
			_my_object_vtable.dtor = vfunction_t0_t(vfunction_address);
			break;

		case 1:
			_my_object_vtable.print = vfunction_t0_t(vfunction_address);
			break;

		default:
			break;
		}

		vfunction_address_pointer++;
	}
}




/////////////////////////////////////////////////////////////////////////////
//===========================================================================
int _tmain(int argc, _TCHAR* argv[])
{
	//-----------------------------------------------------------------------
	printf ("===========================================================================\r\n");
	printf ("STEP1:\r\n");
	printf ("===========================================================================\r\n");


	//-----------------------------------------------------------------------
	unsigned char buffer[8];


	memset(buffer, 0, 8);


	//-----------------------------------------------------------------------
	address_pointer_t memory;


	memory = address_pointer_t(&buffer);


	//-----------------------------------------------------------------------
	my_parent* object;
	

	object = new (memory) my_object(); // in-placemenet new
	object->print();
	object->~my_parent(); // __vfptr == my_parent
	object->print();

	object = new (memory) my_object(); // __vfptr == my_object


	//-----------------------------------------------------------------------
	printf ("\r\n");
	printf ("===========================================================================\r\n");
	printf ("STEP2:\r\n");
	printf ("===========================================================================\r\n");


	//-----------------------------------------------------------------------
	my_object_instance_t fake_object;


	fake_object.__vfptr          = address_pointer_t(*memory);
	fake_object._member_variable = 0x10000001u;


	my_object_vtable_update(fake_object.__vfptr); // __vfptr == my_object


	//-----------------------------------------------------------------------
	my_object_instance_t* fake_object_pointer;


	fake_object_pointer = &fake_object;
	
	__asm { mov ecx, dword ptr[fake_object_pointer] }// this 포인터 ecx레지스터에 저장
	_my_object_vtable.print();


#ifndef _DEBUG
	__asm 
	{ 
		push 0  
		mov  ecx,dword ptr [fake_object_pointer]  
	}
	_my_object_vtable.dtor();


	my_object_vtable_update(fake_object.__vfptr); // __vfptr == my_parent


	__asm { mov ecx, dword ptr[fake_object_pointer] }
	_my_object_vtable.print();
#endif


	return 0;
}

