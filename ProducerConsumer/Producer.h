#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <memory>
#include "DataStructs.h"
#include "SharedDataQueue.h"

class Producer
{
private:
	std::vector <std::string> SeparateFields(const std::string& lineOfFields);
	//SharedDataQueue<std::shared_ptr<InputData>>& dataQueue;
	const std::shared_ptr<SharedDataQueue<InputData>> dataQueue;
	std::exception_ptr& exceptionPtr;
	void AddTerminatingItem()
	{
		//auto terminatingItem = std::make_shared<InputData>();
		InputData terminatingItem;
		terminatingItem.isTerminatingItem = true;
		dataQueue->Push(terminatingItem);
	}
public:
	Producer(const std::shared_ptr<SharedDataQueue<InputData>> p_dataQueue, std::exception_ptr& p_exceptionPtr) : dataQueue(p_dataQueue), exceptionPtr(p_exceptionPtr) { std::cout << "Data queue reference count: " << dataQueue.use_count() << std::endl; }
	~Producer() {}
	void Produce(const std::string& inputFilePath);
};

