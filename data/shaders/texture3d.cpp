/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#pragma vertex_shader_glx2

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_FrontColor = gl_Color;
}

#pragma fragment_shader_glx2

uniform sampler3D texture0;
uniform float depth;

void main()
{
    vec4 col = tex3D(texture0, vec3(gl_TexCoord[0].xy,depth));
	gl_FragColor = col;
}


