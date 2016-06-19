/* 

Amine Rehioui
Created: March 30th 2012

*/

#include "Shoot.h"

#include "GeometryProvider.h"

#include "FakeCubeMapTexture.h"

#include "GraphicExtensionHandler.h"

#include "Utils.h"

namespace shoot
{
	//! constructor
	GeometryProvider::GeometryProvider()
	{
	}

	//! destructor
	GeometryProvider::~GeometryProvider()
	{
	}	

	//! returns a 2D quad vertex buffer
	VertexBuffer* GeometryProvider::Get2DQuad(const Vector2& vSize, const Vector2& vUV)
	{
		for(auto it = m_l2DQuads.begin(); it != m_l2DQuads.end(); )
		{
			if(auto pVB = (*it).m_VB)
			{
				if((*it).m_vSize == vSize)				
					return pVB;				
				++it;
			}
			else
				it = m_l2DQuads.erase(it);			
		}
		VertexBuffer* pVB = Create2DQuad(vSize, vUV);
		QuadInfo info = { vSize, Handle<VertexBuffer>(pVB)};
		m_l2DQuads.push_back(info);
		return pVB;
	}

	//! Creates a 2D quad vertex buffer
	VertexBuffer* GeometryProvider::Create2DQuad(const Vector2& vSize, const Vector2& vUV)
	{
		VertexBuffer* pVB = GraphicsDriver::Instance()->CreateVertexBuffer();
		pVB->SetName("2DQuad");

		Vertex3D* pVertices = snew Vertex3D[4];
		pVertices[0].UV = Vector2::Create(vUV.X, 0.0f);  pVertices[0].Pos = Vector3::Create(vSize.X, 0.0f, 0.0f); // Top Right
		pVertices[1].UV = Vector2::Create(vUV.X, vUV.Y); pVertices[1].Pos = Vector3::Create(vSize.X, vSize.Y, 0.0f); // Bottom Right
		pVertices[2].UV = Vector2::Create(0.0f, 0.0f);   pVertices[2].Pos = Vector3::Zero; // Top Left		
		pVertices[3].UV = Vector2::Create(0.0f, vUV.Y);  pVertices[3].Pos = Vector3::Create(0.0f, vSize.Y, 0.0f); // Bottom Left
		pVB->SetVertices(pVertices, 4, Vertex3D::VF_Pos|Vertex3D::VF_UV);
		pVB->SetPrimitiveType(GraphicsDriver::PT_TriangleStrip);
		pVB->SetDynamic(true);
		return pVB;
	}

	//! Creates a 3D quad vertex buffer
	VertexBuffer* GeometryProvider::Create3DQuad(const Vector2& vScale, bool bHasVertexColors)
	{
		VertexBuffer* pVB = GraphicsDriver::Instance()->CreateVertexBuffer();		
		pVB->SetName("3DQuad");

		int format = Vertex3D::VF_Pos|Vertex3D::VF_UV;
		Vertex3D* pVertices = snew Vertex3D[4];
		pVertices[0].UV = Vector2::Create(0.0f, 0.0f); pVertices[0].Pos = Vector3::Create(-vScale.X, vScale.Y, 0.0f);
		pVertices[1].UV = Vector2::Create(1.0f, 0.0f); pVertices[1].Pos = Vector3::Create(vScale.X, vScale.Y, 0.0f);
		pVertices[2].UV = Vector2::Create(0.0f, 1.0f); pVertices[2].Pos = Vector3::Create(-vScale.X, -vScale.Y, 0.0f);
		pVertices[3].UV = Vector2::Create(1.0f, 1.0f); pVertices[3].Pos = Vector3::Create(vScale.X, -vScale.Y, 0.0f);
		if(bHasVertexColors)
		{
			pVertices[0].color = Color::White;
			pVertices[1].color = Color::White;
			pVertices[2].color = Color::White;
			pVertices[3].color = Color::White;
			format |= Vertex3D::VF_Color;
		}
		pVB->SetVertices(pVertices, 4, format);
		pVB->SetPrimitiveType(GraphicsDriver::PT_TriangleStrip);
		pVB->SetDynamic(true);
		return pVB;
	}

