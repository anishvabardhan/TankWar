#pragma once
#include <vector>
#include "Engine\Math\IntVector2.hpp"
#include "Engine\Math\AABB2.hpp"
#include "Engine\Math\RaycastHit3D.hpp"

//-----------------------------------------------------------------------------------------------
// Forward Declarations
class Image;
class Vector2;
class GameMapChunk;
class Renderable;

//-----------------------------------------------------------------------------------------------
class GameMap
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	GameMap();
	~GameMap();
	
	//-----------------------------------------------------------------------------------------------
	// Accessors/Mutators
	float		GetHeightForDiscrete( const IntVector2& pos ) const;
	Vector3		GetNormalForDiscrete( const IntVector2& coord ) const;
	Vector3		GetPositionForDiscrete( const IntVector2& coord ) const;
	float		GetLinearHeight( const Vector2& pos ) const;
	Vector3		GetNormalAtPosition( const Vector3& pos ) const; 
	Vector3		GetPositionForXZ( const Vector2& pos ) const;
	AABB2		GetBounds() const; 

	//-----------------------------------------------------------------------------------------------
	// Methods
	bool		IsPointBelow( const Vector3& point );
	void		LoadFromImage( Image& image, const AABB2& extents, float minHeight, float maxHeight, const IntVector2& chunkLayout );
	void		FreeAllChunks();
	float		GetDistanceFromTerrain( const Vector3& point );
	RaycastHit	Raycast( Ray3& ray, float maxDistance );

	//-----------------------------------------------------------------------------------------------
	// Members
	AABB2						m_extents;
	Image*						m_heightMap;
	float						m_minHeight;
	float						m_maxHeight;
	std::vector<GameMapChunk*>	m_chunks;
	std::vector<float>			m_heights;
	std::vector<Vector3>		m_normals;
	IntVector2					m_chunkLayout;
	Vector2						m_cellSize;
	bool						m_mapGenerated = false;
};


