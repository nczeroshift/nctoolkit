
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#pragma vertex_shader_glx2

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_FrontColor = gl_Color;
	gl_TexCoord[1].xyz = normalize(gl_NormalMatrix*gl_Normal);
}

#pragma fragment_shader_glx2

void main()
{
	float zn = normalize(gl_TexCoord[1].xyz).z;
	gl_FragColor = texture2D(gphTexture0,gl_TexCoord[0].xy) * vec4(zn,zn,zn,1.0);
}
