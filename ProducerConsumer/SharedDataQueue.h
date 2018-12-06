#pragma once
#include <iostream>
#include <mutex>
#include <chrono>
#include <memory>
#include <deque>
#include <condition_variable>
#include "DataStructs.h"

template <typename T>
class SharedDataQueue //: std::enable_shared_from_this<T>
{
private:
	mutable std::mutex _mutex;
	std::condition_variable dataAvailable;
	std::deque<T> dataQueue;

public:
	SharedDataQueue() = default;
	SharedDataQueue(const SharedDataQueue&) = delete; //Prevent object copying
	SharedDataQueue operator=(const SharedDataQueue &) = delete; //Prevent object copying
	~SharedDataQueue() {};

	void Push(T inputData)
	{
		std::unique_lock<std::mutex> locker(_mutex);
		dataQueue.push_front(inputData);
		locker.unlock();
		dataAvailable.notify_all(); //Notify all waiting threads that data is available
	}

	T Pop()
	{
		T input;

		std::unique_lock<std::mutex> locker(_mutex);
		dataAvailable.wait(locker, [&]() { return !dataQueue.empty(); }); //Only wake if queue has data available
		input = dataQueue.back();
		dataQueue.pop_back();
		//std::cout << "Thread " << std::this_thread::get_id() << " consuming..." << std::endl;
		locker.unlock();
		//dataAvailable.notify_all();

		return input;
	}

	const T& Back() const //Read-only peek at back item
	{
		std::unique_lock<std::mutex> locker(_mutex);
		dataAvailable.wait(locker, [&]() { return !dataQueue.empty(); }); //Only wake if queue has data available
		/*if (dataQueue.empty())
		{
			locker.unlock();
			return nullptr;
		}*/
		auto& back = dataQueue.back();
		locker.unlock();
		//dataAvailable.notify_all();
		return back;
	}

	//size_t Size() { return dataQueue.size(); } //NOT THREAD SAFE CURRENTLY!!! Just for testing
};