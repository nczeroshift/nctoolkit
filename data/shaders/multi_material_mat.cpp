
#pragma vertex_shader_glx2

#include "vsh.cpp"
#include "material_vsh.cpp"

void main()
{
	vec4 P = gl_Vertex;
	vec3 N = gl_Normal;
	
	material_vsh_prepare(gl_ModelViewMatrix,gl_NormalMatrix,P, N);
	
	gl_Position = gl_ModelViewProjectionMatrix * P;
}

#pragma fragment_shader_glx2

#include "fsh.cpp"
#include "material.cpp"

void main()
{
	vec3 color = material_diffuse_phong(lamp_mv_pos,vec2(0.0,0.0));
	gl_FragColor = vec4(color, gphAlpha);
}


