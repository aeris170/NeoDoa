#pragma once

#include <eventpp/callbacklist.h>

template<typename> struct Event;

template<typename ReturnType, typename... Args>
struct Event<ReturnType(Args...)> {

    Event() noexcept = default;
    Event& operator+=(std::function<ReturnType(Args...)> callback) noexcept;
    void operator()(Args... args) noexcept;
private:
    eventpp::CallbackList<ReturnType(Args...)> callbackList;
};

template<typename ReturnType, typename... Args>
Event<ReturnType(Args...)>& Event<ReturnType(Args...)>::operator+=(std::function<ReturnType(Args...)> callback) noexcept {
    callbackList.append(callback);
    return *this;
}
template<typename ReturnType, typename... Args>
void Event<ReturnType(Args...)>::operator()(Args... args) noexcept {
    callbackList(args...);
}