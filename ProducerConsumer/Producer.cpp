#include "Producer.h"



std::vector<std::string> Producer::SeparateFields(const std::string& lineOfFields)
{
	std::string field;
	std::vector<std::string> values;
	std::istringstream ss(lineOfFields);

	while (std::getline(ss, field, ','))
	{
		values.emplace_back(field);
	}
	return values;
}

void Producer::Produce(const std::string& inputFilePath)
{
	std::ifstream inputFile(inputFilePath);
	std::string lineOfFields;

	try
	{
		if (!inputFile.is_open())
		{
			AddTerminatingItem();
			throw std::runtime_error("ERROR: Failed to open file at '" + inputFilePath + "'. Please check file path.");
		}

		while (std::getline(inputFile, lineOfFields))
		{
			auto fields = SeparateFields(lineOfFields);

			if (fields.size() != 4)
			{
				AddTerminatingItem();
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
					AddTerminatingItem();
					throw std::runtime_error("ERROR: Input data is not in correct format. Cannot have negative quantity. Please check data.");
				}
				else
				{
					inputData.quantity = quantity;
					dataQueue->Push(inputData);
				}
			}
		} //End of file stream

		//Add item to signify production is complete
		AddTerminatingItem();
	}
	catch (...)
	{
		exceptionPtr = std::current_exception();
	}
}

