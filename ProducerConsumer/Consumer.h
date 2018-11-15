#pragma once
#include "DataStructs.h"
#include "SharedDataQueue.h"
#include <memory>
//#include <mutex>
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
	/*static*/ std::mutex _mutex;
	std::condition_variable outputFileAccessible;
	//SharedDataQueue<std::shared_ptr<InputData>>& dataQueue;
	const std::shared_ptr<SharedDataQueue<InputData>> dataQueue;
	/*static*/ std::vector <std::string> uniqueSymbols; //TO DO: Might want to make this an std::set or std::map for faster lookup
	void WriteDataToFile(const InputData& inputData);
	void SortOutputData(const std::string& fileName);
	//void AddUniqueSymbol(const std::string& symbol);
	bool SymbolExists(const std::string& symbol);
	/*static void AddUniqueSymbol(std::string symbol)
	{
		//Must protect vector access with mutex
		std::unique_lock<std::mutex> locker(_mutex);
	}*/
public:
	Consumer(const std::shared_ptr<SharedDataQueue<InputData>> p_dataQueue) : dataQueue(p_dataQueue) { std::cout << "Data queue reference count: " << dataQueue.use_count() << std::endl; }
	~Consumer() {}
	void Consume();
};

