/* 

Amine Rehioui
Created: April 21th 2010
TTF support: May 23rd 2014

*/

#pragma once


namespace shoot
{
	//! Text entity
	class Text : public Entity2D				 
	{
	public:

		//! Alignement
		enum E_Alignement
		{
			A_Left,
			A_Center,
			A_Right
		};

		//! FontType
		enum E_FontType
		{
			FT_Numbers,
			FT_NumbersAndLetters,
			FT_Dynamic
		};

		//! CharData
		struct CharData
		{
			Vector2 UVMin, UVMax, vSize;
			Vector3 vOffset, vAdvance;
		};

		//! FontInfo
		class FontInfo : public Object
		{
		public:

			FontInfo()
				: m_pData(NULL)
			{
			}

			~FontInfo()
			{
				sdelete_array(m_pData);
			}

			E_FontType m_Type;
			uint m_Size;
			Reference<MemoryTexture> m_Texture;
			std::map<wchar_t, Text::CharData> m_CharData;
			uchar* m_pData;
		};

		//! Macro to provide type information
		DECLARE_OBJECT(Text, Entity2D);

		//! constructor
		Text();

		//! destructor
		~Text();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		virtual void Init();
			
		//! registers the entity and its children for rendering		
		virtual void RegisterForRendering();

		//! Get the local axis-aligned bounding box
		virtual AABBox2D GetBoundingBox() const;

		//! Reloads the font given a font path
		void SetFont(std::string strPath);

		//! sets the text
		void SetText(const std::wstring& strText);

		//! sets the text
		inline void SetText(const std::string& strText) { SetText(std::wstring(strText.begin(), strText.end())); }

		//! sets the color
		void SetColor(const Color& color);

		//! returns the text
		const std::wstring& GetText() const { return m_strText; }

		//! sets the desired vertex count used for rendering
		inline void SetDesiredVertexCount(int count) { m_DesiredVertexCount = count; }

		//! sets the text area size
		inline void SetArea(const Size& area) { m_Area = area; }

		//! pre-rendering
		void PreRender();

	protected:

		//! updates the geometry
		void UpdateGeometry(const std::wstring& text, const Color& color);

		bool m_bDirty;
		int m_DesiredVertexCount;
		Reference<FontInfo> m_FontInfo;

		// properties
		E_FontType m_FontType;
		std::wstring m_strText;
		IPath m_Font;
		uint m_FontSize;
		Size m_Area;		
		E_Alignement m_eAlignement;
		uint m_LineSpacing;
		Color m_Color;
	};
}



