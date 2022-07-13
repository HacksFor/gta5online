#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry))
        {
            do
            {
                if (!_wcsicmp(modEntry.szModule, modName))
                {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}

DWORD GetProcId(const wchar_t* procName)
{
    DWORD procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);

        if (Process32First(hSnap, &procEntry))
        {
            do
            {
                if (!_wcsicmp(procEntry.szExeFile, procName))
                {
                    procId = procEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &procEntry));

        }
    }
    CloseHandle(hSnap);
    return procId;
}


const DWORD processID = GetProcId(L"GTA5.exe");
const HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
const uintptr_t dllBaseAddress = GetModuleBaseAddress(processID, L"GTA5.exe");



int main()
{
    //so it basically rewrite game assembly and create internal function hook (you can look at game memory with cheatEngine etc)
    // 
    //mov[rdx + rcx * 8], 2FAF080 	//new car cost in hex   0x2DF802B-address
    //jmp gta5.exe + 15DC17E
    //
    //5 bytes we don't care about
    //
    //mov rax, [GTA5.exe + 2686820]
    //cmp[rdx + rcx * 8], rax
    //je gta5.exe + 2DF802B  		//first line
    //mov rax, [rdx + rcx * 8]
    //jmp gta5.exe + 15DC17E
    //
    //jmp gta5.exe + 2DF803C        //0x15DCA73 end of function that writes to car value and other addreses


    const long long int var1 = 16791097132433671367;
    const long long int var2 = 4269687111;
    const int var3 = 0xCA3C8190;
    int var4 = 0;
    const long long int var5 = 71633462610224756;

    const long long int var6 = 5190269934155187433;

    std::cout << "Input your car cost: ";
    std::cin >> var4;

    std::cout << "\nStarted script...\n";

    WriteProcessMemory(processHandle, (void*)(dllBaseAddress + 0x2DF802B), &var1, sizeof(long long int), 0);
    WriteProcessMemory(processHandle, (void*)(dllBaseAddress + 0x2DF8033), &var2, sizeof(long long int), 0);
    WriteProcessMemory(processHandle, (void*)(dllBaseAddress + 0x2DF803B), &var3, sizeof(int), 0);
    WriteProcessMemory(processHandle, (void*)(dllBaseAddress + 0x2DF803F), &var4, sizeof(int), 0);
    WriteProcessMemory(processHandle, (void*)(dllBaseAddress + 0x2DF8043), &var5, sizeof(long long int), 0);

    WriteProcessMemory(processHandle, (void*)(dllBaseAddress + 0x15DCA73), &var6, sizeof(long long int), 0);

    //WriteProcessMemory(processHandle, (void*)(dllBaseAddress + 0x905A52), &line1, sizeof(long long int), 0);//movss xmm6<-[gta5.exe + 6B7F8]
    //WriteProcessMemory(processHandle, (void*)(dllBaseAddress + 0x905A5A), &line2, sizeof(long int), 0);//movss [rsi]<-xmm6
    std::cout << "Memory Patched...\n";
    std::cout << "If script didn't work (or game crashed) look for updated version github.com/hacksfor/gta5online\n";
    Sleep(10000);//you better to use sleep_for() i just don't care
}

