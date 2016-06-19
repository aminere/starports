/* 

Amine Rehioui
Created: June 15th 2013

*/

#pragma once


namespace shoot
{
	//! GameData
	class GameData : public Entity
	{
		DECLARE_OBJECT(GameData, Entity);

	public:

		//! E_ProductType
		enum E_ProductType
		{
			PT_FPSMode,
			PT_Unknown
		};

		//! Product
		struct Product
		{
			E_ProductType m_Type;
			std::wstring m_ID;
			std::wstring m_Name;
			std::wstring m_Price;
		};

		//! save file name
		static const char* m_sSaveFile;

		//! constructor
		GameData();

		//! destructor
		~GameData();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! returns the last unlocked level
		inline int GetLastUnlockedLevel() const { return m_LastUnlockedLevel; }

		//! sets the last unlocked level
		inline void SetLastUnlockedLevel(int level) { m_LastUnlockedLevel = level; }

		//! GetCurrentLevel
		inline int GetCurrentLevel() const { return m_CurrentLevel; }

		//! SetCurrentLevel
		inline void SetCurrentLevel(int level) { m_CurrentLevel = level; }

		//! returns the music volume
		inline float GetMusicVolume() const { return m_fMusicVolume; }

		//! returns the sound volume
		inline float GetSoundVolume() const { return m_fSoundVolume; }

		//! sets the music volume
		inline void SetMusicVolume(float fVolume) { m_fMusicVolume = fVolume; }

		//! sets the sound volume
		inline void SetSoundVolume(float fVolume) { m_fSoundVolume = fVolume; }

		//! gets said thank you status
		inline bool GetSaidThankYou() const { return m_bSaidThankYou; }

		//! sets said thank you status
		inline void SetSaidThankYou(bool bSaid) { m_bSaidThankYou = bSaid; }

		//! Saves the game data
		void Save();

		//! Saves the object to XML
		void SaveToXML(const char* strPath) override;

		//! AddProductInfo
		void AddProductInfo(const std::wstring& id, const std::wstring& name, const std::wstring& price);

		//! OnProductPurchased
		bool OnProductPurchased(const std::wstring& id);

		//! HasProduct
		bool HasProduct(E_ProductType type);

		//! GetProductInfo
		bool GetProductInfo(E_ProductType type, Product& product);

	private:

		//! GetProductType
		E_ProductType GetProductType(const std::wstring& id);

		std::vector<Product> m_ProductInfos;

		// properties
		int m_CurrentLevel;
		int m_LastUnlockedLevel;
		float m_fMusicVolume;
		float m_fSoundVolume;
		bool m_bSaidThankYou;
		Array<int> m_Products;
	};
}



