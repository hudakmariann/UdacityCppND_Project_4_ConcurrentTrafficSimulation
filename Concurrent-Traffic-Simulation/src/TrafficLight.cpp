#include <iostream>
#include <random>
#include "TrafficLight.h"
#include <vector>
#include <queue>
#include <future>
#include <thread>
#include <algorithm>

/* Implementation of class "MessageQueue" */


template <class T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    {
        // perform queue modification under the lock
        std::unique_lock<std::mutex> uLock(_mutex);

        //TO REVIEWER: QUESTION: here the reviewer suggests a while loop to check for the condition again in case of spurious wake-up.
        //but in Lesson 4, chapter 3, a lambda function is suggested instead of the while loop to simplify the code further.
        //Is not the lambda function sufficient to re-check the condition?

        _condition.wait(uLock, [this] { return !_queue.empty(); }); // pass unique lock to condition variable

        // remove last vector element from queue
        T msg = std::move(_queue.back());
        _queue.pop_back();

        return msg; // will not be copied due to return value optimization (RVO) in C++
    }
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.

    // simulate some work
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // perform vector modification under the lock
        std::lock_guard<std::mutex> uLock(_mutex);

        // add vector to queue
        //std::cout << "   Message " << msg << " has been sent to the queue" << std::endl;
        _queue.push_back(std::move(msg));
        _condition.notify_one(); // notify client after pushing new Vehicle into vector
}

/* Implementation of class "TrafficLight" */

 
TrafficLight::TrafficLight()
{
  //std::shared_ptr<MessageQueue<TrafficLightPhase>> _lightPhases=std::make_shared <MessageQueue<TrafficLightPhase>>;
      _currentPhase = TrafficLightPhase::red;
      //std::shared_ptr<MessageQueue<TrafficLightPhase>> _lightPhases(new MessageQueue<TrafficLightPhase>); 
    
}

TrafficLight::~TrafficLight(){};

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
  while (true)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    auto  message = _lightPhases.receive();
    if (message==green) {break;} //TO REVIEWER: here the "=" instead of "==" was just a typo on my part, perhaps due to my object Pascal/Delphi past....
  } 
//_condition.notify_one();
}

TrafficLight::TrafficLightPhase TrafficLight::getCurrentPhase()

{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread 
    //when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{ //begin procedure
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
  
  //TO REVIEWER: the Reviewer suggested that I should have used the sleep_for for 4000 seconds and only then should I have started to measure the time . I also thought of this. THe only reason why I did not do it like this in my previous submission is that in the project instructions, the folowing is required: "Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles."  But here I rewrite the code to sleep for 4000 ms in the first part of the cycle.

//var
bool newCycle=true;
int r; 
std::chrono::high_resolution_clock::time_point t1;
std::chrono::high_resolution_clock::time_point t2;
srand(time(0));
//int d;

//begin
while (true)
  {//outer while begin    
    std::this_thread::sleep_for(std::chrono::milliseconds(3999)); 
    t1 = std::chrono::high_resolution_clock::now();
    r = rand() % 2000+1;
    while (true)
      {//inner while begin
	  std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
      t2 = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
      if (duration>=r)
        {
        _currentPhase=getCurrentPhase() == green ? red: green; //here the reviewer suggested "_currentPhase = !_currentPhase;" instead of the if..else, however, the if..else had been already commented out even in my first submission - I leave it here just for myself to see in the future that these 3 expressions are equivalent.
        
        /* the above line is equivalent to:
        if (_currentPhase==green){_currentPhase=red;}
        else
        if (_currentPhase==red){_currentPhase=green;}*/
        
        _lightPhases.send(std::move(_currentPhase));
        //std::cout<<"_currentPhase= " <<_currentPhase << "\n";
        //d=duration;
        break;
        } // end if duration>=r
    }//end inner while
  //std::cout<<"duration= " << d << ", r= "<< r << "\n";
  } //end outer while
} //end procedure

