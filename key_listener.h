#ifndef KEY_LISTENER_H
#define KEY_LISTENER_H

#include <thread>
#include <atomic>
#include <functional>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static thread_local const std::function<void(WPARAM,DWORD)>* LLKP_data;
static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam){
    if(nCode == HC_ACTION)
        (*LLKP_data)(wParam, reinterpret_cast<PKBDLLHOOKSTRUCT>(lParam)->vkCode);
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

class key_listener : protected std::thread
{
private:
    std::atomic_bool stop = ATOMIC_VAR_INIT(false);
    DWORD vkCode;
public:
    key_listener(const std::function<void(WPARAM,DWORD)> receiver);
    ~key_listener();
    bool running();
};

#endif // KEY_LISTENER_H
