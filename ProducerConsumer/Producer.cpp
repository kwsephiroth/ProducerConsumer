#include "Producer.h"



void Producer::SeparateFields(const std::string& lineOfFields, std::vector<std::string>& fieldBuffer)
{
	std::string field;
	std::istringstream ss(lineOfFields);

	while (std::getline(ss, field, ','))
	{
		fieldBuffer.emplace_back(field);
	}
}

void Producer::Produce(const std::string& inputFilePath)
{
	std::ifstream inputFile(inputFilePath);
	std::string lineOfFields;

	try
	{
		if (!inputFile.is_open())
		{
			std::call_once(this->terminatorFlag, [this] { AddTerminatingItem(); });
			throw std::runtime_error("ERROR: Failed to open file at path '" + inputFilePath + "'. Please check file path.");
		}

		std::vector<std::string> fields;
		while (std::getline(inputFile, lineOfFields))
		{
			SeparateFields(lineOfFields, fields);

			if (fields.size() != 4)
			{
				std::call_once(this->terminatorFlag, [this] { AddTerminatingItem(); });
				throw std::runtime_error("ERROR: Input data is not in correct format. 4 fields per line expected. Please check data.");
			}
			else
			{
				InputData inputData;
				inputData.sequenceNumber = std::stod(fields[0]);
				inputData.symbol = fields[1];
				inputData.price = std::stod(fields[2]);
				int quantity = std::stoi(fields[3]);
				if (quantity < 0)
				{
					std::call_once(this->terminatorFlag, [this] { AddTerminatingItem(); });
					throw std::runtime_error("ERROR: Input data is not in correct format. Cannot have negative quantity. Please check data.");
				}
				else
				{
					inputData.quantity = quantity;
					dataQueue->Push(inputData);
				}
			}
			fields.clear();
		} //End of file stream

		//Add item to signify production is complete
		std::call_once(this->terminatorFlag, [this] { AddTerminatingItem(); });
	}
	catch (...)
	{
		exceptionPtr = std::current_exception();
	}
}

