#include <Utility/ObserverPattern.hpp>

ObserverPattern::Observer::~Observer() {}

ObserverPattern::Observable::~Observable() {}

void ObserverPattern::Observable::AddObserver(Observer& o) { observers.push_back(&o); }
void ObserverPattern::Observable::RemoveObserver(Observer& o) { observers.erase(std::remove(observers.begin(), observers.end(), &o), observers.end()); }
void ObserverPattern::Observable::RemoveAllObservers() { observers.clear(); }

void ObserverPattern::Observable::NotifyObservers(Notification message) {
    for (auto observer : observers) {
        observer->OnNotify(this, message);
    }
}