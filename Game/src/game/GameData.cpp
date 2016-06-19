/* 

Amine Rehioui
Created: June 15th 2013

*/

#include "Game.h"

#include "GameData.h"

#include "File.h"

namespace shoot
{
	DEFINE_OBJECT(GameData);

	//! statics
	const char* GameData::m_sSaveFile = "GameData.xml";

	//! constructor
	GameData::GameData()
		// properties
		: m_CurrentLevel(0)
		, m_LastUnlockedLevel(0)
		, m_fMusicVolume(1.0f)
		, m_fSoundVolume(1.0f)
		, m_bSaidThankYou(false)
	{
	}

	//! destructor
	GameData::~GameData()
	{
	}
	
	//! serializes the entity to/from a PropertyStream
	void GameData::Serialize(PropertyStream& stream)
	{		
		stream.Serialize("LastUnlockedLevel", &m_LastUnlockedLevel);		
		stream.Serialize("MusicVolume", &m_fMusicVolume);
		stream.Serialize("SoundVolume", &m_fSoundVolume);
		stream.Serialize("SaidThankYou", &m_bSaidThankYou);
		stream.Serialize("CurrentLevel", &m_CurrentLevel);		
		stream.Serialize("Products", &m_Products);
	}

	//! Saves the game data
	void GameData::Save()
	{
		File::UseWritablePath uwp;
		super::SaveToXML(m_sSaveFile);
	}

	//! Saves the object to XML
	void GameData::SaveToXML(const char* strPath)
	{
		File::UseWritablePath uwp;
		super::SaveToXML(strPath);
	}

	//! AddProduct
	void GameData::AddProductInfo(const std::wstring& id, const std::wstring& name, const std::wstring& price)
	{
		auto it = std::find_if(m_ProductInfos.begin(), m_ProductInfos.end(), [&](const Product& p) { return p.m_ID == id; });
		if (it == m_ProductInfos.end())
		{
			Product p;
			p.m_ID = id;
			p.m_Name = name;
			p.m_Price = price;
			p.m_Type = GetProductType(id);
			m_ProductInfos.push_back(p);			
		}
	}

	//! OnProductPurchased
	bool GameData::OnProductPurchased(const std::wstring& id)
	{
		Log.Print("OnProductPurchased(%s)\n", id.c_str());
		auto type = GetProductType(id);;
		if (type == PT_Unknown)
			return false;

		if (!HasProduct(type))
		{
			m_Products.Add(snew int(type));
			Save();
			return true;
		}

		return false;
	}

	//! HasProduct
	bool GameData::HasProduct(E_ProductType type)
	{
		for (auto &product : m_Products)
			if (*product == (int)type)
				return true;

		return false;
	}

	//! GetProductInfo
	bool GameData::GetProductInfo(E_ProductType type, Product& product)
	{
		auto it = std::find_if(m_ProductInfos.begin(), m_ProductInfos.end(), [&](const Product& p) { return p.m_Type == type; });
		if (it != m_ProductInfos.end())
		{
			product = (*it);
			return true;
		}
		return false;
	}

	//! GetProductType
	GameData::E_ProductType GameData::GetProductType(const std::wstring& id)
	{
		if (id == L"com.frantic.starports.extramode1")
			return PT_FPSMode;

		SHOOT_ASSERT(false, "Unknown product type '%s'", id.c_str());
		return PT_Unknown;
	}
}

