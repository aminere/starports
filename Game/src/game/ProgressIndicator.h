/* 

Amine Rehioui
Created: April. 18th 2015

*/

#pragma once


namespace shoot
{
	//! ProgressIndicator
	class ProgressIndicator : public Entity3D
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(ProgressIndicator, Entity3D);

		//! constructor
		ProgressIndicator();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream) override;

		//! called during the initialization of the entity
		virtual void Init() override;

		//! called during the update of the entity
		virtual void Update() override;

		//! SetDuration
		inline void SetDuration(float duration) { m_Duration = duration; }

		//! SetRemoveOnCompletion
		inline void SetRemoveOnCompletion(bool remove) { m_RemoveOnCompletion = remove; }

		//! UpdateDuration
		void UpdateDuration(float);

		//! StartProgress
		void StartProgress(float duration);

	protected:

		//! UpdateProgress
		void UpdateProgress(float factor, bool finished = false);
				
		float m_Time;
		bool m_Finished;
		Handle<VertexBuffer> m_VB;
		bool m_RemoveOnCompletion;
		Handle<Entity3D> m_TipIndicator;

		// properties
		int m_NumSegments;
		float m_Radius;
		float m_Thickness;
		float m_Duration;
	};
}



