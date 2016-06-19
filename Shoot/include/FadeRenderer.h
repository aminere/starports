/* 

Amine Rehioui
Created: June 15th 2013

*/

#pragma once


namespace shoot
{
	//! FadeRenderer
	class FadeRenderer : public Object
	{
		DECLARE_OBJECT(FadeRenderer, Object);

	public:

		//! constructor
		FadeRenderer();

		//! destructor
		virtual ~FadeRenderer();

		//! update the fade effect
		void Update();

		//! renders the fade effect
		void Render();

		//! returns the fade color
		inline const Color& GetColor() const { return m_Color; }

		//! sets the fade color
		void SetColor(const Color& color);

		//! starts a fade effect
		void StartFade(const Color& srcColor, const Color& destColor, float fDuration, bool additive, bool stayFadedUntilFurtherNotice = false);

		inline const Color& GetDestColor() const { return m_FadeDestColor; }

		//! returns the instance
		inline static FadeRenderer* Instance()
		{
			return ms_pInstance;
		}

	private:

		Reference<Texture> m_WhiteBG;
		Color m_Color;

		bool m_bFadeInProgress;
		float m_fFadeDuration;
		float m_fFadeTimer;
		Color m_FadeSrcColor;
		Color m_FadeDestColor;
		bool m_Additive;
		bool m_StayFadedUntilFurtherNotice;

		static FadeRenderer* ms_pInstance;
	};
}



