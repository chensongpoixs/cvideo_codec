/**********************************************************************
*   程序中加入存储Dump的代码
*   create : 
*   date   : 2022-03-19
*   author : chensong 
***********************************************************************
#include<Windows.h>  
#include<DbgHelp.h>  
#pragma comment(lib,"DbgHelp.lib")  
// 创建Dump文件  
void CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)  
{  
    HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  
    // Dump信息  
    MINIDUMP_EXCEPTION_INFORMATION dumpInfo;  
    dumpInfo.ExceptionPointers = pException;  
    dumpInfo.ThreadId = GetCurrentThreadId();  
    dumpInfo.ClientPointers = TRUE;  
    // 写入Dump文件内容  
    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);  
    CloseHandle(hDumpFile);  
}  
// 处理Unhandled Exception的回调函数  
LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)  
{     
    CreateDumpFile(L"Test.dmp",pException);  
    return EXCEPTION_EXECUTE_HANDLER;  
}  
  
void fun(int *p)  
{  
    p[0]=0;  
}  
int main(int argc, char * argv[])  
{  
    //注册异常处理函数  
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);   
    fun(NULL);  
    return 0;  
  
}  



////////////////////////////////////////////////////////////////////////////////

/* 
Windows 程序捕获崩溃异常 生成dump
概述
事情的起因是，有个同事开发的程序，交付的版本程序，会偶尔随机崩溃了。

悲催的是没有输出log，也没有输出dump文件。

我建议他给程序代码加个异常捕获，在崩溃时生成dump，方便找出问题点。

隔了一天之后，短暂交流，发现他没有这个开发经验，我只好披挂上阵了。

开动
查阅MSDN文档，和stackoverlfow.com的相关文章，可知

SetUnhandledExceptionFilter 可以捕获触发系统崩溃的异常

风风火火开始写代码

复制代码
    void exceptionHandler(PEXCEPTION_POINTERS excpInfo)
    {
        // your code to handle the exception. Ideally it should
        // marshal the exception for processing to some other
        // thread and wait for the thread to complete the job
        std::unique_lock<std::mutex> lk(g_handlerLock);
        generateMiniDump(nullptr, excpInfo);
    }

    LONG WINAPI unhandledException(PEXCEPTION_POINTERS excpInfo = nullptr)
    {
        DebugBreak();

        if (excpInfo == nullptr)
        {
            __try // Generate exception to get proper context in dump
            {
                RaiseException(EXCEPTION_BREAKPOINT, 0, 0, nullptr);
            }
            __except (exceptionHandler(GetExceptionInformation()), EXCEPTION_EXECUTE_HANDLER)
            {
            }
        }
        else
        {
            exceptionHandler(excpInfo);
        }

        return 0;
    }

    SetUnhandledExceptionFilter(unhandledException);
复制代码
测试
在main函数入口，设置异常处理函数SetUnhandledExceptionFilter。

异常处理函数负责捕获异常，调用MiniDumpWriteDump生成dump文件，供开发者使用Windbg调试

编译运行

Access Volation C000005错误可以顺利捕获

令人费解的是，

abort，数组越界，虚函数调用异常等均无法捕获

系统把这些异常给拦截了，并给出了程序崩溃的提示窗口

改进
为了捕获这些异常并生成dump文件，必须要把系统拦截的那一层给禁止掉

1. 禁止系统弹出崩溃窗口，该窗口提示非常渣，对开发者和用户都不友好

  SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
可以模仿腾讯的QQ程序，专门开发一个对用户界面友好Bug Report的程序，
在程序崩溃时转存储dump文件时，运行该程序提示用户有用的信息。

2. 注册异常捕获函数

  SetUnhandledExceptionFilter(unhandledException);
当异常发生时，系统会跳进我们的unhandleException回调中

在该回调函数中，我们可以弹出Bug Report这样的子进程，并存储异常dump文件

3. 拦截C Runtime的异常处理

  _set_invalid_parameter_handler(invalidParameter);
  _set_purecall_handler(pureVirtualCall);
  signal(SIGABRT, sigAbortHandler);
  _set_abort_behavior(0, 0);
这些异常处理只是简单的调用unhandleException函数

4. 开启系统的程序崩溃请求

Vista之后，微软加了一个特性

程序崩溃时，默认不交给程序崩溃处理

而是使用一个莫名其妙的机制，不让程序进入崩溃环节

搞得用户懵逼，开发者也让代码无法进入崩溃异常处理

复制代码
    void EnableCrashingOnCrashes()
    {
        typedef BOOL(WINAPI *tGetPolicy)(LPDWORD lpFlags);
        typedef BOOL(WINAPI *tSetPolicy)(DWORD dwFlags);
        const DWORD EXCEPTION_SWALLOWING = 0x1;

        HMODULE kernel32 = LoadLibraryA("kernel32.dll");
        tGetPolicy pGetPolicy = (tGetPolicy)GetProcAddress(kernel32,
            "GetProcessUserModeExceptionPolicy");
        tSetPolicy pSetPolicy = (tSetPolicy)GetProcAddress(kernel32,
            "SetProcessUserModeExceptionPolicy");
        if (pGetPolicy && pSetPolicy)
        {
            DWORD dwFlags;
            if (pGetPolicy(&dwFlags))
            {
                // Turn off the filter 
                pSetPolicy(dwFlags & ~EXCEPTION_SWALLOWING);
            }
        }
    }
复制代码
5. 断了系统SetUnhandledExceptionFilter的后路

C Runtime等异常，运行时库会调用SetUnhandledExceptionFilter向系统注册一个NULL

从而使得我们之前注册的回调失效

真是无语(ˉ▽ˉ；)...

在这里，需要hook掉SetUnhandledExceptionFilter，在我们注册完回调之后，让它默认不做任何处理

用到Windows核心编程这本书里面，Jeffrey Richter开发的CAPIHook这个模块

复制代码
    void PreventSetUnhandledExceptionFilter()
    {
        CAPIHook apiHook("kernel32.dll",
            "SetUnhandledExceptionFilter",
            (PROC)ExceptionFilterHookProc);
    }
复制代码
其中ExceptionFilterHookProc这个函数是个空函数，无需做多余操作，直接renturn null即可

6. 完整流程

复制代码
    void setExceptionHandlers()
    {
        if (!IsDebuggerPresent() && !g_isHandlerSet)
        {
            g_isHandlerSet = true;

            SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
            SetUnhandledExceptionFilter(unhandledException);
            _set_invalid_parameter_handler(invalidParameter);
            _set_purecall_handler(pureVirtualCall);
            signal(SIGABRT, sigAbortHandler);
            _set_abort_behavior(0, 0);

            EnableCrashingOnCrashes();
            PreventSetUnhandledExceptionFilter();
        }
    }
复制代码
在VS或者Windbg中调试时，我们就没有必要生成dump文件了

IsDebuggerPresent这个系统API会帮助我们判断我们是否在调试环境中

总结
即使看起来这么简单的一个功能，也是需要挺多细节处理的。*/
