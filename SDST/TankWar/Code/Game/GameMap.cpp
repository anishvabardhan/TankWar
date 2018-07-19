#include "Game/GameMap.hpp"
//-----------------------------------------------------------------------------------------------
// Game Includes
#include "Game/GameMapChunk.hpp"
#include "Game/GameCommon.hpp"
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Mesh/MeshBuilder.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/DebugRenderUtils.hpp"
#include "Engine/Math/Ray3.hpp"

//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Constructor
//
GameMap::GameMap()
{
}

//-----------------------------------------------------------------------------------------------
// Destructor
//
GameMap::~GameMap()
{
	FreeAllChunks();
}

//-----------------------------------------------------------------------------------------------
// Returns the height at the given point on the XZ plane
//
float GameMap::GetHeightForDiscrete(const IntVector2& pos) const
{
	Vector2 hDimensions = m_heightMap->GetDimensions();
	if(pos.x < 0 ||pos.y < 0 || pos.x > hDimensions.x || pos.y > hDimensions.y)
	{
		return GetHeightForDiscrete(IntVector2::ZERO);
	}
	int index = (int) ((pos.y * (float) m_heightMap->GetDimensions().x) + pos.x);
	return m_heights[index];
}

//-----------------------------------------------------------------------------------------------
// Returns the normal at a discrete coordinate
//
Vector3 GameMap::GetNormalForDiscrete(const IntVector2& coord) const
{
	Vector3 du = GetPositionForDiscrete(coord + STEP_EAST) - GetPositionForDiscrete(coord + STEP_WEST);
	Vector3 tangent = du.GetNormalized();

	Vector3 dv = GetPositionForDiscrete(coord + STEP_NORTH) - GetPositionForDiscrete(coord + STEP_SOUTH);
	Vector3 bitan = dv.GetNormalized();

	Vector3 normal = CrossProduct(bitan, tangent);
	return normal;
}

//-----------------------------------------------------------------------------------------------
// Returns a position for the discrete coordinate
//
Vector3 GameMap::GetPositionForDiscrete(const IntVector2& coord) const
{
	Vector2 hDimensions = m_heightMap->GetDimensions();
	if(coord.x < 0 ||coord.y < 0 || coord.x > hDimensions.x || coord.y > hDimensions.y)
	{
		return Vector3(0.f, GetHeightForDiscrete(IntVector2::ZERO), 0.f);
	}
	Vector2 originXZ = m_extents.mins;
	Vector2 coordXZ = originXZ + Vector2(coord);

	float height = GetHeightForDiscrete(coord);

	return Vector3(coordXZ.x, height, coordXZ.y);
}

//-----------------------------------------------------------------------------------------------
// Returns the height at a continuous point on the XZ plane
//
float GameMap::GetLinearHeight(const Vector2& pos) const
{
	Vector2 position;
	Vector2 hDimensions = m_heightMap->GetDimensions();
	IntVector2 bl;
	position.x = ClampFloat(pos.x, m_extents.mins.x, m_extents.maxs.x);
	position.y = ClampFloat(pos.y, m_extents.mins.y, m_extents.maxs.y);
	float test_x = RangeMapFloat(position.x, m_extents.mins.x, m_extents.maxs.x, 0.f, hDimensions.x - 2.f);
	float test_y = RangeMapFloat(position.y, m_extents.mins.y, m_extents.maxs.y, 0.f, hDimensions.y - 2.f);

	bl = IntVector2((int) floor(test_x), (int) floor(test_y));
	IntVector2 br = bl + IntVector2(1, 0);
	IntVector2 tr = bl + IntVector2(1, 1);
	IntVector2 tl = bl + IntVector2(0, 1);

	float blHeight = GetHeightForDiscrete(bl);
	float brHeight = GetHeightForDiscrete(br);
	float tlHeight = GetHeightForDiscrete(tl);
	float trHeight = GetHeightForDiscrete(tr);
	
	float hbot = Interpolate(blHeight, brHeight, fmodf(test_x,1));
	float htop = Interpolate(tlHeight, trHeight, fmodf(test_x,1));
	float hfinal = Interpolate( hbot, htop, fmodf(test_y,1));

	return hfinal;

}

//-----------------------------------------------------------------------------------------------
// Gets a bilinearly sampled normal
//
Vector3 GameMap::GetNormalAtPosition(const Vector3& pos) const
{
	Vector2 position; 
	Vector2 hDimensions = m_heightMap->GetDimensions();
	IntVector2 bl;
	position.x = ClampFloat(pos.x, m_extents.mins.x - 1.f, m_extents.maxs.x - 1.f);
	position.y = ClampFloat(pos.z, m_extents.mins.y - 1.f, m_extents.maxs.y - 1.f);
	float test_x = RangeMapFloat(position.x, m_extents.mins.x, m_extents.maxs.x, 0.f, hDimensions.x - 2.f);
	float test_y = RangeMapFloat(position.y, m_extents.mins.y, m_extents.maxs.y, 0.f, hDimensions.y - 2.f);

	bl = IntVector2((int) floor(test_x), (int) floor(test_y));
	IntVector2 br = bl + IntVector2(1, 0);
	IntVector2 tr = bl + IntVector2(1, 1);
	IntVector2 tl = bl + IntVector2(0, 1);

	Vector3 blNormal = GetNormalForDiscrete(bl);
	Vector3 brNormal = GetNormalForDiscrete(br);
	Vector3 tlNormal = GetNormalForDiscrete(tl);
	Vector3 trNormal = GetNormalForDiscrete(tr);

	Vector3 nbot = Interpolate(blNormal, brNormal, fmodf(test_x,1));
	Vector3 ntop = Interpolate(tlNormal, trNormal, fmodf(test_x,1));
	Vector3 nfinal = Interpolate( nbot, ntop, fmodf(test_y,1));
	nfinal.Normalize();
	return nfinal;
}

