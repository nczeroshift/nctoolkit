
#pragma vertex_shader_glx2

#include "material.cpp"

uniform mat4 gphModelMatrix;
uniform mat4 matLampProjView;

varying vec4 vPosInLight;

void main()
{
	vec4 P = gl_Vertex;
	vec3 N = gl_Normal;
	
	material_vsh_prepare(P, N);
	
	gl_Position = gl_ModelViewProjectionMatrix * P;
	
	vPosInLight = matLampProjView * gphModelMatrix * gl_Vertex;
}

#pragma fragment_shader_glx2

#include "material.cpp"
#include "shadow.cpp"

uniform sampler2D gphTexture0;
uniform float texSize;
varying vec4 vPosInLight;

void main()
{
	float bias = 0.00001;
	float shadowFactor = texture2DShadowLerp(gphTexture0, texSize, vPosInLight, bias); 
	vec3 color = material_diffuse_phong(lamp_mv_pos,vec2(0.0,0.0));
	gl_FragColor = vec4( vec3(color)*(0.5 + shadowFactor * 0.5), gphAlpha);
}


