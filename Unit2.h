//---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include "Unit1.h"
#include "Unit3.h"
//---------------------------------------------------------------------------
class Read : public TThread
{
private:
	class Write *write;
	int i;
protected:
	void __fastcall Execute();
public:
	__fastcall Read(bool CreateSuspended);
	TEvent *event;
};

BYTE* data_buffer;
//---------------------------------------------------------------------------
#endif