	//! returns a fullscreen quad
	VertexBuffer* GeometryProvider::GetFullScreenQuad()
	{
		if (m_FullScreenQuad.IsValid())
			return m_FullScreenQuad;

		m_FullScreenQuad = GraphicsDriver::Instance()->CreateVertexBuffer();
		m_FullScreenQuad->SetName("FullScreenQuad");
		Vertex3D* pVertices = snew Vertex3D[4];
		pVertices[0].UV = Vector2::Create(1.0f, 1.0f); pVertices[0].Pos = Vector3::Create(1.0f, 1.0f, 0.0f); // Top Right
		pVertices[1].UV = Vector2::Create(1.0f, 0.0f); pVertices[1].Pos = Vector3::Create(1.0f, -1.0f, 0.0f); // Bottom Right			
		pVertices[2].UV = Vector2::Create(0.0f, 1.0f); pVertices[2].Pos = Vector3::Create(-1.0f, 1.0f, 0.0f); // Top Left
		pVertices[3].UV = Vector2::Create(0.0f, 0.0f); pVertices[3].Pos = Vector3::Create(-1.0f, -1.0f, 0.0f); // Bottom Left			
		m_FullScreenQuad->SetVertices(pVertices, 4, Vertex3D::VF_Pos | Vertex3D::VF_UV);
		m_FullScreenQuad->SetPrimitiveType(GraphicsDriver::PT_TriangleStrip);
		m_FullScreenQuad->SetApplyWorldTransforms(false);
		m_FullScreenQuad->SetDynamic(true);
		return m_FullScreenQuad;
	}

	//! updates a text vertex buffer
	void GeometryProvider::UpdateTextVertexBuffer(VertexBuffer* pVB, int fontSize, const std::wstring& text, const Size& area, Text::E_Alignement alignement, int lineSpacing, std::map<wchar_t, Text::CharData>& charMap)
	{
		std::vector<std::wstring> words;
		Utils::Tokenize(text.c_str(), L" ", words);

		std::vector< LineInfo > lines;
		int currentLine = 0;
		int pixelsRemainingInLine = area.Width;
		size_t totalCharacters = 0;
		int spaceWidth = (charMap.find(L' ') != charMap.end()) ? int(charMap[L' '].vAdvance.X) : 0;
		for(size_t currentWord=0; currentWord < words.size(); ++currentWord)
		{
			if(currentLine == lines.size())
			{
				lines.push_back(LineInfo());
			}

			int wordWidth = GetTextWidth(words[currentWord], charMap);
			if(wordWidth < pixelsRemainingInLine || lines[currentLine].words.empty())
			{
				pixelsRemainingInLine -= wordWidth;

				// consume one space
				int spaceWidth = (charMap.find(L' ') != charMap.end()) ? int(charMap[L' '].vAdvance.X) : 0;
				pixelsRemainingInLine -= spaceWidth;
				if(!lines[currentLine].words.empty())
				{
					lines[currentLine].width += spaceWidth;
					++totalCharacters;
				}
				
				auto word = words[currentWord];
				lines[currentLine].words.push_back(word);
				lines[currentLine].width += wordWidth;
				totalCharacters += word.length();
			}
			else
			{
				pixelsRemainingInLine = area.Width;
				++currentLine;
				--currentWord;
			}
		}

		uint numVertices = (uint)totalCharacters*6;
		if(numVertices > pVB->GetMaxVertices())
		{
			Vertex3D* pVertices = snew Vertex3D[numVertices];
			pVB->SetVertices(pVertices, numVertices, Vertex3D::VF_Pos|Vertex3D::VF_UV);
			pVB->SetPrimitiveType(GraphicsDriver::PT_Triangle);
		}
		pVB->SetNumVertices(0);
		pVB->SetMaxVertices(numVertices);
		pVB->SetDirty(true);

		int currentCharacter = 0;
		for (size_t i=0; i<lines.size(); ++i)
		{
			Vector3 vCharacterPos = Vector3::Create(0.0f, area.Height + float(i*lineSpacing), 0.0f);

			switch(alignement)
			{
			case Text::A_Center:
				vCharacterPos.X = (float(area.Width)-lines[i].width)/2.0f;
				break;

			case Text::A_Right:
				vCharacterPos.X = float(area.Width)-lines[i].width;
				break;
			}

			for(size_t j=0; j<lines[i].words.size(); ++j)
			{
				for (size_t k = 0; k<lines[i].words[j].length(); ++k)
				{
					auto c = lines[i].words[j].at(k);
					if(charMap.find(c) == charMap.end())
					{
						SHOOT_WARNING(false, "TextTTF: CharData for '%c' not found", c);
						continue;
					}
					const Text::CharData& d = charMap[c];
					AddCharacter(pVB, d, vCharacterPos, currentCharacter++);
					vCharacterPos += d.vAdvance;
				}

				if(j+1 < lines[i].words.size())
				{
					wchar_t c = L' ';
					if(charMap.find(c) == charMap.end())
					{
						SHOOT_WARNING(false, "TextTTF: CharData for '%c' not found", c);
						continue;
					}
					const Text::CharData& d = charMap[c];
					AddCharacter(pVB, d, vCharacterPos, currentCharacter++);					
					vCharacterPos += d.vAdvance;
				}
			}
		}
	}

