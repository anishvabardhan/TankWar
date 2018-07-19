#pragma once
#include "Engine\Math\IntVector2.hpp"

//-----------------------------------------------------------------------------------------------
// Forward Declarations
class GameMap;
class Renderable;
class Vector3;

//-----------------------------------------------------------------------------------------------
class GameMapChunk
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	GameMapChunk( GameMap* gameMap, const IntVector2& coords );
	~GameMapChunk();
	
	//-----------------------------------------------------------------------------------------------
	// Accessors/Mutators
	
	//-----------------------------------------------------------------------------------------------
	// Methods
			void	Cleanup();
			void	UpdateMesh(); 
			Vector3	GenerateTerrain( float u, float v );

	//-----------------------------------------------------------------------------------------------
	// Members
	GameMap*	m_map;
	IntVector2	m_chunkCoords;
	Renderable*	m_renderable;
};


