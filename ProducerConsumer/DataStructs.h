#pragma once
#include<string>

struct InputData
{
	double sequenceNumber;
	std::string symbol;
	double price;
	unsigned int quantity;
	bool isTerminatingItem; //Not sure if I'll commit to this mechanism

	InputData() : isTerminatingItem() {}
};

struct OutputData
{
	double price;
	unsigned int quantity;
	double sequenceNumber;
};

struct SymbolStats
{
	std::string symbol;
	unsigned int count;
};