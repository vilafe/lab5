//---------------------------------------------------------------------------

#include <System.hpp>
#pragma hdrstop

#include "Unit2.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall Read::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall Read::Read(bool CreateSuspended)
	: TThread(CreateSuspended)
{
	event = new TEvent(NULL, true, false, "", false);
	write = new Write(event, true);
}
//---------------------------------------------------------------------------
void __fastcall Read::Execute()
{
	int read_size = StrToInt(Form1->Edit1->Text);
	std::vector<BYTE> init_buf(read_size, 0);
	data_buffer = &init_buf.front();

	UnicodeString disk_label = Form1->Edit2->Text;
	LPWSTR disk = (L"\\\\.\\" + disk_label + L":").c_str();

	HANDLE ReaderH = CreateFileW(disk,
					GENERIC_READ,
					FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL);

	if (ReaderH == INVALID_HANDLE_VALUE) {
		int msgboxID = MessageBox(
		NULL,
		(LPCWSTR)L"Error creating file descriptor!",
		(LPCWSTR)L"Error",
		MB_ICONERROR
		);
		return;
	}

	write->FreeOnTerminate=true;
	write->Start();

	i = 0;
	while(true)
	{
		while(event->WaitFor(1) == wrSignaled)
			Sleep(5);

		LARGE_INTEGER sector_offset;
		sector_offset.QuadPart = i * read_size;
		unsigned long current_position = SetFilePointer(ReaderH, sector_offset.LowPart, &sector_offset.HighPart, FILE_BEGIN);

		if (current_position != sector_offset.LowPart) {
			CloseHandle(ReaderH);
			int msgboxID = MessageBox(
				NULL,
				(LPCWSTR)L"Error positioning!",
				(LPCWSTR)L"Error",
				MB_ICONERROR
			);
			return;
		}

		DWORD bytes_to_read = read_size;
		DWORD bytes_read;

		bool read_result = ReadFile(ReaderH, data_buffer, bytes_to_read, &bytes_read, NULL);

		if (!read_result || bytes_read != bytes_to_read) {
			CloseHandle(ReaderH);
			int msgboxID = MessageBox(
				NULL,
				(LPCWSTR)L"Error read!",
				(LPCWSTR)L"Error",
				MB_ICONERROR
			);
			return;
		}

		event->SetEvent();
		Sleep(10);
		i++;

		if(Terminated) {
			Form1->Button1->Enabled=true;
			break;
		}
	}

	CloseHandle(ReaderH);
	write->Terminate();
	delete write;
}
//---------------------------------------------------------------------------

