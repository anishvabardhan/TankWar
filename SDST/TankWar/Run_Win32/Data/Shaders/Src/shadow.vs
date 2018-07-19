// define the shader version (this is required)
#version 420 core
 
// Attributes - input to this shasder stage (constant as far as the code is concerned)
in vec3 POSITION;
in vec4 COLOR;
in vec2 UV;

// Uniforms ==============================================
uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJECTION; 

// Outputs

// Entry point - required.  What does this stage do?
void main( void )
{
   vec4 local_pos = vec4( POSITION, 1.0f );  

   vec4 world_pos = MODEL * local_pos; 
   vec4 camera_pos = VIEW * world_pos; 
   vec4 clip_pos = PROJECTION * camera_pos; 

   gl_Position = clip_pos;
}
