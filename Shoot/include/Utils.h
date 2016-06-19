/* 

Amine Rehioui
Created: May 3rd 2012

*/

#pragma once


namespace shoot
{
	// utils
	class Utils
	{
	public:

		//! formats a resource path
		static std::string FormatResourcePath(const std::string& path);

		//! tokenizes a string
		static void Tokenize(const char* pStr, const char* pDelim, std::vector<std::string>& tokens);

		//! tokenizes a string
		static void Tokenize(const wchar_t* pStr, const wchar_t* pDelim, std::vector<std::wstring>& tokens);

		//! returns an file extension from a file path
		static std::string GetFileExtension(const std::string& path);

		//! returns a file path with no extension
		static std::string GetFilePathNoExt(const std::string& path);

		//! GetFileName
		static std::string GetFileName(const std::string& path);

		//! string comparison
		static bool Equals(const char* str1, const char* str2, bool bCaseSensitive = false);

		//! string format
		static std::string StringFormat(const char* format, ...);

		//! reads text from a file
		static char* ReadFileText(const char* path);

		//! returns true if more recent
		static bool IsFileMoreRecent(const std::string& file, const std::string& other);		

		//! returns the bin path
		static std::string GetBinPath(const std::string& file);

		//! creates a binary file
		static File* CreateBinFile(const std::string& binPath);

		//! converts a native type to a string
		template <class T>
		static std::string ToString(T value)
		{
			std::stringstream ss(std::stringstream::in | std::stringstream::out);
			ss << value;
			std::string result = ss.str();
			return result;
		}

		//! converts a native type to a wstring
		template <class T>
		static std::wstring ToWString(T value)
		{
			std::wstringstream ss(std::wstringstream::in | std::wstringstream::out);
			ss << value;
			auto result = ss.str();
			return result;
		}

		//! converts a string to a native type
		template <class T>
		static T FromString(const std::string& str)
		{
			std::stringstream ss(std::stringstream::in | std::stringstream::out);
			ss << str;
			T t;
			ss >> t;
			return t;
		}
	};
}



