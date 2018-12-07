#pragma once
#include "DataStructs.h"
#include "SharedDataQueue.h"
#include <memory>
#include <mutex>
//#include <condition_variable>
#include <vector>
//#include <chrono>
//#include <stdio.h>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include <unordered_map>

class Consumer
{
private:
	std::mutex _mutex;
	const std::shared_ptr<SharedDataQueue<InputData>> dataQueue;
	std::unordered_map<std::string, uint32_t> uniqueSymbols;
	void WriteDataToFile(bool symbolExists, const InputData& inputData);
	void SortOutputData();
	void SplitRowIntoValues(const std::string& dataRow, std::vector<std::string>& fieldBuffer);
	bool UpdateUniqueSymbols(const std::string& symbol);
	std::exception_ptr& exceptionPtr;

public:
	Consumer(const std::shared_ptr<SharedDataQueue<InputData>> p_dataQueue, std::exception_ptr& p_exceptionPtr) : dataQueue(p_dataQueue), exceptionPtr(p_exceptionPtr)
	{ 
		//std::cout << "Data queue reference count: " << dataQueue.use_count() << std::endl; 
	}
	~Consumer() {}
	void Consume();
	void DisplayRowCounts();
};

