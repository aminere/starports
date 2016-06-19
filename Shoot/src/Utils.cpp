/* 

Amine Rehioui
Created: May 3rd 2012

*/

#include "Shoot.h"

#include "Utils.h"

#include "File.h"

#include <string.h>

#include <ctype.h> // ::tolower
#include <wchar.h>

namespace shoot
{
	//! formats a resource path
	std::string Utils::FormatResourcePath(const std::string& path)
	{
		std::vector<std::string> tokens;
		Tokenize(path.c_str(), "/\\", tokens);
		if (std::find(tokens.begin(), tokens.end(), "..") != tokens.end())
        {
			bool _continue = false;
			do 
			{
				std::vector<std::string> tokensOut;
				for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end();)
				{
					std::vector<std::string>::iterator nextToken = it + 1;
					if (nextToken == tokens.end() || (*nextToken) != "..")
					{
						tokensOut.push_back((*it));
						++it;
					}
					else
					{
						it += 2;
					}
				}
				tokens = tokensOut;
				auto it = std::find(tokens.begin(), tokens.end(), "..");
				_continue = (it != tokens.end()) && (it != tokens.begin());
			} while (_continue);
        }
 
        std::string pathOut = tokens[0];
        for(size_t i=1; i<tokens.size(); ++i)
        {
            pathOut = pathOut + "/" + tokens[i];
        }
		return pathOut;
	}

	//! tokenizes a string
	void Utils::Tokenize(const char* pStr, const char* pDelim, std::vector<std::string>& tokens)
	{
		char aStr[1024];		
		strcpy(aStr, pStr);
		char* token = strtok(aStr, pDelim);
		while(token)
		{
			tokens.push_back(std::string(token));
			token = strtok(NULL, pDelim);
		}
	}

	//! tokenizes a string
	void Utils::Tokenize(const wchar_t* pStr, const wchar_t* pDelim, std::vector<std::wstring>& tokens)
	{
		wchar_t aStr[1024];
		wcscpy(aStr, pStr);        

		#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32
			auto token = wcstok(aStr, pDelim);
		#else
			wchar_t *last;
			auto token = wcstok(aStr, pDelim, &last);
		#endif

		while (token)
		{
			tokens.push_back(std::wstring(token));

			#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32
				token = wcstok(NULL, pDelim);
			#else
				token = wcstok(NULL, pDelim, &last);
			#endif
		}
	}

	//! returns an file extension from a file path
	std::string Utils::GetFileExtension(const std::string& path)
	{
		auto extIndex = path.rfind(".");
		SHOOT_ASSERT(extIndex >= 0, "Resource::GetFileExtension: invalid path");
		return path.substr(extIndex+1);
	}

	//! returns a file path with no extension
	std::string Utils::GetFilePathNoExt(const std::string& path)
	{
		auto extIndex = path.rfind(".");
		SHOOT_ASSERT(extIndex >= 0, "Resource::GetFileExtension: invalid path");		
		return path.substr(0, extIndex);
	}

	//! GetFileName
	std::string Utils::GetFileName(const std::string& path)
	{
		auto fileIndex = path.rfind("/");
		if (fileIndex != std::string::npos)
			return path.substr(fileIndex+1);
		return path;
	}

	//! string comparison
	bool Utils::Equals(const char* str1, const char* str2, bool bCaseSensitive /*= false*/)
	{
		std::string s1(str1), s2(str2);
		if(!bCaseSensitive)
		{
			std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
			std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
		}
		return s1 == s2;
	}

	//! string format
	std::string Utils::StringFormat(const char* format, ...)
	{
		static char buffer[1024];
		va_list va;
		va_start(va, format);
		vsprintf(buffer, format, va);
		va_end(va);
		
		return std::string(buffer);
	}

	//! reads text from a file
	char* Utils::ReadFileText(const char* path)
	{
		File* pFile = File::Create(path, File::M_ReadBinary);
		pFile->Open();
		auto fileSize = pFile->GetSize();
		char* pText = snew char[fileSize+1];
		pFile->Read(pText, fileSize);
		pText[fileSize] = '\0';
		pFile->Close();
		sdelete(pFile);
		return pText;
	}

	//! returns true if more recent
	bool Utils::IsFileMoreRecent(const std::string& file, const std::string& other)
	{
#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32
		static auto getFileTime = [](const std::string& path)
		{
			auto wpath = std::wstring(path.begin(), path.end());
			auto hFile = CreateFile(wpath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			SHOOT_ASSERT(hFile != INVALID_HANDLE_VALUE, "CreateFile failed.");
			FILETIME ftCreate, ftAccess, ftWrite;
			SYSTEMTIME stUTC, stLocal;
			SHOOT_VERIFY(GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite), "GetFileTime failed.");
			FileTimeToSystemTime(&ftWrite, &stUTC);
			SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
			CloseHandle(hFile);
			return stLocal;
		};

		auto time = getFileTime(std::string("data/") + file);
		auto reference = getFileTime(std::string("data/") + other);

		if (time.wYear > reference.wYear)
			return true;
		else if (time.wYear < reference.wYear)
			return false;
		else
		{
			if (time.wMonth > reference.wMonth)
				return true;
			else if (time.wMonth < reference.wMonth)
				return false;
			else
			{
				if (time.wDay > reference.wDay)
					return true;
				else if (time.wDay < reference.wDay)
					return false;
				else
				{
					if (time.wHour > reference.wHour)
						return true;
					else if (time.wHour < reference.wHour)
						return false;
					else
					{
						if (time.wMinute > reference.wMinute)
							return true;
						else if (time.wMinute < reference.wMinute)
							return false;
						else
						{
							if (time.wSecond > reference.wSecond)
								return true;
							else if (time.wSecond < reference.wSecond)
								return false;
							else
								return false;
						}
					}
				}
			}
		}
#endif // SHOOT_PLATFORM_WIN32
		return false;
	}

	//! returns the bin path
	std::string Utils::GetBinPath(const std::string& file)
	{
		auto binPath = file;
		std::replace(binPath.begin(), binPath.end(), '/', '_');
		binPath = std::string("../temp/") + binPath;
		return binPath;
	}

#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32
	//! creates a binary file
	File* Utils::CreateBinFile(const std::string& binPath)
	{
		auto binFile = File::CreateNative(binPath.c_str(), File::M_WriteBinary);
		if (!binFile->Open(false))
		{
			SHOOT_VERIFY(CreateDirectory(L"temp", NULL), "CreateDirectory failed");
			binFile->Open();
		}
		return binFile;
	}
#endif
}
