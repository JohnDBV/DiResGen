#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>

enum ErrorCodes
{//no need for enum classes,in this project. I return the values directly
	NoError = 0,
	NoSourceFile,
	NoDestinationFile,
	TooManyParameters,
	CannotReadInputFile,
	CannotWriteOutputFile,
	UnknownError
};

[[noreturn]] void displayHelp(char* argv0);
[[noreturn]] void processCommandLineArguments(int argc, char* argv[]);
int generateData();

constexpr const char* gitHubPage = "https://github.com/JohnDBV/DiResGen";

int mandatoryParamsCount = 0;
std::string srcFilePath;
std::string destFilePath;
bool isHelpDisplayed = false;

int main(int argc, char* argv[])
{
	processCommandLineArguments(argc, argv);

	switch (mandatoryParamsCount)
	{
		case 0 :
		{
			if (!isHelpDisplayed)
			{
				std::cout << "Please provide the source file path !";
				return ErrorCodes::NoSourceFile;
			}
			else
			{
				return ErrorCodes::NoError;
			}
		}
		break;
		case 1:
		{
			std::cout << "Please provide the destination file path !";
			return ErrorCodes::NoDestinationFile;
		}
		break;
		case 2:
		{
			return generateData();
		}
		break;
		default :
		{
			std::cout << "Too many parameters !";
			return ErrorCodes::TooManyParameters;
		}
		break;
	}

	return ErrorCodes::UnknownError;
}

void displayHelp(char* argv0)
{
	std::cout << "Usage :" << std::endl
		<< argv0 << " [/h] [--help] <binaryFileName.*> <resultFileName.h/cs/java>" << std::endl;
}

void processCommandLineArguments(int argc, char* argv[])
{
	if (1 == argc)
	{
		displayHelp(argv[0]);
		isHelpDisplayed = true;
	}
	else
	{
		for (int i = 1; i < argc; ++i)
		{
			if ((std::string(argv[i]) == "/h") || (std::string(argv[i]) == "--help"))
			{
				if (!isHelpDisplayed)
				{//we can use both "/h" and "--help". Do it only ONCE !
					displayHelp(argv[0]);
					isHelpDisplayed = true;
				}
			}
			else
			{
				if (0 == mandatoryParamsCount)
				{
					srcFilePath = argv[i];
					mandatoryParamsCount++;
				}
				else
				{
					destFilePath = argv[i];
					mandatoryParamsCount++;
				}
			}
		}

	}

}

int generateData()
{
	//Let's read the data first
	std::unique_ptr<uint8_t[]> data;
	uint64_t dataLength = std::filesystem::file_size(srcFilePath);

	std::fstream fs1(srcFilePath, std::ios::in | std::ios::binary);

	if (fs1.good())
	{
		dataLength = std::filesystem::file_size(srcFilePath);
		data = std::make_unique<uint8_t[]>(dataLength);
		fs1.read(reinterpret_cast<char*>(data.get()), dataLength);
	}
	else
	{
		fs1.close();
		return ErrorCodes::CannotReadInputFile;
	}

	fs1.close();

	//Let's generate the output file
	std::ostringstream ss;

	std::vector<uint8_t> arr = { 1,2,3,4,5,6,7,8,9 };

	//Add the credits
	ss << "//Created under MIT license with DiResGen : " << gitHubPage << std::endl;
	ss << std::endl;

	//Add include for the c++ language
	ss << "#include <vector>" << std::endl;
	ss << std::endl;

	//Add the file content
	ss << "unsigned long long dataLength = " << dataLength << ";" << std::endl;
	ss << "std::vector<uint8_t> data = {" << std::endl;

	//Add all the elements and a comma and a space, except for the last one
	for (int i = 0; i < dataLength - 1; ++i)
	{
		//If we add an uint8_t to the stringstream, it will be added as a character. Let's convert to 4 byte integer so the hex conversion is ok
		ss << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << uint32_t(data[i]);
		ss << std::dec << ", ";

		if ((i + 1) % 16 == 0)
			ss << std::endl;
	}
	//Add the last element,also
	ss << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << uint32_t(data[dataLength - 1]);
	//Add the "footer"
	ss << std::endl << "};";

	//And write the content to the file
	std::fstream fs2(destFilePath, std::ios::out | std::ios::trunc);

	if (fs2.good())
	{
		fs2 << ss.str();
		std::cout << "Result  written on \'" << destFilePath << "\'";
	}
	else
	{
		fs2.close();
		return ErrorCodes::CannotWriteOutputFile;
	}

	fs2.close();

	return ErrorCodes::NoError;
}
