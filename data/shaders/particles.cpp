/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#pragma vertex_shader_glx2

varying vec2 uv;
varying vec3 vertex;
varying float alpha;

uniform float dt;
uniform float p1;
uniform float p2;
uniform float p3;
uniform float p4;

void main()
{
    vertex = gl_Vertex.xyz;
    uv = gl_MultiTexCoord0.xy;
  
    float size = p1;
    
    vec3 center = vec3(((uv - 0.5) * 2.0) * 32.0, 0.0);
    center.y += dt * p2;
    center.y = center.y - floor(center.y / 64.0) * 64.0;
    center.y -= 32.0;
    
    alpha = (center.y / 64.0)+0.5;
    
    center.z += vertex.z * 4.0;
    center.z += cos(alpha * 3.1415 + dt * p2) * p3;
    
    vec4 P = vec4(gl_Vertex.xyz * size, 1.0);
    
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ProjectionMatrix * (gl_ModelViewMatrix * vec4(center, 1.0) + vec4(P.xyz, 0.0));
	gl_FrontColor = gl_Color;
}

#pragma fragment_shader_glx2

varying float alpha;
varying vec2 uv;
varying vec3 vertex;

uniform float p4;

void main()
{
    float d = 1.0 - length(vertex.xy * 0.5)*2.0;
    if(d<0.0)
        d = 0.0;
    float v = pow(d,p4);
	gl_FragColor = vec4(vec3(1.0),(1.0-alpha) * v);
}


