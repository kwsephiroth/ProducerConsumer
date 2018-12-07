#include <iostream>
#include <thread>
#include <string>
#include <memory>
#include <vector>
#include <cassert>
#include "DataStructs.h"
#include "Producer.h"
#include "Consumer.h"
#include "ExecutionTimer.h"


int main(int argc, char* argv[])
{
	std::string filePath;
	std::exception_ptr threadExceptionPtr = nullptr;

	if (argc == 2)
	{
		filePath = argv[1];
	}
	else
	{
		std::cout << "Please enter input file path: ";
		std::getline(std::cin, filePath);
	}

	//SharedDataQueue<std::shared_ptr<InputData>> dataQueue;
	auto dataQueue = std::make_shared<SharedDataQueue<InputData>>();

	//std::cout << "Data queue reference count: " << dataQueue.use_count() << std::endl;
	TimeResolution resolution = TimeResolution::Seconds;
	TimePoint start = HiResClock::now();
	Producer producer{ dataQueue, threadExceptionPtr };
	std::thread producerThread(&Producer::Produce, &producer, std::ref(filePath));
	assert(producerThread.joinable());
	producerThread.join(); //join() instructs calling thread to wait for completion of worker thread tasks

	Consumer consumer{ dataQueue, threadExceptionPtr };
	std::thread consumerThread(&Consumer::Consume, &consumer);
	assert(consumerThread.joinable());
	consumerThread.join();

	if (threadExceptionPtr) //Check for any exceptions after worker thread(s) has finished
	{
		try
		{
			std::rethrow_exception(threadExceptionPtr);
		}
		catch (const std::exception& ex)
		{
			std::cerr << ex.what() << std::endl;
		}
	}
	TimePoint finish = HiResClock::now();
	std::cout << "Executed in " << ExecutionTimer::GetExecutionTime(start, finish, resolution) << " " << TimeResolutionToString(resolution) << std::endl;
	consumer.DisplayRowCounts();
	//std::cout << "Data queue reference count: " << dataQueue.use_count() << std::endl;
	//std::cout << "Data consumption complete and output files generated! Exiting..." << std::endl;//Temporary comment
	system("pause"); //Temporary statement to prevent debug console from closing
	return 0;
}