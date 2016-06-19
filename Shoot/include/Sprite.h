/* 

Amine Rehioui
Created: April 17th 2010

*/

#pragma once


#include "Image.h"

#include "Texture.h"

namespace shoot
{
	//! Sprite entity
	/** Contains common 2D animation and transformation features needed by gameplay */
	class Sprite : public Image				 
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(Sprite, Image);

		//! Constructor
		Sprite();

		//! Destructor
		virtual ~Sprite();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		virtual void Init();

		//! called during the update of the entity		
		virtual void Update();		

		//! Reloads the sprite given a sprite resource path
		void SetPath(std::string strPath);

		//! Returns the number of animations
		size_t GetNumAnimations() const { return m_aAnimations.GetSize(); }

		//! returns an animation name
		/** \param animIndex: specifies the animation index. Default is -1 to get the current animation name. */
		std::string GetAnimationName(int animIndex = -1);

		//! Returns the current animation index
		size_t GetCurrentAnimationIndex() const { return m_CurrentAnimation; }

		//! sets the current animation from an animation name
		/** \param strName: name of the animation
			\param frameIndex: specifies the starting frame */
		void SetCurrentAnimation(const char* strName, size_t frameIndex = 0);

		//! Sets the current animation from an animation index
		/** \param animIndex: index of the animation
			\param frameIndex: specifies the starting frame */
		void SetCurrentAnimation(size_t animIndex, size_t frameIndex = 0);

		//! Returns the number of frames
		inline size_t GetNumFrames() const { return m_aAnimations[m_CurrentAnimation].aFrames.size(); }

		//! returns the current frame
		inline size_t GetCurrentFrame() const { return m_CurrentFrame; }

		//! Sets the current frame
		void SetCurrentFrame(size_t index);

		//! Start playback at the specified frame
		/** \param bLoop: specifies if played in a loop
			\param frameIndex: specifies the starting frame. Default is -1 to continue at the current frame. */
		void Play(bool bLoop = false, int frameIndex = -1);

		//! stops the playback
		void Stop();

		//! returns the playback status
		bool IsStopped() const { return m_bIsStopped; }

		//! Get the bounding box
		AABBox2D GetBoundingBox() const;

		//! Structure describing an animation
		struct Animation : public ISerializableStruct
		{
			//! constructor
			Animation() 
			: framesPerSecond(30) // it's better to expose the FPS in the xml
			, fFrameDeltaTime(0.03f)
			{
			}

			std::vector< Reference<Texture> > aFrames;
			float fFrameDeltaTime;

			// properties
			std::string name;
			int framesPerSecond;
			Array<std::string> aFramePaths;			

			//! Initializes the Animation frames
			void Init();

			//! serialize the animation
			void Serialize(PropertyStream& stream)
			{
				stream.Serialize("Name", &name);
				stream.Serialize("FPS", &framesPerSecond);
				stream.Serialize("Frames", &aFramePaths);				
			}
		};

		private:

		//! Deletes the animations
		void DeleteAnimations();

		Array<Animation> m_aAnimations;
		
		size_t m_CurrentFrame;
		float m_fCurrentFrameDuration;

		bool m_bIsStopped;
		bool m_bIsLooping;

		// properties
		std::string m_strPath;
		size_t m_CurrentAnimation;

		// used to expose the animations as an enumeration in the editor
		static const int AnimationLiteralSize = 64;
		char **m_ppAnimationLiterals; 
	};
}

