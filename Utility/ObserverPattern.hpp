#pragma once

#include <vector>
#include <entt.hpp>

using namespace entt::literals;

namespace ObserverPattern {

    struct Observable;

    using Notification = entt::hashed_string::hash_type;

    struct Observer {
        virtual ~Observer() = default;

    protected:
        virtual void OnNotify(const Observable* source, Notification message) = 0;

    private:
        friend struct Observable;
    };

    struct Observable {

        virtual ~Observable() = default;

        inline void AddObserver(Observer& o) { observers.push_back(&o); }
        inline void RemoveObserver(Observer& o) { observers.erase(std::remove(observers.begin(), observers.end(), &o)); }
        inline void RemoveAllObservers() { observers.clear(); }

    protected:
        inline virtual void NotifyObservers(Notification message) {
            for (auto observer : observers) {
                observer->OnNotify(this, message);
            }
        }

    private:
        std::vector<Observer*> observers;
    };
}