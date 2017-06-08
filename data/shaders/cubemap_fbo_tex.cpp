
#pragma vertex_shader_glx2

#include "vsh.cpp"
#include "material_vsh.cpp"

//uniform mat4 altModelMatrix;
//uniform mat3 altNormalMatrix;

//uniform mat4 gphModelMatrix;
//uniform mat4 matLampProjView;
//varying vec4 vPosInLight;

void main()
{
	vec4 P = gl_Vertex;
	vec3 N = gl_Normal;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	material_vsh_prepare(gl_ModelViewMatrix,gl_NormalMatrix,P.xyz, N);
	
	gl_Position = gl_ModelViewProjectionMatrix * P;
	
	//vPosInLight = matLampProjView * gphModelMatrix * gl_Vertex;
}

#pragma fragment_shader_glx2

#include "fsh.cpp"
#include "material.cpp"

uniform sampler2D gphTexture0;
//uniform float texSize;
//varying vec4 vPosInLight;

void main()
{
    vec4 tex = texture2D(gphTexture0, gl_TexCoord[0].xy);
	vec3 color = material_diffuse_phong(lamp_mv_pos,vec2(0.0,0.0));
	gl_FragColor = vec4(mix(color,tex.xyz * color,tex.w), gphAlpha);
}


