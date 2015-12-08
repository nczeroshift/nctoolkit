
#pragma vertex_shader_dx9

#pragma pixel_shader_dx9

#pragma vertex_shader_glx2

varying vec3 nnor;

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
	gl_FragColor = vec4(vec3(1.000,0.387,0)*zn+zn*zn*vec3(1.000,0.587,0),1.0);
}

