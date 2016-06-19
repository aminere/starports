/*

Amine Rehioui
Created: April 12th 2015

*/

#include "Shoot.h"

#include "FontManager.h"

#include "File.h"

#include <ft2build.h>
#include <freetype.h>

namespace shoot
{
	//! Constructor
	FontManager::FontManager()
	{
	}

	//! Destructor
	FontManager::~FontManager()
	{
	}

	//! GetFont
	Text::FontInfo* FontManager::GetFont(Text::E_FontType type, uint size, const std::wstring& text, MemoryTexture* previousTexture)
	{
		if (type != Text::FT_Dynamic)
		{
			for (auto& font : m_FontInfos)
			{
				if (font->m_Type == type && font->m_Size == size)				
					return font;
			}
		}		

		// create a new font
		auto info = _CreateFont(type, size, text, previousTexture);

		if (type != Text::FT_Dynamic)
			m_FontInfos.push_back(Reference<Text::FontInfo>(info));

		return info;
	}

	//! CreateFont
	Text::FontInfo* FontManager::_CreateFont(Text::E_FontType type, uint size, const std::wstring& text, MemoryTexture* previousTexture)
	{
		FT_Library ft;
		SHOOT_VERIFY(!FT_Init_FreeType(&ft), "FT_Init_FreeType failed");
		FT_Face face;
		std::string path = std::string("common/default.ttf");

		auto fontFile = File::Create(path.c_str(), File::M_ReadBinary);
		fontFile->Open();
		auto fontBuffer = snew FT_Byte[fontFile->GetSize()];
		fontFile->Read(fontBuffer, fontFile->GetSize());
		SHOOT_VERIFY(!FT_New_Memory_Face(ft, fontBuffer, (FT_Long)fontFile->GetSize(), 0, &face), "FT_New_Memory_Face failed");
		FT_Set_Pixel_Sizes(face, 0, size);

		// determine characters
		auto _text = GetFontText(type, text);
		std::list<GlyphInfo> glyphs;
		Text::FontInfo* info = snew Text::FontInfo();
		info->m_Type = type;
		info->m_Size = size;
		for (size_t i = 0; i < _text.length(); ++i)
		{
			auto c = _text.at(i);
			if (info->m_CharData.find(c) != info->m_CharData.end())
				continue;

			SHOOT_VERIFY(!FT_Load_Char(face, c, FT_LOAD_RENDER), "FT_Load_Char failed");
			info->m_CharData[c] = Text::CharData();
			FT_GlyphSlot g = face->glyph;
			GlyphInfo glyphInfo;
			glyphInfo.c = c;
			SHOOT_ASSERT(g->bitmap.width == g->bitmap.pitch, "Glyph width is different from pitch");
			glyphInfo.width = g->bitmap.width;
			glyphInfo.height = g->bitmap.rows;
			glyphInfo.top = g->bitmap_top;
			glyphInfo.left = g->bitmap_left;
			glyphInfo.advance = Vector2::Create(float(g->advance.x >> 6), float(g->advance.y >> 6));
			auto dataSize = glyphInfo.width * glyphInfo.height;
			glyphInfo.data = snew uchar[dataSize];
			memcpy(glyphInfo.data, g->bitmap.buffer, dataSize);
			glyphs.push_back(glyphInfo);
		}

		auto atlasSize = GetBestAtlasSize(glyphs);

		// Fill data & determine character mapping info
		Point curOffset(0, 0);
		int curMaxY = 0;
		int dataSize = atlasSize.Width*atlasSize.Height * 2;
		info->m_pData = snew uchar[dataSize];
		memset(info->m_pData, 0, dataSize);
		Size padding(1, 1);
		for (auto& glyph : glyphs)
		{
			curMaxY = Math::Max(curMaxY, glyph.height);
			if (curOffset.Y + glyph.height > atlasSize.Height)
			{
				SHOOT_ASSERT(false, "FontManager: Texture too small");
				break;
			}
			else if (curOffset.X + glyph.width > atlasSize.Width)
			{
				if (curOffset.Y + curMaxY + padding.Height + glyph.height > atlasSize.Height)
				{
					SHOOT_ASSERT(false, "FontManager: Texture too small");
					break;
				}
				curOffset.X = 0;
				curOffset.Y += (curMaxY + padding.Height);
				curMaxY = 0;
			}

			for (int y = 0; y < glyph.height; ++y)
			{
				for (int x = 0; x < glyph.width; ++x)
				{
					auto index = 2 * (atlasSize.Width*(curOffset.Y + y) + (curOffset.X + x));
					SHOOT_ASSERT(index + 1 < dataSize, "Font data index out of bounds");
					uchar* pixel = &info->m_pData[index];
					pixel[0] = pixel[1] = glyph.data[glyph.width*y + x];
				}
			}

			sdelete_array(glyph.data);

			Text::CharData charData =
			{
				Vector2::Create(float(curOffset.X) / atlasSize.Width, float(curOffset.Y) / atlasSize.Height),
				Vector2::Create(float(curOffset.X + glyph.width) / atlasSize.Width, float(curOffset.Y + glyph.height) / atlasSize.Height),
				Vector2::Create(float(glyph.width), float(glyph.height)),
				Vector3::Create(float(glyph.left), -float(glyph.top), 0.0f),
				Vector3::Create(glyph.advance, 0.0f)
			};

			info->m_CharData[glyph.c] = charData;
			curOffset.X += (glyph.width + padding.Width);
		}

		FT_Done_Face(face);
		FT_Done_FreeType(ft);
		sdelete_array(fontBuffer);
		fontFile->Close();
		sdelete(fontFile);


		if (previousTexture)		
			previousTexture->UpdateData(info->m_pData, Vector2::Create(float(atlasSize.Width), float(atlasSize.Height)));
		else
		{
			info->m_Texture = snew MemoryTexture(info->m_pData, Vector2::Create(float(atlasSize.Width), float(atlasSize.Height)));
		}

		return info;
	}

