/* 

Amine Rehioui
Created: October 17th 2011

*/

#include "Shoot.h"

#include "StatsManager.h"

#include "MemoryManager.h"

#include "Text.h"

#include <stdio.h> // sprintf

#include "InputManager.h"

#include "FileSystem.h"

namespace shoot
{
	//! constructor
	StatsManager::StatsManager()
		: m_bEnabled(!true)
	{
		const char* strFont = "common/default.ttf";
		float lineSpacing = 24.0f;
		m_FPS = snew Text();
		m_FPS->SetFont(strFont);
		m_FPS->SetPosition(Vector2::Create(0.0f, lineSpacing*0));
		m_FPS->Init();
		m_MemoryUsed = snew Text();
		m_MemoryUsed->SetFont(strFont);
		m_MemoryUsed->SetPosition(Vector2::Create(0.0f, lineSpacing*1));
		m_MemoryUsed->Init();
		m_NumDrawCalls = snew Text();
		m_NumDrawCalls->SetFont(strFont);
		m_NumDrawCalls->SetPosition(Vector2::Create(0.0f, lineSpacing*2));
		m_NumDrawCalls->Init();
		m_FileSystemInfo = snew Text();
		m_FileSystemInfo->SetArea(Size(480, int(lineSpacing)*2));
		m_FileSystemInfo->SetFont(strFont);
		m_FileSystemInfo->SetPosition(Vector2::Create(0.0f, lineSpacing*3));
		m_FileSystemInfo->Init();
		m_QuadTreeInfo = snew Text();
		m_QuadTreeInfo->SetFont(strFont);
		m_QuadTreeInfo->SetPosition(Vector2::Create(0.0f, lineSpacing*5));
		m_QuadTreeInfo->Init();
	}

	//! destructor
	StatsManager::~StatsManager()
	{
	}

	//! update
	void StatsManager::Update()
	{
		if(InputManager::Instance()->IsKeyJustReleased(InputManager::KT_L))		
			m_bEnabled = !m_bEnabled;

		m_FPS->SetText(Utils::StringFormat("FPS: %.2f", Engine::Instance()->GetFPS()));		
		m_MemoryUsed->SetText(Utils::StringFormat("RAM: %.2f KB, Peak %.2f KB", float(MemoryManager::GetUsedMemory()) / 1024.0f, float(MemoryManager::GetPeakMemory()) / 1024.0f));

		if(m_bEnabled)
		{
			Engine* pEngine = Engine::Instance();
			QuadTree* pQuadTree = pEngine->GetNumContexts() ? pEngine->GetContext(0)->GetQuadTree() : NULL;
			if(pQuadTree)
			{				
				m_QuadTreeInfo->SetText(Utils::StringFormat("Nodes drawn %d/%d", pQuadTree->GetNodesDrawn(), pQuadTree->GetNodeCount()));
			}
			else
			{
				m_QuadTreeInfo->SetText(L"");
			}

			EntityRenderer::Stats& renderingStats = EntityRenderer::Instance()->GetStats();
			m_NumDrawCalls->SetText(Utils::StringFormat("Mat %d, Vtx %d, Tri %d, Draw %d", renderingStats.NumMaterials, renderingStats.NumVertices, renderingStats.NumTriangles, renderingStats.NumDrawCalls));

			std::vector<FileSystem::PartitionInfo>& partitions = FileSystem::Instance()->GetPartitions();
			auto parts = Utils::StringFormat("(%d) ", partitions.size());
			for (size_t i=0; i<partitions.size(); ++i)
			{
				parts += Utils::StringFormat("%d ", partitions[i].pData ? partitions[i].dataSize / 1024 : 0);
			}
			m_FileSystemInfo->SetText(parts);
		}
	}

	//! registers for rendering
	void StatsManager::RegisterForRendering()
	{
#define REGISTER_TEXT(x) if(x->GetText().length()) \
		{ \
			x->PreRender(); \
			EntityRenderer::Instance()->RegisterDebugEntity(x); \
		}
		
		if(!m_bEnabled)
			return;

		REGISTER_TEXT(m_FPS);
		REGISTER_TEXT(m_MemoryUsed);
		REGISTER_TEXT(m_NumDrawCalls);
		REGISTER_TEXT(m_FileSystemInfo);
		REGISTER_TEXT(m_QuadTreeInfo);
	}	
}
