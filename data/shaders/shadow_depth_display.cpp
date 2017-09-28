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

uniform sampler2D gphTexture0;

void main()
{
    float d = texture2D(gphTexture0, gl_TexCoord[0].xy).r;
	gl_FragColor = vec4(vec3(d),1.0);
}


