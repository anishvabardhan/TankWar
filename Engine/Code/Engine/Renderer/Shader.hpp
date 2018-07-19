#pragma once
#include <string>
#include <map>
#include "Engine/Core/XMLUtils.hpp"

//-----------------------------------------------------------------------------------------------
// Forward Declarations
class ShaderProgram;

//-----------------------------------------------------------------------------------------------
enum CullMode 
{
	CULLMODE_BACK,			// GL_BACK     glEnable(GL_CULL_FACE); glCullFace(GL_BACK); 
	CULLMODE_FRONT,			// GL_FRONT    glEnable(GL_CULL_FACE); glCullFace(GL_FRONT); 
	CULLMODE_NONE,			// GL_NONE     glDisable(GL_CULL_FACE)
};
static const std::map<std::string, CullMode> ParseCullMode = 
{
	{"back",	CULLMODE_BACK},
	{"front",	CULLMODE_FRONT},
	{"none",	CULLMODE_NONE}
};

//-----------------------------------------------------------------------------------------------
enum FillMode 
{
	FILLMODE_SOLID,         // GL_FILL     glPolygonMode( GL_FRONT_AND_BACK, GL_FILL )
	FILLMODE_WIRE,          // GL_LINE     glPolygonMode( GL_FRONT_AND_BACK, GL_LINE )
};
static const std::map<std::string, FillMode> ParseFillMode = 
{
	{"solid",	FILLMODE_SOLID},
	{"wire",	FILLMODE_WIRE}
};

//-----------------------------------------------------------------------------------------------
enum WindOrder 
{
	WIND_CLOCKWISE,         // GL_CW       glFrontFace( GL_CW ); 
	WIND_COUNTER_CLOCKWISE, // GL_CCW      glFrontFace( GL_CCW ); 
};
static const std::map<std::string, WindOrder> ParseWindOrder = 
{
	{"cw",	WIND_CLOCKWISE},
	{"ccw",	WIND_COUNTER_CLOCKWISE}
};

//-----------------------------------------------------------------------------------------------
enum DepthTestOp
{
	COMPARE_LESS,
	COMPARE_GREATER,
	COMPARE_GEQUAL,
	COMPARE_LEQUAL,
	COMPARE_NOTEQUAL,
	COMPARE_NEVER,
	COMPARE_ALWAYS
};
static const std::map<std::string, DepthTestOp> ParseDepthTest = 
{
	{"less",		COMPARE_LESS},
	{"greater",		COMPARE_GREATER},
	{"gequal",		COMPARE_GEQUAL},
	{"lequal",		COMPARE_LEQUAL},
	{"notequal",	COMPARE_NOTEQUAL},
	{"never",		COMPARE_NEVER},
	{"always",		COMPARE_ALWAYS}
};

//-----------------------------------------------------------------------------------------------
enum BlendFactor
{
	BLEND_ZERO,
	BLEND_ONE,
	BLEND_ONE_MINUS_SRC_ALPHA,
	BLEND_SRC_ALPHA,
	BLEND_SRC_COLOR,
	BLEND_ONE_MINUS_SRC_COLOR,
	BLEND_DST_ALPHA,
	BLEND_ONE_MINUS_DST_ALPHA,
	BLEND_DST_COLOR,
	BLEND_ONE_MINUS_DST_COLOR,
	BLEND_CONSTANT,
	BLEND_ONE_MINUS_CONSTANT
};
static const std::map<std::string, BlendFactor> ParseBlendFactor = 
{
	{"zero",			BLEND_ZERO},
	{"one",				BLEND_ONE},
	{"inv_src_alpha",	BLEND_ONE_MINUS_SRC_ALPHA},	
	{"src_alpha",		BLEND_SRC_ALPHA},
	{"src_color",		BLEND_SRC_COLOR},
	{"inv_src_color",	BLEND_ONE_MINUS_SRC_COLOR},
	{"dst_alpha",		BLEND_DST_ALPHA},
	{"inv_dst_alpha",	BLEND_ONE_MINUS_DST_ALPHA},
	{"dst_color",		BLEND_DST_COLOR},
	{"inv_dst_color",	BLEND_ONE_MINUS_DST_COLOR},
	{"constant",		BLEND_CONSTANT},
	{"inv_constant",	BLEND_ONE_MINUS_CONSTANT}
};

