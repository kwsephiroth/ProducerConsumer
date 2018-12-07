#include "Consumer.h"

void Consumer::WriteDataToFile(bool symbolExists, const InputData & inputData)
{
	/*std::string symbol(inputData.symbol);
	std::string upperCaseSymbol;
	for (auto& ch : symbol)
	{
		upperCaseSymbol += toupper(ch);
	}
	std::string fileName = upperCaseSymbol + ".csv";*/
	
	try
	{
		std::string fileName = inputData.symbol + ".csv";
		std::ofstream outputFile;

		if (!symbolExists)
		{
			outputFile.open(fileName);
		}
		else
		{
			outputFile.open(fileName, std::ios::app);
		}

		if (!outputFile.is_open())
		{
			//std::cerr << "ERROR: Failed to create output file named '" << fileName << "'." << std::endl;
			throw std::runtime_error("ERROR: Could not open file named '" + fileName + "'.");
		}
		else
		{
			outputFile << inputData.price << ",";
			outputFile << inputData.quantity << ",";
			outputFile << inputData.sequenceNumber << "\n";
		}
	}
	catch (...)
	{
		exceptionPtr = std::current_exception();
	}
}

void Consumer::SortOutputData()
{
	try
	{
		std::ifstream inputFile;

		for (auto itr = uniqueSymbols.begin(); itr != uniqueSymbols.end(); itr++)
		{
			std::vector<std::unique_ptr<InputData>> temp;
			std::string fileName = (itr->first + ".csv");

			inputFile.open(fileName);
			if (inputFile.fail())
			{
				//std::cout << "ERROR: Could not open file named '" << fileName << "'." << std::endl;
				//throw - 1;
				throw std::runtime_error("ERROR: Could not open file named '" + fileName + "'.");
			}

			std::string row;
			std::vector<std::string> values;
			while (inputFile >> row)
			{
				SplitRowIntoValues(row, values);
				if (values.size() == 3)
				{
					std::unique_ptr<InputData> data(new InputData());
					data->symbol = itr->first;
					data->price = std::stod(values[0]);
					data->quantity = std::stoi(values[1]);
					data->sequenceNumber = std::stod(values[2]);
					temp.push_back(std::move(data));
				}
				values.clear();
			}

			inputFile.close();

			sort(temp.begin(), temp.end(),
				[](const std::unique_ptr<InputData>& element1, const std::unique_ptr<InputData>& element2)
			{
				if (element1->price == element2->price)
				{
					return (element1->sequenceNumber > element2->sequenceNumber);
				}
				else
					return (element1->price < element2->price);
			});

			std::ofstream outputFile(fileName, std::ios::out);

			if (outputFile.fail())
			{
				//std::cout << "ERROR: Could not open file named '" << fileName << "'." << std::endl;
				//throw - 1;
				throw std::runtime_error("ERROR: Could not open file named '" + fileName + "'.");
			}

			for (size_t i = 0; i < temp.size(); i++)
			{
				outputFile << temp[i]->price << ",";
				outputFile << temp[i]->quantity << ",";
				outputFile << temp[i]->sequenceNumber << "\n";
			}

			temp.clear();
			outputFile.close();
		}
	}
	catch (...)
	{
		exceptionPtr = std::current_exception();
	}
}

	void Consumer::SplitRowIntoValues(const std::string& dataRow, std::vector<std::string>& fieldBuffer)
	{
		std::string value;
		for (size_t i = 0; i < dataRow.length(); i++)
		{
			if (dataRow[i] != ',')
			{
				value += dataRow[i];
			}

			if ((dataRow[i] == ',') || (i == (dataRow.length() - 1)))
			{
				fieldBuffer.push_back(value);
				value.clear();
			}
		}
	}

bool Consumer::UpdateUniqueSymbols(const std::string& symbol)
{
	auto itr = uniqueSymbols.find(symbol);
	if (itr == uniqueSymbols.end())
	{
		uniqueSymbols.insert(std::make_pair(symbol, 1));
		return false;
	}
	else
	{
		itr->second++;
		return true;
	}
}

void Consumer::DisplayRowCounts()
{
	uint32_t totalRows = 0;
	for (auto itr = uniqueSymbols.begin(); itr != uniqueSymbols.end(); itr++)
	{
		std::cout << itr->first << " Rows: " << itr->second << std::endl;
		totalRows += itr->second;
	}
	std::cout << "Total Rows: " << totalRows << std::endl;
}

void Consumer::Consume()
{
	while (true)
	{
		auto data = dataQueue->Pop(); //Assign copy of data

		if (data.isTerminatingItem)
		{
			SortOutputData();
			break;
		}
		//TODO: Synchronize this function call
		auto symbolExists = UpdateUniqueSymbols(data.symbol);

		//TODO: Synchronize this function call
		WriteDataToFile(symbolExists, data);
	}
}
