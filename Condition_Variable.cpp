// Condition_Variable.cpp : This file contains the 'main' function. Program execution begins and ends there.
// 
// if you're traveling in overnight bus, to ensure you can get away from bus stop is to wake up all night whiel chatting to driver.
// you arive, you get off, but you'll  feel tired and travel is longer because you distracted drtive
// 
// another way is check timetable, set alarm and go to sleep
// but you might not wake up.  i there's more traffic, you'll wake up early, and vise versa
// 
// to ensure you get off correctly, you have to ask someone to wake you up at the right moment
// 
// 
// condition variable: mechanism for waiting for an event to triggered by another thread
// 
// condition variable is associated with some event, and one or more threads can wait for that event to happen.  if SOME THREAD
// HAS DETERMINED THAT THE CONDITION HAS BEEN SATISFIED,  it can notify one or more of the threads waiting for that variable , and 
// wake them up and allow them to continue processing.
// 
// 
// event: ariving at destination
// waiting thread: sleeping passenger
// 
// thread that determines event to happenand notify it to waiting thread- driver of the bus
//

#include <iostream>
#include <mutex>
#include <thread>
#include <string>
#include <chrono>

#include <condition_variable> 

bool have_I_arrived = false;
int distance_my_destination = 10;
int distance_covered = false;

std::condition_variable cv;
std::mutex m;

bool keep_driving()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		distance_covered++;
	}

	return false;
}

void keep_awake_all_night()
{
	while (distance_covered < distance_my_destination)
	{
		std::cout << "keep check, whether i am there \n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	std::cout << "finally i am there, distance_coverd = " << distance_covered << std::endl;;
}

void set_the_alarm_and_take_a_nap()
{
	if (distance_covered < distance_my_destination)
	{
		std::cout << "let me take a nap \n";
		std::this_thread::sleep_for(std::chrono::milliseconds(10000));
	}
	std::cout << "finally i am there, distance_coverd = " << distance_covered << std::endl;;
}


/*---------------------------------------------------------------------------------------------------------------------------------------*/

void keep_moving()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		distance_covered++;

		//notify the waiting threads if the event occurss
		if (distance_covered == distance_my_destination)
			cv.notify_one();							//notify_one() is a method for calling a particular waiting thread
	}

}

//waking up of thread can be from 
//1. notification of condition to another thread
//2. spurious wakeup from operating system.  if this happens before condition comes true, it will fall back to sleep

void ask_driver_to_wake_u_up_at_right_time()
{
	std::unique_lock<std::mutex> ul(m);

	//wait on condition variable
	//cv.wait (unique lock, condition)
	//cv.wait(ul, lambda expression to specify the condition after thread gets triggered
	// 
	//when passenger thread reaches the wait statement the first time, it will get
	// ownership of mutex associated with UL and check wether condition in lambda is true
	// it won't be true in the first moment, so condition variable will unlock associated 
	// mutex and make it sleep until it gets notified.
	// 
	// cv should be able to call lock and unlock on associated mutex. can't use lock_guard 
	// because it does not provideflexibility
	//

	cv.wait(ul, [] {return distance_covered == distance_my_destination; });
	std::cout << "finally i am there, distance_coverd = " << distance_covered << std::endl;;
}

void run_code()
{
   /* std::thread driver_thread(keep_driving);
    std::thread keep_awake_all_night_thread(keep_awake_all_night);
    std::thread set_alarm_and_take_a_nap_thread(set_the_alarm_and_take_a_nap);

    keep_awake_all_night_thread.join();
    set_alarm_and_take_a_nap_thread.join();
    driver_thread.join();*/


	std::thread driver_thread(keep_moving);
	std::thread passener_thread(ask_driver_to_wake_u_up_at_right_time);
	passener_thread.join();
	driver_thread.join();

}
int main()
{
    run_code();
    /*std::cout << "Hello World!\n";*/
}

