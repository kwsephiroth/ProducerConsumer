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
	int consumerCount = 0;
	std::exception_ptr threadExceptionPtr = nullptr;

	//TODO: Allow consumer thread count as command-line parameter.

	if (argc == 3)
	{
		filePath = argv[1];
		consumerCount = atoi(argv[2]);
	}
	else
	{
		std::cout << "Please enter input file path: ";
		std::getline(std::cin, filePath);
		std::cout << "Please enter the number of processing threads (1 to 8 max): ";
		std::cin >> consumerCount;

		if (consumerCount <= 0 || consumerCount > 8)
		{
			std::cerr << "ERROR: Invalid number of processing threads. Number must be between 1 and 8 inclusive." << std::endl;
			system("pause");
			return -1;
		}
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

	std::vector<std::unique_ptr<Consumer>> consumers;
	std::vector<std::thread> consumerThreads;

	for (int i = 0; i < consumerCount; i++)
	{
		consumers.push_back(std::make_unique<Consumer>(dataQueue, threadExceptionPtr));
		std::thread consumerThread(&Consumer::Consume, consumers[i].get());
		consumerThreads.push_back(std::move(consumerThread));
	}

	for (auto& th : consumerThreads)
	{
		assert(th.joinable());
		th.join();
	}

	/*Consumer consumer{ dataQueue, threadExceptionPtr };
	std::thread consumerThread(&Consumer::Consume, &consumer);
	assert(consumerThread.joinable());
	consumerThread.join();

	Consumer consumer2{ dataQueue, threadExceptionPtr };
	std::thread consumer2Thread(&Consumer::Consume, &consumer2);
	assert(consumer2Thread.joinable());
	consumer2Thread.join();*/

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
	else
	{
		Consumer::SortOutputFiles();
		TimePoint finish = HiResClock::now();
		std::cout << "\nExecuted in " << ExecutionTimer::GetExecutionTime(start, finish, resolution) << " " << TimeResolutionToString(resolution) << std::endl << std::endl;
		consumers[0]->DisplayRowCounts();
		//std::cout << "Data queue reference count: " << dataQueue.use_count() << std::endl;
		//std::cout << "Data consumption complete and output files generated! Exiting..." << std::endl;//Temporary comment
	}
	system("pause"); //Temporary statement to prevent debug console from closing
	return 0;
}