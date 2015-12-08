
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#pragma vertex_shader_glx2

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

#pragma fragment_shader_glx2

uniform sampler2D gphTexture0;

void main()
{
	vec4 c = texture2D(gphTexture0, gl_TexCoord[0].xy).xyzw;
	gl_FragColor = vec4(1.0-c.xyz,c.w);
}


