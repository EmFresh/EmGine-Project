#include <memory>
#include "fileIO.h"
			  
int  fileIO::readFileToBuffer(cstring path, unsigned char * buffer)
{
	auto file_s = std::unique_ptr<FILE>();
	auto file = file_s.get();
	if(fopen_s((&file), path, "rb") == 0)
	{
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		fseek(file, 0, SEEK_SET);
		file_s ? "not null" : "null";

		buffer = new unsigned char[size];
	
			//char buff;
			fread(buffer, sizeof(char), size, file);
			for(int a = 0; a < size;a++)
			{
				printf("%c",buffer[a]);
			}
		fclose(file);
		delete[] buffer;
		return size;
	}

	return false;
}
