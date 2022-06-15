//---------------------------------------------------------------------------

#include <System.hpp>
#pragma hdrstop

#include "Unit3.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall Write::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------
extern BYTE* data_buffer;


__fastcall Write::Write(TEvent *event, bool CreateSuspended)
	: TThread(CreateSuspended), Event(event)
{
}
//---------------------------------------------------------------------------
void __fastcall Write::Execute()
{
	FreeOnTerminate = true;

	int write_size = StrToInt(Form1->Edit1->Text);

	HANDLE WriteH = CreateFileW(L".\\image.copy",
			FILE_APPEND_DATA | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

	if (WriteH == INVALID_HANDLE_VALUE)
	{
		CloseHandle(WriteH);
		int msgboxID = MessageBox(
		NULL,
		(LPCWSTR)L"Error creating file of image!",
		(LPCWSTR)L"Error",
		MB_ICONERROR
		);
		return;
	}

	i = 0;
	while(true)
	{
		LARGE_INTEGER sec_off;
		sec_off.QuadPart = i * write_size;
		unsigned long curr_pos = SetFilePointer(WriteH, sec_off.LowPart, &sec_off.HighPart, FILE_BEGIN);
		if (curr_pos != sec_off.LowPart)
		{
			CloseHandle(WriteH);
			int msgboxID = MessageBox(
			NULL,
			(LPCWSTR)L"Error positioning in writeble file!",
			(LPCWSTR)L"Error",
			MB_ICONERROR
			);
			return;
		}

		DWORD btw = write_size;
		DWORD bw;

		if(Event->WaitFor(1) == wrSignaled) {
			bool write_result = WriteFile(WriteH, data_buffer, btw, &bw, NULL);

			if (write_result == false || bw != btw)
			{
				CloseHandle(WriteH);
				int msgboxID = MessageBox(
				NULL,
				(LPCWSTR)L"Error writing!",
				(LPCWSTR)L"Error",
				MB_ICONERROR
				);
				return;
			}

			i++;
			Event->ResetEvent();
		}
	}

	CloseHandle(WriteH);
}
//---------------------------------------------------------------------------

