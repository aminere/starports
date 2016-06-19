/* 

Amine Rehioui
Created: April 21th 2010
TTF Support: May 23rd 2014

*/

#include "Shoot.h"

#include "Text.h"

#include "MaterialProvider.h"
#include "GeometryProvider.h"

#include "TextVisitor.h"
#include "FontManager.h"

namespace shoot
{
	DEFINE_OBJECT(Text);

	// Alignement literals
	const char* const g_strAlignementLiterals[] =
	{
		"Left",
		"Center",
		"Right",
		0
	};

	// FontType literals
	const char* const g_strFontTypeLiterals[] =
	{
		"Numbers",
		"NumbersAndLetters",
		"Dynamic",
		0
	};

	//! constructor
	Text::Text()
		: m_bDirty(true)
		, m_DesiredVertexCount(-1)
		, m_Color(Color::White)
		// properties
		, m_FontType(FT_NumbersAndLetters)
		, m_FontSize(14)
		, m_Area(512, 16)
		, m_eAlignement(A_Left)
		, m_LineSpacing(24)
	{
	}

	//! destructor
	Text::~Text()
	{
	}

	//! serializes the entity to/from a PropertyStream
	void Text::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("FontType", &m_FontType, ENUM_PARAMS1(g_strFontTypeLiterals));
		stream.Serialize("Text", &m_strText);
		stream.Serialize("Font", &m_Font);
		stream.Serialize("FontSize", &m_FontSize);
		stream.Serialize("Area", &m_Area);
		stream.Serialize("Alignement", &m_eAlignement, ENUM_PARAMS1(g_strAlignementLiterals));
		stream.Serialize("LineSpacing", &m_LineSpacing);
		stream.Serialize("Color", &m_Color);

		if(IsInitialized() && (stream.GetMode() == SM_Read))
		{
			m_bDirty = true;
		}
	}

	//! called during the initialization of the entity
	void Text::Init()
	{
		SHOOT_ASSERT(!m_Font->empty(), "Creating text with no font");

		GraphicComponent* pGraphic = GetComponent<GraphicComponent>();
		if(!pGraphic)
		{
			pGraphic = snew GraphicComponent();
			pGraphic->SetRenderingPass(GraphicComponent::RP_2DLayout);
			AddComponent(pGraphic, true);
		}

		Material* pMaterial = snew Material();
		pMaterial->SetFlag(Material::MF_AlphaBlending, true);
		pMaterial->SetShader(Material::m_strDefaultShader);
		pGraphic->SetMaterial(pMaterial);
		
		UpdateGeometry(m_strText, m_Color);

		super::Init();
	}	

	//! registers the entity and its children for rendering
	
	void Text::RegisterForRendering()
	{
		if(m_strText.empty())
			return;
		
		PreRender();
		EntityRenderer::Instance()->RegisterEntity(this);
	}	

	//! Get the local axis-aligned bounding box
	AABBox2D Text::GetBoundingBox() const
	{
		AABBox2D box(Vector2::Zero, Vector2::Create(float(m_Area.Width), float(m_Area.Height)));
		return box;
	}

	//! Reloads the image given an image path
	void Text::SetFont(std::string strPath)
	{
		m_Font = strPath;
	}

	//! sets the text
	void Text::SetText(const std::wstring& strText)
	{ 
		if(m_strText != strText)
		{
			m_strText = strText;
			m_bDirty = true;
		}		
	}

	//! sets the color
	void Text::SetColor(const Color& color)
	{
		m_Color = color;
		m_bDirty = true;
	}

	//! pre-rendering
	void Text::PreRender()
	{
		if(m_bDirty)
		{
			UpdateGeometry(m_strText, m_Color);
			m_bDirty = false;
		}

		VertexBuffer* pVB = GetComponent<GraphicComponent>()->GetVertexBuffer();
		pVB->SetNumVertices(m_DesiredVertexCount >= 0 ? m_DesiredVertexCount : pVB->GetMaxVertices());
	}

	//! updates the geometry
	void Text::UpdateGeometry(const std::wstring& text, const Color& color)
	{
		auto gc = GetComponent<GraphicComponent>();
		auto material = gc->GetMaterial();
		material->SetColor(color);

		MemoryTexture* previousTexture = NULL;
		if (m_FontType == FT_Dynamic)
			previousTexture = DYNAMIC_CAST(material->GetTexture(), MemoryTexture);

		auto _text = text;
		std::transform(_text.begin(), _text.end(), _text.begin(), ::toupper);
		auto fontInfo = FontManager::Instance()->GetFont(m_FontType, m_FontSize, _text, previousTexture);

		if (m_FontType == FT_Dynamic)
			m_FontInfo = fontInfo;

		if (auto vb = gc->GetVertexBuffer())
		{
			GeometryProvider::Instance()->UpdateTextVertexBuffer(vb, m_FontSize, _text, m_Area, m_eAlignement, m_LineSpacing, fontInfo->m_CharData);
		}
		else
		{
			if(!text.empty())
			{
				auto vb = GraphicsDriver::Instance()->CreateVertexBuffer();
				std::string cstr(text.begin(), text.end());
				vb->SetName(cstr);
				vb->SetDynamic(true);
				GeometryProvider::Instance()->UpdateTextVertexBuffer(vb, m_FontSize, _text, m_Area, m_eAlignement, m_LineSpacing, fontInfo->m_CharData);
				gc->SetVertexBuffer(vb);
			}
		}

		if (m_FontType != FT_Dynamic || previousTexture == NULL)
			material->SetTexture(0, fontInfo->m_Texture);
	}
}

