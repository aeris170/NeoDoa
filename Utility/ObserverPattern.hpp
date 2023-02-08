#pragma once

#include <vector>
#include <entt.hpp>

using namespace entt::literals;

namespace ObserverPattern {

    struct Observable;

    using Notification = entt::hashed_string::hash_type;

    struct Observer {

        virtual ~Observer() = 0;

    protected:
        virtual void OnNotify(const Observable* source, Notification message) = 0;

    private:
        friend struct Observable;
    };
    inline Observer::~Observer() {}

    struct Observable {

        virtual ~Observable() = 0;

        inline void AddObserver(Observer& o) { observers.push_back(&o); }
        inline void RemoveObserver(Observer& o) { observers.erase(std::remove(observers.begin(), observers.end(), &o), observers.end()); }
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
    inline Observable::~Observable() {}
}

#ifdef _DEBUG
#include <type_traits>
static_assert(std::is_abstract_v<ObserverPattern::Observer>);
static_assert(std::is_copy_assignable_v<ObserverPattern::Observer>);
static_assert(std::is_move_assignable_v<ObserverPattern::Observer>);


static_assert(std::is_abstract_v<ObserverPattern::Observable>);
static_assert(std::is_copy_assignable_v<ObserverPattern::Observable>);
static_assert(std::is_move_assignable_v<ObserverPattern::Observable>);
#endif