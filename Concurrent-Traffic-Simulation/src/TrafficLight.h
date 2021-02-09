#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"


// forward declarations to avoid include cycle
class Vehicle;

// FP.3 Define a class „MessageQueue“ which has the public methods send and receive.
// Send should take an rvalue reference of type TrafficLightPhase whereas receive should return this type.
// Also, the class should define an std::dequeue called _queue, which stores objects of type TrafficLightPhase.
// Also, there should be an std::condition_variable as well as an std::mutex as private members.

template <class T>
class MessageQueue
{
public:
    T receive();
    void send(T &&msg);
    
private:
    std::mutex _mutex;
    std::condition_variable _condition;
    std::deque<T> _queue;
};

// FP.1 : Define a class „TrafficLight“ which is a child class of TrafficObject.
// The class shall have the public methods „void waitForGreen()“ and „void simulate()“
// as well as „TrafficLightPhase getCurrentPhase()“, where TrafficLightPhase is an enum that
// can be either „red“ or „green“. Also, add the private method „void cycleThroughPhases()“.
// Furthermore, there shall be the private member _currentPhase which can take „red“ or „green“ as its value.

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();
    ~TrafficLight();
    
    // getters / setters
    enum TrafficLightPhase {red, green};
    TrafficLightPhase getCurrentPhase();
    void setCurrentPhase(TrafficLightPhase currentPhase) {_currentPhase=currentPhase;}

    // typical behaviour methods
    void waitForGreen();
    void simulate();
    
        std::condition_variable _condition;
        std::mutex _mutex;


private :
        // typical behaviour methods

    void cycleThroughPhases();
    TrafficLightPhase _currentPhase;
    
   // std::shared_ptr<MessageQueue<TrafficLightPhase>> _lightPhases;//(new MessageQueue<TrafficLightPhase>);
    

        // FP.4b : create a private member of type MessageQueue for messages of type TrafficLightPhase
        // and use it within the infinite loop to push each new TrafficLightPhase into it by calling
        // send in conjunction with move semantics.
    
    MessageQueue<TrafficLightPhase> _lightPhases;
    
    
    };

#endif
    
///HM-----------------------------------------------------
//this was the example code in the related lesson (I leave this here for myself for future reference only):
/*template <class T>
class MessageQueue
{
public:
    T receive()
    {
        // perform queue modification under the lock
        std::unique_lock<std::mutex> uLock(_mutex);
        _cond.wait(uLock, [this] { return !_messages.empty(); }); // pass unique lock to condition variable

        // remove last vector element from queue
        T msg = std::move(_messages.back());
        _messages.pop_back();

        return msg; // will not be copied due to return value optimization (RVO) in C++
    }

    void send(T &&msg)
    {
        // simulate some work
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // perform vector modification under the lock
        std::lock_guard<std::mutex> uLock(_mutex);

        // add vector to queue
        std::cout << "   Message " << msg << " has been sent to the queue" << std::endl;
        _messages.push_back(std::move(msg));
        _cond.notify_one(); // notify client after pushing new Vehicle into vector
    }

private:
    std::mutex _mutex;
    std::condition_variable _cond;
    std::deque<T> _messages;
};*/
//HM end -----------------------------------------------------