//-----------------------------------------------------------------------------------------------
// Returns the positionat XZ
//
Vector3 GameMap::GetPositionForXZ(const Vector2& pos) const
{
	Vector3 position;
	position.x = pos.x;
	position.z = pos.y;
	position.y = GetLinearHeight(pos);

	return position;
}

//-----------------------------------------------------------------------------------------------
// Returns the bounds of the map
//
AABB2 GameMap::GetBounds() const
{
	return m_extents;
}

//-----------------------------------------------------------------------------------------------
// Returns true if the point is below the terrain
//
bool GameMap::IsPointBelow(const Vector3& point)
{
	float terrainHeight = GetLinearHeight(point.xz());
	return (terrainHeight - point.y) > 0;
}

//-----------------------------------------------------------------------------------------------
// Loads the map from the heightmap image
//
void GameMap::LoadFromImage(Image& image, const AABB2& extents, float minHeight, float maxHeight, const IntVector2& chunkLayout)
{
	m_heightMap = &image;
	IntVector2 imageDimensions = image.GetDimensions();
	m_extents = extents; 
	Vector2 mapSize = m_extents.maxs - m_extents.mins;	

	// Load the heights
	for(int rowIndex = 0; rowIndex < imageDimensions.x; ++rowIndex)
	{
		for(int colIndex = 0; colIndex < imageDimensions.y; ++colIndex)
		{
			Rgba pixel = image.GetTexel(rowIndex, colIndex);
			float height = RangeMapFloat((float) pixel.r, 0.f, 255.f, minHeight, maxHeight);
			m_heights.push_back(height);
		}
	}
	
	IntVector2 sampleCounts(imageDimensions.x / chunkLayout.x, imageDimensions.y / chunkLayout.y);
	Vector2 chunkSize;
	chunkSize.x = mapSize.x / (float) chunkLayout.x;
	chunkSize.y = mapSize.y / (float) chunkLayout.y;
	m_cellSize.x = mapSize.x / (float) sampleCounts.x;
	m_cellSize.y = mapSize.y / (float) sampleCounts.y;
	for(int rowIndex = 0; rowIndex < chunkLayout.y; ++rowIndex)
	{
		for(int colIndex = 0; colIndex < chunkLayout.x; ++colIndex)
		{
			GameMapChunk* chunk = new GameMapChunk(this, IntVector2(rowIndex, colIndex));
			MeshBuilder builder;
			float umin = m_extents.mins.x + chunkSize.x * colIndex;
			float umax = umin + chunkSize.x;
			float vmin = m_extents.mins.y + chunkSize.y * rowIndex;
			float vmax = vmin + chunkSize.y;
			builder.Begin(PRIMITIVE_TRIANGLES, true);
			builder.AddSurfacePatch([chunk](float u, float v) { return chunk->GenerateTerrain(u,v); } , umin, umax, sampleCounts.y, vmin, vmax, sampleCounts.x);
			builder.End();
			chunk->m_renderable->SetMesh(builder.CreateMesh<VertexLit>());
			m_chunks.push_back(chunk);
		}
	}

	m_mapGenerated = true; // Debug thingy
	
}

//-----------------------------------------------------------------------------------------------
// Destroys all chunks and frees the memory
//
void GameMap::FreeAllChunks()
{
	for(GameMapChunk* chunk : m_chunks)
	{
		delete chunk;
		chunk = nullptr;
	}

	m_chunks.clear();
}

//-----------------------------------------------------------------------------------------------
// Returns the distance from the terrain
//
float GameMap::GetDistanceFromTerrain(const Vector3& point)
{
	Vector3 terrainPoint = GetPositionForXZ(point.xz());
	
	return (terrainPoint - point).GetLength();
}

//-----------------------------------------------------------------------------------------------
// Raycasts agains the terrain and returns a hit result
//
RaycastHit GameMap::Raycast(Ray3& ray, float maxDistance)
{
	Vector3 abovePoint = ray.start;
	Vector3 belowPoint = ray.Evaluate(maxDistance);
	Vector3 point = Average(abovePoint, belowPoint);

	int stepCount = 0;
	while(GetDistanceFromTerrain(point) > EPSILON && stepCount < MAX_RAYCAST_STEPS)
	{
		if(IsPointBelow(point))
		{
			belowPoint = point;
		}
		else
		{
			abovePoint = point;
		}

		point = Average(abovePoint, belowPoint);
	
		stepCount++;
	}

	RaycastHit hitResult;
	if(stepCount >= MAX_RAYCAST_STEPS)
	{
		return hitResult;
	}

	hitResult.hit = true;
	hitResult.position = point;
	hitResult.normal = GetNormalAtPosition(point);
	

	return hitResult;
}
