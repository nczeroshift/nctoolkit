
#pragma vertex_shader_glx2

#include "vsh.cpp"
#include "cubemap_vsh.cpp"
#include "material_vsh.cpp"

/*
uniform vec4 cameraPos;
uniform mat4 gphModelMatrix;
varying vec3 normalV;
varying vec3 eyeV;
*/

void main()
{
    vec4 P = gl_Vertex;
	vec3 N = gl_Normal;
    
    prepare_cubemap(P,N);
    
    gl_Position = gl_ModelViewProjectionMatrix * P;
    
    /*
	vec4 P = gl_Vertex;
	vec3 N = gl_Normal;
	
	material_vsh_prepare(P, N);
	
	
	    
    normalV = normalize(vec3(gphModelMatrix * vec4(N,0.0)));
    vec3 pos = vec3(gphModelMatrix * P);
    eyeV = normalize(pos-cameraPos.xyz);
    */
}

#pragma fragment_shader_glx2

#include "fsh.cpp"
#include "cubemap.cpp"
#include "material.cpp"

uniform samplerCube gphTexture0;

/*
uniform samplerCube gphTexture0;
varying vec3 normalV;
varying vec3 eyeV;
*/

void main()
{
    /*vec3 color = material_diffuse_phong(lamp_mv_pos,vec2(0.0,0.0));
    vec3 nn = normalize(normalV);
    vec3 ee = normalize(eyeV);
    vec3 tt = reflect(ee, nn); // refract(ee, nn, 1.1);
    gl_FragColor = vec4(textureCube(gphTexture0, tt).rgb,1.0);
    */
    
    gl_FragColor = cubemap_reflect(gphTexture0);
}


