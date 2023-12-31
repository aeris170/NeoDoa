#pragma once

#include <vector>
#include <entt/entt.hpp>

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

    struct Observable {

        virtual ~Observable() = 0;

        void AddObserver(Observer& o);
        void RemoveObserver(Observer& o);
        void RemoveAllObservers();

    protected:
        virtual void NotifyObservers(Notification message);

    private:
        std::vector<Observer*> observers;
    };
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