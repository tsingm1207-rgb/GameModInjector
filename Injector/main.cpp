#include <Windows.h>
#include <filesystem>
#include <string>

// 用 WriteConsoleW 代替 wcout，避免所有编码问题
void Print(const std::wstring& msg)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written = 0;
    WriteConsoleW(hOut, msg.c_str(), (DWORD)msg.size(), &written, nullptr);
}

void PrintLine(const std::wstring& msg)
{
    Print(msg + L"\n");
}

bool InjectDLL(HANDLE hProcess, const std::wstring& dllPath)
{
    size_t allocSize = (dllPath.size() + 1) * sizeof(wchar_t);

    LPVOID dll_str = VirtualAllocEx(hProcess, nullptr, allocSize,
        MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (!dll_str)
    {
        PrintLine(L"VirtualAllocEx 失败，错误码：" + std::to_wstring(GetLastError()));
        return false;
    }

    if (!WriteProcessMemory(hProcess, dll_str, dllPath.c_str(), allocSize, nullptr))
    {
        PrintLine(L"WriteProcessMemory 失败，错误码：" + std::to_wstring(GetLastError()));
        VirtualFreeEx(hProcess, dll_str, 0, MEM_RELEASE);
        return false;
    }

    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!kernel32)
    {
        PrintLine(L"GetModuleHandleW 失败，错误码：" + std::to_wstring(GetLastError()));
        VirtualFreeEx(hProcess, dll_str, 0, MEM_RELEASE);
        return false;
    }

    FARPROC loadLibraryW = GetProcAddress(kernel32, "LoadLibraryW");
    if (!loadLibraryW)
    {
        PrintLine(L"GetProcAddress 失败，错误码：" + std::to_wstring(GetLastError()));
        VirtualFreeEx(hProcess, dll_str, 0, MEM_RELEASE);
        return false;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0,
        (LPTHREAD_START_ROUTINE)loadLibraryW, dll_str, 0, nullptr);

    if (!hThread)
    {
        PrintLine(L"CreateRemoteThread 失败，错误码：" + std::to_wstring(GetLastError()));
        VirtualFreeEx(hProcess, dll_str, 0, MEM_RELEASE);
        return false;
    }

    WaitForSingleObject(hThread, INFINITE);

    DWORD exitCode = 0;
    GetExitCodeThread(hThread, &exitCode);
    CloseHandle(hThread);
    VirtualFreeEx(hProcess, dll_str, 0, MEM_RELEASE);

    if (exitCode == 0)
    {
        PrintLine(L"LoadLibraryW 返回 NULL，DLL路径有误或架构不匹配");
        return false;
    }

    return true;
}

int wmain(int argc, wchar_t* argv[])
{
#ifdef _WIN64
    PrintLine(L"Injector 架构：x64");
#else
    PrintLine(L"Injector 架构：x86 ← 必须改为 x64！");
#endif

    if (argc != 3)
    {
        PrintLine(L"使用方法：Injector.exe <游戏文件路径.exe> <DLL路径>");
        return 1;
    }

    std::filesystem::path path_exe(argv[1]);
    std::filesystem::path path_dll(argv[2]);

    if (!std::filesystem::exists(path_exe))
    {
        PrintLine(L"游戏文件不存在：" + path_exe.wstring());
        return 1;
    }

    if (!std::filesystem::exists(path_dll))
    {
        PrintLine(L"DLL文件不存在：" + path_dll.wstring());
        return 1;
    }

    std::wstring absDllPath = std::filesystem::absolute(path_dll).wstring();

    PrintLine(L"目标程序：" + path_exe.wstring());
    PrintLine(L"注入DLL：" + absDllPath);

    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = {};

    if (!CreateProcessW(
        path_exe.wstring().c_str(),
        nullptr, nullptr, nullptr,
        FALSE,
        CREATE_SUSPENDED,
        nullptr, nullptr,
        &si, &pi))
    {
        PrintLine(L"创建进程失败，错误码：" + std::to_wstring(GetLastError()));
        return 1;
    }

    PrintLine(L"进程已创建，PID：" + std::to_wstring(pi.dwProcessId));

    if (!InjectDLL(pi.hProcess, absDllPath))
    {
        PrintLine(L"DLL注入失败，正在终止目标进程...");
        TerminateProcess(pi.hProcess, 1);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }

    PrintLine(L"DLL注入成功，恢复进程运行...");

    ResumeThread(pi.hThread);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}