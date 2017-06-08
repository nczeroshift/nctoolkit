
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#pragma vertex_shader_glx2

#include "vsh.cpp"
#include "material_vsh.cpp"

uniform mat4 matLampProjView;
varying vec4 vPosInLight;

void main()
{
	vec4 P = gl_Vertex;
	vec3 N = gl_Normal;
	
	material_vsh_prepare(gl_ModelViewMatrix,gl_NormalMatrix,P.xyz, N);
	
	gl_Position = gl_ModelViewProjectionMatrix * P;
	
	vPosInLight = matLampProjView * gphModelMatrix * gl_Vertex;
}

#pragma fragment_shader_glx2

#include "fsh.cpp"
#include "material.cpp"
#include "shadow.cpp"

uniform sampler2D gphTexture0;
uniform float texSize;
varying vec4 vPosInLight;

void main()
{
	float bias = 0.00001;
	float shadowFactor = texture2DShadowLerp(gphTexture0, vec2(texSize), vPosInLight, bias); 
	vec3 color = material_diffuse_phong(lamp_mv_pos.xyz,vec2(0.0,0.0));
	gl_FragColor = vec4( vec3(color)*(0.5 + shadowFactor * 0.5), gphAlpha);
}