	//! gets a sky box vertex buffer
	VertexBuffer* GeometryProvider::GetSkyBoxVertexBuffer(Texture* pTexture)
	{
		return CreateSkyBoxVertexBufferNoCubemap(pTexture);
	}

	//! creates a sky box vertex buffer when no cube map support
	VertexBuffer* GeometryProvider::CreateSkyBoxVertexBufferNoCubemap(Texture* pTexture)
	{
		VertexBuffer* pVB = GraphicsDriver::Instance()->CreateSkyBoxVertexBufferNoCubemap(pTexture);		

		Vertex3D* pVertices = snew Vertex3D[36];
		float s = 1.0f;
		// matches texels with pixels - uses same method as the Irrlicht engine
		float onePixel = pTexture ? (1.0f / (pTexture->GetSize().X * 1.5f)) : 0.0f;
		float o = 0.0f + onePixel;
		float t = 1.0f - onePixel;

		// -X
		pVertices[6*0+0].Pos = Vector3::Create(-s, -s, -s);	pVertices[6*0+0].UV = Vector2::Create(o, t);
		pVertices[6*0+1].Pos = Vector3::Create(-s, -s, s);		pVertices[6*0+1].UV = Vector2::Create(o, o);
		pVertices[6*0+2].Pos = Vector3::Create(-s, s, s);		pVertices[6*0+2].UV = Vector2::Create(t, o);
		pVertices[6*0+3].Pos = Vector3::Create(-s, -s, -s);	pVertices[6*0+3].UV = Vector2::Create(o, t);
		pVertices[6*0+4].Pos = Vector3::Create(-s, s, s);		pVertices[6*0+4].UV = Vector2::Create(t, o);
		pVertices[6*0+5].Pos = Vector3::Create(-s, s, -s);		pVertices[6*0+5].UV = Vector2::Create(t, t);

		// +X
		pVertices[6*1+0].Pos = Vector3::Create(s, s, -s);		pVertices[6*1+0].UV = Vector2::Create(o, t);
		pVertices[6*1+1].Pos = Vector3::Create(s, s, s);		pVertices[6*1+1].UV = Vector2::Create(o, o);
		pVertices[6*1+2].Pos = Vector3::Create(s, -s, s);		pVertices[6*1+2].UV = Vector2::Create(t, o);
		pVertices[6*1+3].Pos = Vector3::Create(s, s, -s);		pVertices[6*1+3].UV = Vector2::Create(o, t);
		pVertices[6*1+4].Pos = Vector3::Create(s, -s, s);		pVertices[6*1+4].UV = Vector2::Create(t, o);
		pVertices[6*1+5].Pos = Vector3::Create(s, -s, -s);		pVertices[6*1+5].UV = Vector2::Create(t, t);

		// -Z
		pVertices[6*2+0].Pos = Vector3::Create(-s, s, -s);		pVertices[6*2+0].UV = Vector2::Create(o, o);
		pVertices[6*2+1].Pos = Vector3::Create(s, s, -s);		pVertices[6*2+1].UV = Vector2::Create(t, o);
		pVertices[6*2+2].Pos = Vector3::Create(-s, -s, -s);	pVertices[6*2+2].UV = Vector2::Create(o, t);
		pVertices[6*2+3].Pos = Vector3::Create(-s, -s, -s);	pVertices[6*2+3].UV = Vector2::Create(o, t);
		pVertices[6*2+4].Pos = Vector3::Create(s, s, -s);		pVertices[6*2+4].UV = Vector2::Create(t, o);
		pVertices[6*2+5].Pos = Vector3::Create(s, -s, -s);		pVertices[6*2+5].UV = Vector2::Create(t, t);

		// +Z
		pVertices[6*3+0].Pos = Vector3::Create(-s, s, s);		pVertices[6*3+0].UV = Vector2::Create(o, t);
		pVertices[6*3+1].Pos = Vector3::Create(-s, -s, s);		pVertices[6*3+1].UV = Vector2::Create(o, o);
		pVertices[6*3+2].Pos = Vector3::Create(s, -s, s);		pVertices[6*3+2].UV = Vector2::Create(t, o);
		pVertices[6*3+3].Pos = Vector3::Create(-s, s, s);		pVertices[6*3+3].UV = Vector2::Create(o, t);
		pVertices[6*3+4].Pos = Vector3::Create(s, -s, s);		pVertices[6*3+4].UV = Vector2::Create(t, o);
		pVertices[6*3+5].Pos = Vector3::Create(s, s, s);		pVertices[6*3+5].UV = Vector2::Create(t, t);

		// +Y
		pVertices[6*4+0].Pos = Vector3::Create(-s, s, -s);		pVertices[6*4+0].UV = Vector2::Create(o, t);
		pVertices[6*4+1].Pos = Vector3::Create(-s, s, s);		pVertices[6*4+1].UV = Vector2::Create(o, o);
		pVertices[6*4+2].Pos = Vector3::Create(s, s, s);		pVertices[6*4+2].UV = Vector2::Create(t, o);
		pVertices[6*4+3].Pos = Vector3::Create(-s, s, -s);		pVertices[6*4+3].UV = Vector2::Create(o, t);
		pVertices[6*4+4].Pos = Vector3::Create(s, s, s);		pVertices[6*4+4].UV = Vector2::Create(t, o);
		pVertices[6*4+5].Pos = Vector3::Create(s, s, -s);		pVertices[6*4+5].UV = Vector2::Create(t, t);

		// -Y
		pVertices[6*5+0].Pos = Vector3::Create(s, -s, -s);		pVertices[6*5+0].UV = Vector2::Create(o, t);
		pVertices[6*5+1].Pos = Vector3::Create(s, -s, s);		pVertices[6*5+1].UV = Vector2::Create(o, o);
		pVertices[6*5+2].Pos = Vector3::Create(-s, -s, s);		pVertices[6*5+2].UV = Vector2::Create(t, o);
		pVertices[6*5+3].Pos = Vector3::Create(s, -s, -s);		pVertices[6*5+3].UV = Vector2::Create(o, t);
		pVertices[6*5+4].Pos = Vector3::Create(-s, -s, s);		pVertices[6*5+4].UV = Vector2::Create(t, o);
		pVertices[6*5+5].Pos = Vector3::Create(-s, -s, -s);	pVertices[6*5+5].UV = Vector2::Create(t, t);

		pVB->SetVertices(pVertices, 36, Vertex3D::VF_Pos|Vertex3D::VF_UV);
		pVB->SetNumVertices(6); // will be rendered 6 vertices (1 face) at a time
		pVB->SetDynamic(true);
		return pVB;
	}