	//! GetFontText
	std::wstring FontManager::GetFontText(Text::E_FontType type, const std::wstring& text)
	{
		switch (type)
		{
		case Text::FT_Dynamic: 
			return text;

		case Text::FT_Numbers:
		{
			std::wstring out;
			for (int i = 0; i < 10; ++i)
				out += Utils::ToWString(i);
			return out;
		}
			
		default:
		{
			std::wstring out;
			for (int i = 0; i < 10; ++i)
				out += Utils::ToWString(i);

			for (wchar_t c = L'A'; c <= L'Z'; ++c)
				out += c;

			out += L" .,:()-'";
			return out;
		}
		}		
	}

	//! GetBestAtlasSize
	Size FontManager::GetBestAtlasSize(std::list<GlyphInfo>& glyphs)
	{
		Size size(32, 32);
		Size padding(1, 1);

		while (true)
		{
			Point curOffset(0, 0);
			int curMaxY = 0;
			bool sizeOK = true;
			for (auto& glyph : glyphs)
			{
				curMaxY = Math::Max(curMaxY, glyph.height);
				if (curOffset.Y + glyph.height > size.Height)
				{
					size = Size(size.Width * 2, size.Height * 2);
					sizeOK = false;
					break;
				}
				else if (curOffset.X + glyph.width > size.Width)
				{
					if (curOffset.Y + curMaxY + padding.Height + glyph.height > size.Height)
					{
						size = Size(size.Width * 2, size.Height * 2);
						sizeOK = false;
						break;
					}
					curOffset.X = 0;
					curOffset.Y += (curMaxY + padding.Height);
					curMaxY = 0;
				}

				curOffset.X += (glyph.width + padding.Width);
			}

			if (sizeOK)
				break;
		}

		return size;
	}
}

