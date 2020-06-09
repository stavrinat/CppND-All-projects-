#include <iostream>
#include <random>
#include "TrafficLight.h"
#include <cstdlib> 
#include <chrono>
#include <ctime>   

/* Implementation of class "MessageQueue" */


template <typename T>
T  MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
  
  /*from udacity lessons:
   Vehicle popBack()
    {
        // perform vector modification under the lock
        std::unique_lock<std::mutex> uLock(_mutex);
        _cond.wait(uLock, [this] { return !_vehicles.empty(); }); // pass unique lock to condition variable

        // remove last vector element from queue
        Vehicle v = std::move(_vehicles.back());
        _vehicles.pop_back();

        return v; // will not be copied due to return value optimization (RVO) in C++
    }*/
  
  //The method receive should use std::unique_lock<std::mutex>
  	  std::unique_lock<std::mutex> uLock(_mutexMessageQueue);
  //and _condition.wait()  to wait for and receive new messages 
  	_condMessageQueue.wait(uLock, [this] { return !_queue.empty(); });
  //and pull them from the queue using move semantics
  T v = std::move(_queue.back());
        _queue.pop_back();
  // The received object should then be returned by the receive function
  return v;
  
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
  /*from udacity lessons
  void pushBack(Vehicle &&v)
    {
        // perform vector modification under the lock
        std::lock_guard<std::mutex> uLock(_mutex);
        std::cout << "   Vehicle #" << v.getID() << " will be added to the queue" << std::endl; 
        _vehicles.emplace_back(std::move(v));

        // simulate some work
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }*/
  
  //use the mechanisms std::lock_guard<std::mutex>
   std::lock_guard<std::mutex> uLock(_mutexMessageQueue);
  //add a new message to the queue
   _queue.emplace_back(std::move(msg));
  //afterwards send a notification
  _condMessageQueue.notify_one();
}


/* Implementation of class "TrafficLight" */

// about contructos in derived classes
//https://www.learncpp.com/cpp-tutorial/114-constructors-and-initialization-of-derived-classes/
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

TrafficLight::~TrafficLight()
{
    std::cout<<"TrafficLight destroyed \n";
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
  while(true)
  {
    
           if(messQueue.receive()==TrafficLightPhase::green)
       					return;
      
	}

}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::setCurrentPhase(TrafficLightPhase tPhase)
{
     _currentPhase  = tPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
  
  /*from udacity lessons
  // copying thread objects causes a compile error
        /*
        std::thread t(printHello);
        threads.push_back(t); 
        */
  /*The problem with our code is that by pushing the thread object into the vector, we attempt to make a copy of it. 
  However, thread objects do not have a copy constructor and thus can not be duplicated. 
  If this were possible, we would create yet another branch in the flow of execution - which 
  is not what we want. The solution to this problem is to use move semantics, 
  which provide a convenient way for the contents of objects to be moved' between objects, rather than copied*/

        // moving thread objects will work
       // threads.emplace_back(std::thread(printHello));  */
  threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases,this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
  
  // how to generate a random number in C++? https://stackoverflow.com/questions/13445688/how-to-generate-a-random-number-in-c/13445752
  std::random_device dev;
    std::mt19937 randnumgen(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist46(4000,6000); // distribution in range [4,6] seconds, that is in range[4000,6000] ms

  // what is the duration of a cycle?
  	long  cD = dist46(randnumgen); 
  
  // how to get current time and date in c++? https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
      auto start = std::chrono::system_clock::now();

  
  while(true)
  {
    	// the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      auto end = std::chrono::system_clock::now();
    
    //measures the time between two loop cycles
     // std::chrono::duration<int> elapsed_seconds = end-start;
    auto elapsed_seconds = end-start;
    
    // without the next command it is wrong, because elapsed_seconds >= cycleDuration , produces error
    // that you cannot use operant >= , to compare different types , because elapsed_seconds is not an int neither long int
    //https://en.cppreference.com/w/cpp/chrono/duration/duration_cast
    // std::chrono::duration_cast  --> Converts a std::chrono::duration to a duration of different type ToDuration.
    // example :   auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    long el = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_seconds).count();
      if(el>=cD)
      {
          //toggles the current phase of the traffic light between red and green 
        
        if(_currentPhase == TrafficLightPhase::green)
        {
          	_currentPhase = TrafficLightPhase::red;
        }
        else
        {
          	_currentPhase = TrafficLightPhase::green;
        }
        
        
         //and sends an update method to the message queue using move semantics
        messQueue.send(std::move(_currentPhase));
        
         // re-calculate randomly cycle duration for next round
        cD = dist46(randnumgen); 
        //save current time
         start = std::chrono::system_clock::now();
       
      }


  }
  
}

