#include "key_listener.h"

// https://gist.github.com/Youka/015684ad9a6999456fb7
key_listener::key_listener(const std::function<void(WPARAM,DWORD)> receiver) : std::thread([this, receiver]()
{
    const HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, nullptr, 0);
    if (hook) {
        LLKP_data = &receiver;
        MSG msg;
        while (!this->stop) {
            const auto timer_id = SetTimer(nullptr, 0, 1000, nullptr);
            GetMessage(&msg, nullptr, 0, 0);
            KillTimer(nullptr, timer_id);
            if (msg.message != WM_TIMER) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        UnhookWindowsHookEx(hook);
    } else {
        this->stop = true;
    }
}){}

bool key_listener::running() {
    return !this->stop;
}

key_listener::~key_listener() {
    this->stop = true;
    this->join();
}
