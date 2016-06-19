/* 

Amine Rehioui
Created: April 22nd 2012

*/

#pragma once


namespace shoot
{
	//! PNG loader
	class PNGLoader
	{
	public:

		static void* LoadFromRawPng(const std::string& path, int& width, int& height, int& channels, int& bpp, File* binFile, bool force32Bits);
	};
}



