#include "Consumer.h"

void Consumer::WriteDataToFile(const InputData & inputData)
{
	std::string symbol(inputData.symbol);
	std::string upperCaseSymbol;
	for (auto& ch : symbol)
	{
		upperCaseSymbol += toupper(ch);
	}
	std::string fileName = upperCaseSymbol + ".csv";
	std::ofstream outputFile;

	if (!SymbolExists(symbol))
	{
		uniqueSymbols.emplace_back(std::string(symbol));
		outputFile.open(fileName);
	}
	else
	{
		outputFile.open(fileName, std::ios::app);
	}

	if (!outputFile.is_open())
	{
		std::cerr << "ERROR: Failed to create output file named '" << fileName << "'." << std::endl;
	}
	else
	{
		outputFile /*<< std::fixed << std::setprecision(4)*/ << inputData.price << ",";
		outputFile << inputData.quantity << ",";
		outputFile << inputData.sequenceNumber << "\n";
	}
}

void Consumer::SortOutputData(const std::string & fileName)
{

}

/*void Consumer::AddUniqueSymbol(const std::string & symbol)
{
	if (!SymbolExists(symbol))
	{

	}
}*/

bool Consumer::SymbolExists(const std::string & symbol)
{
	auto itr = std::find(uniqueSymbols.begin(), uniqueSymbols.end(), symbol);
	if (itr != uniqueSymbols.end())
	{
		return true;
	}
	return false;
}

void Consumer::Consume()
{
	while (true)
	{
		//auto& data = dataQueue.Pop(); //Assign reference of data
		auto data = dataQueue->Pop(); //Assign copy of data

		if (data.isTerminatingItem)
		{
			//TO DO: Sort the files here??
			break;
		}

		WriteDataToFile(data);
	}
}