	//! adds a character to the vertex buffer
	void GeometryProvider::AddCharacter(VertexBuffer* pVB, const Text::CharData& d, const Vector3& vCharacterPos, int currentCharacter)
	{
		Vertex3D* pVertices = reinterpret_cast<Vertex3D*>(pVB->GetVertices());
		int i = currentCharacter;
		pVertices[i*6+0].UV = Vector2::Create(d.UVMin.X, d.UVMin.Y); pVertices[i*6+0].Pos = vCharacterPos + d.vOffset + Vector3::Zero; // Top Left
		pVertices[i*6+1].UV = Vector2::Create(d.UVMax.X, d.UVMin.Y); pVertices[i*6+1].Pos = vCharacterPos + d.vOffset + Vector3::Create(d.vSize.X, 0.0f, 0.0f); // Top Right
		pVertices[i*6+2].UV = Vector2::Create(d.UVMax.X, d.UVMax.Y); pVertices[i*6+2].Pos = vCharacterPos + d.vOffset + Vector3::Create(d.vSize.X, d.vSize.Y, 0.0f); // Bottom Right
		pVertices[i*6+3].UV = Vector2::Create(d.UVMin.X, d.UVMax.Y); pVertices[i*6+3].Pos = vCharacterPos + d.vOffset + Vector3::Create(0.0f, d.vSize.Y, 0.0f); // Bottom Left
		pVertices[i*6+4].UV = Vector2::Create(d.UVMin.X, d.UVMin.Y); pVertices[i*6+4].Pos = vCharacterPos + d.vOffset + Vector3::Zero; // Top Left
		pVertices[i*6+5].UV = Vector2::Create(d.UVMax.X, d.UVMax.Y); pVertices[i*6+5].Pos = vCharacterPos + d.vOffset + Vector3::Create(d.vSize.X, d.vSize.Y, 0.0f); // Bottom Right
	}

	//! returns the text width in pixels
	int GeometryProvider::GetTextWidth(const std::wstring& text, std::map<wchar_t, Text::CharData>& charMap)
	{
		int width = 0;
		for (size_t i = 0; i<text.length(); ++i)
		{
			auto c = text.at(i);
			if(charMap.find(c) == charMap.end())
			{
				continue;
			}
			width += int(charMap[c].vAdvance.X);
		}
		return width;
	}
}
