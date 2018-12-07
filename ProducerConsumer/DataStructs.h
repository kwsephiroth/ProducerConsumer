#pragma once
#include<string>

struct InputData
{
	double sequenceNumber;
	std::string symbol;
	double price;
	unsigned int quantity;
	bool isTerminatingItem;

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