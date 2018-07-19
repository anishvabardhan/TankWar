#include "Game/Tank.hpp"
//-----------------------------------------------------------------------------------------------
// Game Includes
#include "Game/GameMap.hpp"
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/DebugRenderUtils.hpp"
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Constructor
//
Tank::Tank() 
	: GameObject()
{
	Mesh* sphere = Renderer::GetInstance()->CreateOrGetMesh("Cube");
	SetMesh(sphere);

	Material* mat = Renderer::GetInstance()->CreateOrGetMaterial("Data/Materials/Tank.mat");
	SetMaterial(*mat);
}

//-----------------------------------------------------------------------------------------------
// Destructor
//
Tank::~Tank()
{

}

//-----------------------------------------------------------------------------------------------
// Updates the tank 
//
void Tank::Update(float deltaSeconds)
{
	Matrix44 alignmentMatrix = ComputeAlignmentMatrix();
	m_transform->SetLocalMatrix(alignmentMatrix);
	DebugRenderBasis(0.f, alignmentMatrix, 2.f);

	// Adjust height based on position XZ
	Vector3 position = m_transform->GetWorldPosition();
	position.y = m_map->GetLinearHeight(position.xz()) + 0.5f;
	m_transform->SetPosition(position);

	UNUSED(deltaSeconds);
}

//-----------------------------------------------------------------------------------------------
// Compute the terrain alignment matrix for the tank position
//
Matrix44 Tank::ComputeAlignmentMatrix()
{
	Vector3 normal = m_map->GetNormalAtPosition(m_transform->GetWorldPosition());

	Vector3 right = CrossProduct(Vector3::UP, m_transform->GetForward());
	Vector3 correctForward = CrossProduct(right, normal);
	Vector3 correctRight = CrossProduct(normal, correctForward);

	Vector3 translation = m_transform->GetWorldPosition();

	Matrix44 mat(correctRight, normal, correctForward, translation);

	return mat;
}