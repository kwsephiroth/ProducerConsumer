#include "Consumer.h"

std::atomic<bool> Consumer::shouldConsume;
std::once_flag Consumer::consumeFlag;
std::once_flag Consumer::sortFlag;
static std::mutex _symbolsMutex;
static std::mutex _outfileMutex;
static std::unordered_map<std::string, uint32_t> uniqueSymbols;

void Consumer::WriteDataToFile(bool symbolExists, const InputData & inputData)
{
	static std::mutex _outfileMutex;
	/*std::string symbol(inputData.symbol);
	std::string upperCaseSymbol;
	for (auto& ch : symbol)
	{
		upperCaseSymbol += toupper(ch);
	}
	std::string fileName = upperCaseSymbol + ".csv";*/
	
	/*try
	{*/
		std::unique_lock<std::mutex> lock(_outfileMutex);
		std::cout << "Thread [" << std::this_thread::get_id() << "] called Consumer::WriteDataToFile()." << std::endl;
		//std::cout << "locked _outfileMutex" << std::endl;
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
			std::stringstream priceStr;
			priceStr << std::showpoint << std::fixed << std::setprecision(2) << inputData.price;
			outputFile << priceStr.str() << ",";
			outputFile << inputData.quantity << ",";
			outputFile << inputData.sequenceNumber << "\n";
		}
	/*}
	catch (...)
	{
		exceptionPtr = std::current_exception();
	}*/
}

void Consumer::SortOutputFiles()
{
	/*try
	{*/
		std::cout << "Thread [" << std::this_thread::get_id() << "] called Consumer::SortOutputFiles()." << std::endl;
		if (Consumer::shouldConsume.load() == true)
			return;

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
				std::stringstream priceStr;
				priceStr << std::showpoint << std::fixed << std::setprecision(2) << temp[i]->price;
				outputFile << priceStr.str() << ",";
				outputFile << temp[i]->quantity << ",";
				outputFile << temp[i]->sequenceNumber << "\n";
			}

			temp.clear();
			outputFile.close();
		}
	/*}
	catch (...)
	{
		exceptionPtr = std::current_exception();
	}*/
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
	std::unique_lock<std::mutex> lock(_symbolsMutex);
	//std::cout << "Thread [" << std::this_thread::get_id() << "] called Consumer::UpdateUniqueSymbols()." << std::endl;
	//std::cout << "locked _symbolsMutex" << std::endl;
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
	std::unique_lock<std::mutex> lock(_symbolsMutex);
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
	try
	{
		//std::cout << "Thread [" << std::this_thread::get_id() << "] called Consumer::Consume()" << std::endl;
		while (Consumer::shouldConsume.load())
		{
			auto data = dataQueue->Pop(); //Assign copy of data

			if (data.isTerminatingItem)
			{
				Consumer::shouldConsume.store(false);
				//std::call_once(Consumer::sortFlag, [this] { SortOutputFiles(); });
				break;
			}

			auto symbolExists = UpdateUniqueSymbols(data.symbol);

			WriteDataToFile(symbolExists, data);
			//std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
	catch (...)
	{
		exceptionPtr = std::current_exception();
	}
}
