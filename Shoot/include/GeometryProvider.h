/* 

Amine Rehioui
Created: March 30th 2012

*/

#pragma once


#include "VertexBuffer.h"
#include "Text.h"

namespace shoot
{
	// forwards
	class Texture;

	//! Geometry provider
	class GeometryProvider : public Singleton<GeometryProvider>
	{
		DECLARE_SINGLETON(GeometryProvider);

	public:

		//! returns a 2D quad vertex buffer
		VertexBuffer* Get2DQuad(const Vector2& vSize, const Vector2& vUV);

		//! Creates a 3D quad vertex buffer
		VertexBuffer* Create3DQuad(const Vector2& vScale, bool bHasVertexColors);

		//! returns a fullscreen quad
		VertexBuffer* GetFullScreenQuad();

		//! updates a text vertex buffer
		void UpdateTextVertexBuffer(VertexBuffer* pVB, int fontSize, const std::wstring& text, const Size& area, Text::E_Alignement alignement, int lineSpacing, std::map<wchar_t, Text::CharData>& charMap);

		//! gets a sky box vertex buffer
		VertexBuffer* GetSkyBoxVertexBuffer(Texture* pTexture);

	private:

		//! Creates a 2D quad vertex buffer
		VertexBuffer* Create2DQuad(const Vector2& vSize, const Vector2& vUV);

		//! creates a sky box vertex buffer
		VertexBuffer* CreateSkyBoxVertexBuffer();

		//! creates a sky box vertex buffer when no cube map support
		VertexBuffer* CreateSkyBoxVertexBufferNoCubemap(Texture* pTexture);

		//! adds a character to the vertex buffer
		void AddCharacter(VertexBuffer* pVB, const Text::CharData& d, const Vector3& vCharacterPos, int currentCharacter);

		//! returns the text width in pixels
		int GetTextWidth(const std::wstring& text, std::map<wchar_t, Text::CharData>& charMap);

		// geometry pools
		struct QuadInfo
		{
			Vector2 m_vSize;
			Handle<VertexBuffer> m_VB;
		};	

		std::list<QuadInfo> m_l2DQuads;
		Reference<VertexBuffer> m_FullScreenQuad;

		//! line info
		struct LineInfo
		{
			LineInfo()
				: width(0)
			{
			}
			int width;
			std::vector<std::wstring> words;
		};
	};
}




