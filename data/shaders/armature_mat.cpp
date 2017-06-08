#pragma vertex_shader_glx2
#version 120
#include "vsh.cpp"
#include "material_vsh.cpp"

uniform mat4 bones_matrix[32];

varying vec3 lightVec;
varying vec3 halfVec;
varying vec3 eyeVec;
	
void computeBumpmapVectors(vec3 pRef, vec3 nRef){
    vec4 position 	= vec4(pRef,1.0);
	vec3 normal 	= nRef;
	vec2 uv 		= gl_MultiTexCoord0.xy;
	vec3 tangent 	= gl_MultiTexCoord1.xyz;
	
	vec3 n = normalize (gl_NormalMatrix * normal);
	vec3 t = normalize (gl_NormalMatrix * tangent);
	vec3 b = cross (n, t);
	
	vec3 lightPos = vec3(gl_ModelViewMatrix * vec4(0,0,10.0,0)); 
	
	vec3 vertexPosition = vec3(gl_ModelViewMatrix *  position);
	vec3 lightDir = normalize(lightPos - vertexPosition);
	
	vec3 v;
	v.x = dot (lightDir, t);
	v.y = dot (lightDir, b);
	v.z = dot (lightDir, n);
	lightVec = normalize (v);
	
	v.x = dot (vertexPosition, t);
	v.y = dot (vertexPosition, b);
	v.z = dot (vertexPosition, n);
	eyeVec = normalize (v);
	
	vertexPosition = normalize(vertexPosition);
	
	vec3 halfVector = normalize(vertexPosition + lightDir);
	v.x = dot (halfVector, t);
	v.y = dot (halfVector, b);
	v.z = dot (halfVector, n);

	halfVec = v ; 
}

void main()
{
	vec3 P = vec3(0.0);
	vec3 N = vec3(0.0);
	
    gl_TexCoord[0].xy = gl_MultiTexCoord0.xy;
    
    vec4 bId = gl_MultiTexCoord2;
    vec4 bWeight = gl_MultiTexCoord3;
   
    for(int i = 0; i < 4;i ++){
        float w = bWeight.x;
        int id = int(bId.x);
        if(w <= 0.0001) continue;
        
        P += (bones_matrix[id] * (vec4(gl_Vertex.xyz, 1.0)) * w).xyz;
        N += (bones_matrix[id] * (vec4(gl_Normal, 0.0)) * w).xyz;
        
        bWeight = bWeight.yzwx;
        bId = bId.yzwx;
    }

    N = normalize(N);
    
    computeBumpmapVectors(P,N);
    
    material_vsh_prepare(gl_ModelViewMatrix, gl_NormalMatrix, P, N);
	
	gl_Position = gl_ModelViewProjectionMatrix * vec4(P, 1.0);
}

#pragma fragment_shader_glx2

#include "fsh.cpp"
#include "material.cpp"

uniform sampler2D gphTexture0;

void main()
{
    vec3 normal = 2.0 * texture2D (gphTexture0, gl_TexCoord[0].xy).rgb - 1.0;
	normal = normalize (normal);
    
    //v_nm_nor = normal;
    //gl_FragColor = vec4(vec3(dot(normal,vec3(0,0,1))),1.0);
    
	vec3 color = material_diffuse_phong(lamp_mv_pos,vec2(0.0,0.0));
	gl_FragColor = vec4(color, gphAlpha);
}


