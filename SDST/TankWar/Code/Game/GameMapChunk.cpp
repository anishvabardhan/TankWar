#include "Game/GameMapChunk.hpp"
//-----------------------------------------------------------------------------------------------
// Game Includes
#include "Game/GameMap.hpp"

//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Mesh/MeshBuilder.hpp"
#include "Engine/Renderer/Mesh/Mesh.hpp"
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Constructor
//
GameMapChunk::GameMapChunk(GameMap* gameMap, const IntVector2& coords)
{
	m_map = gameMap;
	m_chunkCoords = coords;

	m_renderable = new Renderable();
	m_renderable->SetMaterial(*Renderer::GetInstance()->CreateOrGetMaterial("Data/Materials/terrain.mat"));
}

//-----------------------------------------------------------------------------------------------
// Destructor
//
GameMapChunk::~GameMapChunk()
{
	Cleanup();
}

//-----------------------------------------------------------------------------------------------
// Cleans up the destruction
//
void GameMapChunk::Cleanup()
{
	delete m_renderable;
	m_renderable = nullptr;

	m_map = nullptr;
}

//-----------------------------------------------------------------------------------------------
// Generates the chunk of terrain 
//
Vector3 GameMapChunk::GenerateTerrain(float u, float v)
{
	// Used to compute the surface patch
	Vector3 vertex;
	vertex.x = u;
	vertex.z = v;

	vertex.y = m_map->GetLinearHeight(Vector2(u,v));

	return vertex;
}
