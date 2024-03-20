#include<iostream>
#include<Windows.h>
#include<TlHelp32.h>
#include<Psapi.h>
#include<vector>
using namespace std;
HANDLE HProces = NULL;
DWORD GetPidG(const char* nameprocess)
{
	PROCESS_MEMORY_COUNTERS ProcMC;
	PROCESSENTRY32 Pe32;
	HANDLE hSnapshot = NULL;
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{

		return 0;

	}
	else
	{
		Pe32.dwSize = sizeof(PROCESSENTRY32);
		HANDLE hProcess = NULL;
		while (Process32Next(hSnapshot, &Pe32))
		{
			if (strcmp(Pe32.szExeFile, nameprocess) == 0)
			{
				hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, Pe32.th32ProcessID);
				if (hProcess == INVALID_HANDLE_VALUE)
				{
					return 0;
				}
				GetProcessMemoryInfo(hProcess, &ProcMC, sizeof(ProcMC));
				if (ProcMC.WorkingSetSize > 300000000)
				{
					return Pe32.th32ProcessID;
					CloseHandle(hSnapshot);
					break;
				}
				else
				{

				}
			}
		}
		CloseHandle(hSnapshot);
	}

}
DWORD GetModuleGame(DWORD PIDGame , const char* ProcGame)
{
	MODULEENTRY32 Mod32;
	HANDLE hModuleSnap = NULL;
	Mod32.dwSize = sizeof(MODULEENTRY32);
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, PIDGame);
	//if (Module32First(hModuleSnap, &Mod32))
	//{
	//	if (strcmp(Mod32.szModule, ProcGame))
	//	{
	//		CloseHandle(hModuleSnap);
	//		return DWORD(Mod32.modBaseAddr);
	//		
	//	}
	//
	//}
	while (Module32Next(hModuleSnap, &Mod32))
	{
		cout << "Found: " << Mod32.szModule << endl;
		if (strcmp(Mod32.szModule, ProcGame)==0)
		{
			cout << "Found: " << Mod32.szModule << " ModBase: " << DWORD(Mod32.modBaseAddr) << endl;
			CloseHandle(hModuleSnap);
			return DWORD(Mod32.modBaseAddr);
			break;
		}

	}
	system("pause");
	return 0;
	CloseHandle(hModuleSnap);
}
void FailedHacking()
{
	Sleep(3000);
	exit(0);
}
DWORD GetBaseAddress(DWORD GetBaseAddr)
{
	DWORD VaulePointer = 0x000295DC;
	DWORD BaseAddressNeedOffset = 1809983544;
	ReadProcessMemory(HProces, LPVOID(GetBaseAddr + VaulePointer), &BaseAddressNeedOffset, sizeof(BaseAddressNeedOffset), 0);
	vector<DWORD> Offset{ 0x11C,0x20C,0x2C,0x4,0x40,0xFC, };
	for (int i = 0; i < Offset.size(); i++)
	{
		ReadProcessMemory(HProces, LPVOID(BaseAddressNeedOffset + Offset.at(i)), &BaseAddressNeedOffset, sizeof(BaseAddressNeedOffset), 0);
	}
	DWORD OffsetDan = 0xB94;
	DWORD AddressDan = BaseAddressNeedOffset + OffsetDan;
	return AddressDan;
}
int main()
{
	DWORD PID = GetPidG("aow_exe.exe");
	if (PID == 0)
	{
		cout << "That Bai Trong viec tim Kiem Process Aow" << endl;
		FailedHacking();
	}
	else
	{
		cout << "Tim Thay Process Aow_exe voi Pid la: " << PID << endl;
		DWORD GetBaseAddr = GetModuleGame(PID, "aow_exe.exe");
		if (GetBaseAddr == 0)
		{
			cout << "Khong The Tim Duoc BaseAddress Cua Module" << endl;
			FailedHacking();
			
		}
		else
		{
			cout << "Tim Thay ModuleBaseAddress la " << GetBaseAddr << endl;
			HProces = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
			if (HProces == INVALID_HANDLE_VALUE)
			{
				cout << "khong the Tan Cong Vao Game !" << endl;
				FailedHacking();
			}
			else
			{
				bool vbipshow = false;
				cout << "Tan Cong vao Game Thanh COng" << endl;
				DWORD AddressDanReal = GetBaseAddress(GetBaseAddr);
				cout << "Tim Thay AddrssDan la: " << AddressDanReal << endl;
				DWORD SoDanNhanVat;
				while (true)
				{
					ReadProcessMemory(HProces, LPVOID(AddressDanReal), &SoDanNhanVat, sizeof(SoDanNhanVat), 0);
					if (!vbipshow)
					{
						cout << "Tim Thay So Dan Nhan vat Co Duoc la: " << SoDanNhanVat << endl;
					}
					if (SoDanNhanVat < 40)
					{
						int vauletowrite = 9999;
						WriteProcessMemory(HProces, LPVOID(AddressDanReal), &vauletowrite, sizeof(vauletowrite), 0);
						cout << "Thanh Cong Ghi " << vauletowrite << " vao dia chi " << AddressDanReal << endl;
						vbipshow = true;
					}
					else
					{
						vbipshow = false;
					}
					Sleep(1000);
				}
				system("pause");
				
			}
		}

	}
}