//-----------------------------------------------------------------------------------------------
enum BlendOp
{
	BLEND_OP_ADD,
	BLEND_OP_SUBTRACT,
	BLEND_OP_REVERSE_SUBTRACT,
	BLEND_OP_MIN,
	BLEND_OP_MAX
};
static const std::map<std::string, BlendOp> ParseBlendOp =
{
	{"add",				BLEND_OP_ADD},
	{"subtract",		BLEND_OP_SUBTRACT},
	{"rev_subtract",	BLEND_OP_REVERSE_SUBTRACT},
	{"min",				BLEND_OP_MIN},
	{"max",				BLEND_OP_MAX}
};

//-----------------------------------------------------------------------------------------------
struct RenderState 
{
	// Raster State Control									
	CullMode		m_cullMode			= CULLMODE_BACK;				
	FillMode		m_fillMode			= FILLMODE_SOLID;			
	WindOrder		m_frontFace			= WIND_COUNTER_CLOCKWISE;	

	// Depth State Control
	DepthTestOp		m_depthCompare		= COMPARE_LESS;			
	bool			m_depthWrite		= true;			

	// Blend
	BlendOp			m_colorBlendOp		= BLEND_OP_ADD;	
	BlendFactor		m_colorSrcFactor	= BLEND_ONE;	
	BlendFactor		m_colorDstFactor	= BLEND_ZERO;	

	BlendOp			m_alphaBlendOp		= BLEND_OP_ADD;	
	BlendFactor		m_alphaSrcFactor	= BLEND_ONE;	
	BlendFactor		m_alphaDstFactor	= BLEND_ONE;	
}; 

//-----------------------------------------------------------------------------------------------
enum RenderQueue
{
	RENDER_QUEUE_OPAQUE,
	RENDER_QUEUE_ADDITIVE,
	RENDER_QUEUE_ALPHA
};
static const std::map<std::string, RenderQueue> ParseRenderQueue =
{
	{"opaque",			RENDER_QUEUE_OPAQUE},
	{"additive",		RENDER_QUEUE_ADDITIVE},
	{"alpha",			RENDER_QUEUE_ALPHA}
};

//-----------------------------------------------------------------------------------------------
class Shader
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	explicit Shader( ShaderProgram* program );
	explicit Shader( const tinyxml2::XMLElement& element );
	~Shader(){}
	
	//-----------------------------------------------------------------------------------------------
	// Accessors/Mutators
			bool			IsOpaque() const { return m_renderQueue == RENDER_QUEUE_OPAQUE; }
			void			SetProgram( ShaderProgram* program ) { m_program = program; }
			void			SetCullMode( CullMode mode ) { m_renderState.m_cullMode = mode; }
			void			SetFillMode( FillMode mode ) { m_renderState.m_fillMode = mode; }
			void			SetFrontFace( WindOrder order ) { m_renderState.m_frontFace = order; }
			void			SetRenderQueue( RenderQueue queue ) { m_renderQueue = queue; }
			void			SetSortOrder( int order ) { m_sortOrder = order; }
			ShaderProgram*	GetProgram() const { return m_program; }
			int				GetSortOrder() const { return m_sortOrder; }
			RenderQueue		GetRenderQueue() const { return m_renderQueue; }
	
	//-----------------------------------------------------------------------------------------------
	// Methods
			void			SetAlphaBlending( BlendOp op, BlendFactor src, BlendFactor dst ); 
			void			DisableAlphaBlending();
			void			SetColorBlending( BlendOp op, BlendFactor src, BlendFactor dst );
			void			DisableColorBlending();
			
			void			SetDepthTest( DepthTestOp compare, bool write ); 
			void			DisableDepth() { SetDepthTest( COMPARE_ALWAYS, false ); }

	//-----------------------------------------------------------------------------------------------
	// Static Methods
	static	Shader*			AcquireResource( const std::string& path );
	static	Shader*			CreateOrGetResource( const std::string& path );
	
	//-----------------------------------------------------------------------------------------------
	// Members
			ShaderProgram*		m_program;
			RenderState			m_renderState;
			RenderQueue			m_renderQueue = RENDER_QUEUE_OPAQUE;
			int					m_sortOrder = 0;

	//-----------------------------------------------------------------------------------------------
	// Static members
	static	std::map<std::string, Shader*> m_loadedShaders;
};

