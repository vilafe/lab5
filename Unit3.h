//---------------------------------------------------------------------------

#ifndef Unit3H
#define Unit3H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include "Unit1.h"
#include "Unit2.h"
#include <iostream>
#include <vector>
#include <windows.h>
//---------------------------------------------------------------------------
class Write : public TThread
{
private:
	TEvent *Event;
	int i;
protected:
	void __fastcall Execute();
public:
	__fastcall Write(TEvent *event, bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif

