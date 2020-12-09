/* 

Amine Rehioui
Created: September 6th 2014

*/

#include "Game.h"

#include "StencilEffects.h"
#include "TextureAnimationVisitor.h"
#include "Playfield.h"
#include "MaterialProvider.h"
#include "GameContext.h"

namespace shoot
{
	extern Vector3 g_vHexOffsets[6];

	DEFINE_OBJECT(StencilEffects);

	//! Constructor
	StencilEffects::StencilEffects()
		: m_bPlayfieldEffectVisible(false)
		, m_fPlayfieldEffectTimer(0.0f)
		, m_TileVBDirty(false)
		// properties
		, m_PlayfieldHighlight(Color::Create(1.0f, 1.0f, 1.0f))
		, m_BlockedTileColor(Color::Red)
		, m_fPlayfieldHighlightAlpha(0.5f)
		, m_fHexagonScaleFactor(0.94f)
		, m_fPlayfieldEffectMaxDuration(2.0f)
		, m_fPlayfieldEffectFadeDuration(0.3f)
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void StencilEffects::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("BillBoardEntity", &m_BillBoardEntity);
		stream.Serialize("PlayfieldHighlight", &m_PlayfieldHighlight);
		stream.Serialize("PlayfieldHighlight.Alpha", &m_fPlayfieldHighlightAlpha);
		stream.Serialize("BlockedTileColor", &m_BlockedTileColor);
		stream.Serialize("PlayfieldEffectMaxDuration", &m_fPlayfieldEffectMaxDuration);
		stream.Serialize("PlayfieldEffectFadeDuration", &m_fPlayfieldEffectFadeDuration);
		stream.Serialize("HexagonScaleFactor", &m_fHexagonScaleFactor);
	}

	//! Initiliazes the effect
	void StencilEffects::Init()
	{
		if(m_BillBoardEntity.IsValid())
			m_BillBoardEntity->Init();
		
		Material::CreationInfo materialInfo;
		materialInfo.AddTexture("common/tex/WhiteBG.xml");
		materialInfo.SetFlag(Material::MF_AlphaBlending, true);		
		m_DefaultMaterial = MaterialProvider::Instance()->GetMaterial(materialInfo);

		materialInfo.SetShader("common/shaders/tex_col.xml");
		m_TilesMaterial = MaterialProvider::Instance()->GetMaterial(materialInfo);

		auto gameContext = GameEngine::Instance()->GetGameContext();
		Playfield* pPlayfield = gameContext ? gameContext->m_Playfield.Get() : NULL;
		if(!pPlayfield)
			return;

		m_PlayfieldVB = GraphicsDriver::Instance()->CreateVertexBuffer();
		const auto& tiles = pPlayfield->GetBuildableTiles();
		int numTiles = (int)tiles.size();
		int maxVertices = numTiles*6;
		int maxIndices = numTiles*12;
		Vertex3D* pVertices = snew Vertex3D[maxVertices];
		ushort* pIndices = snew ushort[maxIndices];
		int currentVertex = 0;
		int currentIndex = 0;
		const Size& dimension = pPlayfield->GetDimension();
		for(auto &tile : tiles)
		{
			Vector3 vPos = tile->m_vPosition + Vector3::Create(0.0f, 0.0f, 0.1f);
			SHOOT_ASSERT(currentVertex+5 < maxVertices, "maxVertices reached");
			SHOOT_ASSERT(currentIndex+11 < maxIndices, "maxIndices reached");

			for (int k = 0; k < 6; ++k)
			{
				pVertices[currentVertex + k].Pos = vPos + (g_vHexOffsets[k] * m_fHexagonScaleFactor);
				pVertices[currentVertex + k].UV = Vector2::Zero;
			}

			pIndices[currentIndex+0] = currentVertex+1;
			pIndices[currentIndex+1] = currentVertex+0;
			pIndices[currentIndex+2] = currentVertex+2;

			pIndices[currentIndex+3] = currentVertex+3;
			pIndices[currentIndex+4] = currentVertex+1;
			pIndices[currentIndex+5] = currentVertex+2;

			pIndices[currentIndex+6] = currentVertex+3;
			pIndices[currentIndex+7] = currentVertex+2;
			pIndices[currentIndex+8] = currentVertex+4;

			pIndices[currentIndex+9] = currentVertex+5;
			pIndices[currentIndex+10] = currentVertex+3;
			pIndices[currentIndex+11] = currentVertex+4;

			currentIndex += 12;
			currentVertex += 6;
		}
		m_PlayfieldVB->SetVertices(pVertices, maxVertices, Vertex3D::VF_Pos|Vertex3D::VF_UV);
		m_PlayfieldVB->SetIndices(pIndices, maxIndices);
		m_PlayfieldVB->SetDynamic(true);
	}	

	//! Renders the effect
	void StencilEffects::Render()
	{
		GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_StencilTesting, true);
		GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_StencilWriting, 0xff);
		GraphicsDriver::Instance()->ClearBuffers(GraphicsDriver::BF_StencilBuffer);

		RenderPlayfieldEffect();
		RenderConstructionEffect();

		GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_StencilTesting, false);		
	}

	//! Update
	void StencilEffects::Update()
	{		
		if (m_TileVBDirty)
		{
			if (!m_TilesVB.IsValid())
				m_TilesVB = GraphicsDriver::Instance()->CreateVertexBuffer();

			// remove empty tiles
			for (auto it = m_lActorTiles.begin(); it != m_lActorTiles.end();)
			{
				if (!(*it)->m_Actor.IsValid())
					it = m_lActorTiles.erase(it);
				else
					++it;
			}

			const auto& tiles1 = m_lActorTiles;
			const auto& tiles2 = PLAYFIELD->GetBlockingTiles();
			auto numTiles = (int)tiles1.size() + (int)tiles2.size();
			auto maxVertices = numTiles * 12;
			auto pVertices = snew Vertex3D[maxVertices];
			int currentVertex = 0;
			UpdateTileVB(tiles1, pVertices, currentVertex);
			UpdateTileVB(tiles2, pVertices, currentVertex);
			m_TilesVB->SetVertices(pVertices, maxVertices, Vertex3D::VF_Pos|Vertex3D::VF_UV|Vertex3D::VF_Color);
			m_TilesVB->SetDynamic(true);
			m_TileVBDirty = false;
		}

		if(m_bPlayfieldEffectVisible)
		{
			if(m_fPlayfieldEffectTimer > m_fPlayfieldEffectDuration)
			{
				m_bPlayfieldEffectVisible = false;
				m_fPlayfieldEffectTimer = 0.0f;
			}
			else
			{
				auto playfieldAlpha = m_fPlayfieldHighlightAlpha;
				if(m_fPlayfieldEffectTimer < m_fPlayfieldEffectFadeDuration)
					playfieldAlpha *= (m_fPlayfieldEffectTimer / m_fPlayfieldEffectFadeDuration);
				else if(m_fPlayfieldEffectTimer > m_fPlayfieldEffectDuration - m_fPlayfieldEffectFadeDuration)
					playfieldAlpha *= ((m_fPlayfieldEffectDuration-m_fPlayfieldEffectTimer) / m_fPlayfieldEffectFadeDuration);

				m_PlayfieldHighlight.A = playfieldAlpha;
				m_fPlayfieldEffectTimer += g_fDeltaTime;
			}
		}
	}

	//! AddConstructionEffect
	void StencilEffects::AddConstructionEffect(MeshEntity* pStencilMesh) 
	{
		m_aStencilMeshes.push_back(Handle<MeshEntity>(pStencilMesh));
	}

	void StencilEffects::RenderPlayfieldEffect()
	{
		if (!GAMECONTEXT)
			return;

		auto playfield = PLAYFIELD.Get();
		if (!playfield)
			return;

		if (!m_bPlayfieldEffectVisible)
			return;

		GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, playfield->GetWorldTransform());
		if (m_TilesVB.IsValid())
		{
			// Fill stencil with 1s
			GraphicsDriver::Instance()->SetStencilFunc(GraphicsDriver::CF_Always, 1, 0xff);
			GraphicsDriver::Instance()->SetStencilOps(GraphicsDriver::SO_Keep, GraphicsDriver::SO_Keep, GraphicsDriver::SO_Replace);

			m_TilesMaterial->Begin();
			m_TilesMaterial->GetShader()->GetConstant("uAmbientColor")->Set(&Color::White, 1);
			m_TilesMaterial->GetShader()->Begin();
			m_TilesVB->Begin(m_TilesMaterial->GetShader());
			m_TilesVB->Draw();
		}

		GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_StencilWriting, 0); // no more writing into the stencil buffer		
		GraphicsDriver::Instance()->SetStencilFunc(GraphicsDriver::CF_NotEqual, 1, 0xff); // Render only where stencil is not 1

		m_DefaultMaterial->Begin();
		m_DefaultMaterial->GetShader()->GetConstant("uAmbientColor")->Set(&m_PlayfieldHighlight, 1);
		m_DefaultMaterial->GetShader()->Begin();
		m_PlayfieldVB->Begin(m_DefaultMaterial->GetShader());
		m_PlayfieldVB->Draw();
	}

	void StencilEffects::RenderConstructionEffect()
	{
		if(m_aStencilMeshes.empty())
			return;

		GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_StencilWriting, 0xff);

		// Fill stencil with 1s
		GraphicsDriver::Instance()->SetStencilFunc(GraphicsDriver::CF_Never, 1 << 1, 0xff);
		GraphicsDriver::Instance()->SetStencilOps(GraphicsDriver::SO_Replace, GraphicsDriver::SO_Keep, GraphicsDriver::SO_Keep);

		GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_ColorWriting, false);

		// Render stencil meshes
		m_DefaultMaterial->Begin();
		m_DefaultMaterial->GetShader()->Begin();
		std::vector<Matrix44> worldTransforms, textureTransforms;
		worldTransforms.resize(1);
		textureTransforms.resize(1);
		for(std::vector< Handle<MeshEntity> >::iterator it = m_aStencilMeshes.begin(); it != m_aStencilMeshes.end(); )
		{
			if(!(*it).IsValid())
			{
				it = m_aStencilMeshes.erase(it);
				continue;
			}
			
			for(auto submesh : (*it)->GetSubMeshes())
			{
				GraphicComponent* gc = submesh->GetComponent<GraphicComponent>();
				gc->GetVertexBuffer()->Begin(m_DefaultMaterial->GetShader());
				worldTransforms[0] = submesh->GetWorldTransform();
				gc->GetVertexBuffer()->Draw(worldTransforms, std::vector<Matrix44>());
			}
			++it;
		}

		GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_DepthTesting, false);
		GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_ColorWriting, true);
		GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_StencilWriting, 0); // no more writing into the stencil buffer		
		GraphicsDriver::Instance()->SetStencilFunc(GraphicsDriver::CF_Equal, 1 << 1, 0xff); // Render only where stencil is 1

		// render billboards
		//m_BillBoardEntity->GetComponent<TextureAnimationVisitor>()->Update();
		GraphicComponent* pGC = m_BillBoardEntity->GetComponent<GraphicComponent>();
		VertexBuffer* pVB = pGC->GetVertexBuffer();
		Material* pMat = pGC->GetMaterial();
		Shader* pShader = pMat->GetShader();
		pMat->Begin();
		pShader->GetConstant("uAmbientColor")->Set(&pMat->GetColor(), 1);
		pShader->Begin();
		pVB->Begin(pShader);
		const Matrix44& billboardMatrix = EntityRenderer::Instance()->Get3DCamera()->GetViewMatrixInverse();
		textureTransforms[0] = pGC->GetTextureTransform();
		for(auto &stencilMesh : m_aStencilMeshes)
		{
			worldTransforms[0] = billboardMatrix;
			worldTransforms[0].SetTranslation(stencilMesh->GetAbsolutePosition());
			pVB->Draw(worldTransforms, textureTransforms);			
		}

		GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_DepthTesting, true);
	}

	void StencilEffects::UpdateTileVB(const std::list<Tile*>& tiles, Vertex3D*& pVertices, int& currentVertex)
	{
		for (auto tile : tiles)
		{
			auto pos = tile->m_vPosition + Vector3::Create(0.0f, 0.0f, 0.1f);
			for (int i = 0; i < 12; ++i)
			{
				pVertices[currentVertex + i].UV = Vector2::Zero;
				pVertices[currentVertex + i].color = Color::Create(m_BlockedTileColor.RGB, m_fPlayfieldHighlightAlpha);
			}

			pVertices[currentVertex + 0].Pos = pos + g_vHexOffsets[1];
			pVertices[currentVertex + 1].Pos = pos + g_vHexOffsets[0];
			pVertices[currentVertex + 2].Pos = pos + g_vHexOffsets[2];

			pVertices[currentVertex + 3].Pos = pos + g_vHexOffsets[3];
			pVertices[currentVertex + 4].Pos = pos + g_vHexOffsets[1];
			pVertices[currentVertex + 5].Pos = pos + g_vHexOffsets[2];

			pVertices[currentVertex + 6].Pos = pos + g_vHexOffsets[3];
			pVertices[currentVertex + 7].Pos = pos + g_vHexOffsets[2];
			pVertices[currentVertex + 8].Pos = pos + g_vHexOffsets[4];

			pVertices[currentVertex + 9].Pos = pos + g_vHexOffsets[5];
			pVertices[currentVertex + 10].Pos = pos + g_vHexOffsets[3];
			pVertices[currentVertex + 11].Pos = pos + g_vHexOffsets[4];
							
			currentVertex += 12;
		}
	}

	//! SetPlayfieldEffectVisible
	void StencilEffects::ShowPlayfieldEffect()
	{ 
		m_bPlayfieldEffectVisible = true;
		m_fPlayfieldEffectDuration = m_fPlayfieldEffectMaxDuration;
			
		if(m_fPlayfieldEffectTimer > m_fPlayfieldEffectDuration - m_fPlayfieldEffectFadeDuration)
			m_fPlayfieldEffectTimer = m_fPlayfieldEffectTimer - (m_fPlayfieldEffectDuration - m_fPlayfieldEffectFadeDuration);
		else if(m_fPlayfieldEffectTimer > m_fPlayfieldEffectFadeDuration)
			m_fPlayfieldEffectTimer = m_fPlayfieldEffectFadeDuration;
	}

	//! OnActorSpawned
	void StencilEffects::OnActorSpawned(Tile* tile)
	{
		m_lActorTiles.push_back(tile);
		m_TileVBDirty = true;
	}
}

