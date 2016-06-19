/* 

Amine Rehioui
Created: April 12th 2015

*/

#pragma once


#include "Resource.h"
#include "Text.h"
//#include "MemoryTexture.h"

struct FT_GlyphSlotRec_;

namespace shoot
{
	//! FontManager
	class FontManager : public Singleton<FontManager>
	{
		DECLARE_SINGLETON(FontManager);

	public:		

		//! GetFont
		Text::FontInfo* GetFont(Text::E_FontType type, uint size, const std::wstring& text, MemoryTexture* previousTexture);
		
	private:

		//! GlyphInfo
		struct GlyphInfo
		{
			int height;
			int width;
			int top;
			int left;
			Vector2 advance;
			uchar* data;
			wchar_t c;
		};

		//! CreateFont
		Text::FontInfo* _CreateFont(Text::E_FontType type, uint size, const std::wstring& text, MemoryTexture* previousTexture);

		//! GetFontText
		std::wstring GetFontText(Text::E_FontType type, const std::wstring& text);

		//! GetBestAtlasSize
		Size GetBestAtlasSize(std::list<GlyphInfo>& glyphs);		

		std::list< Reference<Text::FontInfo> > m_FontInfos;
	};
}



