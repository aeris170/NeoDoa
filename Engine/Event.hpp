#pragma once

#include <eventpp/callbacklist.h>

template<typename> struct Event;

template<typename ReturnType, typename... Args>
struct Event<ReturnType(Args...)> {
    using Callback = std::function<ReturnType(Args...)>;
    using CallbackHandle = eventpp::CallbackList<ReturnType(Args...)>::Handle;

    Event() noexcept = default;
    CallbackHandle operator+=(Callback callback) noexcept;
    void operator-=(CallbackHandle callbackHandle) noexcept;
    void operator()(Args... args) noexcept;
private:
    mutable eventpp::CallbackList<ReturnType(Args...)> callbackList;
};

template<typename ReturnType, typename... Args>
Event<ReturnType(Args...)>::CallbackHandle Event<ReturnType(Args...)>::operator+=(Callback callback) noexcept {
    return callbackList.append(callback);
}
template<typename ReturnType, typename... Args>
void Event<ReturnType(Args...)>::operator-=(CallbackHandle callbackHandle) noexcept {
    callbackList.remove(callbackHandle);
}
template<typename ReturnType, typename... Args>
void Event<ReturnType(Args...)>::operator()(Args... args) noexcept {
    callbackList(args...);
}