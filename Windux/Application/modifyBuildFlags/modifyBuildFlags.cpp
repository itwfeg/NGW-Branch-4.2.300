// modifyBuildFlags.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <fstream>
#include <iostream>

int updateProjectFile(const std::string & path, const std::string & argsList, const std::string & target)
{
	std::string fileData("");

	std::ifstream projFile (path.c_str());

	if (projFile.rdbuf( )->is_open( ))
	{
		char buff[1024];
		int buffSize = sizeof (buff);
		int bytesRead = 0;

		do
		{

			projFile.read(buff, buffSize);

			bytesRead = projFile.gcount();

			if (bytesRead > 0)
			{
				fileData.append(buff, bytesRead);

			}
		}
		while (bytesRead > 0);

		projFile.close();
	}

	if (fileData.length() > 0)
	{
		std::string::size_type nPos = 0;
		bool bInserted = false;

		do
		{

			nPos= fileData.find("PreprocessorDefinitions", nPos);


			if (nPos != std::string::npos)
			{

				nPos = fileData.find('"', nPos);
				nPos = fileData.find('"', nPos + 1);
				fileData.insert(nPos, argsList);

				bInserted = true;
			}
		}
		while (nPos != std::string::npos);


		if (bInserted)
		{

			std::ofstream out(target.c_str(), std::ios::out|std::ios::trunc);

			if (out)
			{

				out << fileData.data();

				out.close();
			}
		}
	}

	return 0;

}

void wcharToString(wchar_t * source, std::string & target)
{
	int len = wcslen(source);

	for (int cntr = 0; cntr < len; cntr++)
	{

		target.append(1,(char)source[cntr]);

	}

}

int _tmain(int argc, _TCHAR* argv[])
{
	std::string source, target, arguments;
	//argv[1] source full path
	
	if (argc > 1)
	{
		wcharToString((wchar_t *)argv[1], source);

		target = source;

		std::string::size_type nPos = target.find('.');

		if (nPos != std::string::npos)
		{
			target.insert(nPos, "_New");
		}

	}
	else
		return -1;

	for (int cntr = 3; cntr <= argc; cntr++)
	{
		arguments += std::string(";");
		wcharToString((wchar_t *)argv[cntr-1], arguments);

	}

	if ((source.length()) && arguments.length())
		updateProjectFile(source, arguments, target);

	return 0;
}

