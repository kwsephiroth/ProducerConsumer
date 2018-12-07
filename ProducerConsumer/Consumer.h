#pragma once
#include "DataStructs.h"
#include "SharedDataQueue.h"
#include <memory>
#include <mutex>
#include <vector>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include <unordered_map>
#include <sstream>
#include <atomic>

class Consumer
{
private:
	//std::mutex _symbolsMutex;
	//std::mutex _outfileMutex;
	static std::once_flag sortFlag;
	static std::once_flag consumeFlag;
	const std::shared_ptr<SharedDataQueue<InputData>> dataQueue;
	//std::unordered_map<std::string, uint32_t> uniqueSymbols;
	void WriteDataToFile(bool symbolExists, const InputData& inputData);
	
	static void SplitRowIntoValues(const std::string& dataRow, std::vector<std::string>& fieldBuffer);
	bool UpdateUniqueSymbols(const std::string& symbol);
	std::exception_ptr& exceptionPtr;
	
	static std::atomic<bool> shouldConsume;

	static void init_shouldConsume_flag(bool val)
	{
		Consumer::shouldConsume.store(val);
	}

public:
	Consumer(const std::shared_ptr<SharedDataQueue<InputData>> p_dataQueue, std::exception_ptr& p_exceptionPtr) : dataQueue(p_dataQueue), exceptionPtr(p_exceptionPtr)
	{ 
		//std::cout << "Data queue reference count: " << dataQueue.use_count() << std::endl; 
		std::call_once(consumeFlag, [] {Consumer::init_shouldConsume_flag(true); });
	}

	/*Consumer(Consumer&& other)
	{
		this->_outfileMutex = std::move(other._outfileMutex);
		this->_symbolsMutex = std::move(other._symbolsMutex);
	}

	Consumer& operator=(Consumer&& other)
	{

	}*/

	~Consumer() {}
	void Consume();
	void DisplayRowCounts();
	static void SortOutputFiles();
};

