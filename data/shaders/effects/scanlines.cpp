
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#pragma vertex_shader_glx2

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_FrontColor = gl_Color;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}

#pragma fragment_shader_glx2

void main()
{
	float dist = length(gl_TexCoord[0].xyz-vec3(0.5,0.5,0.0));
	dist = dist*dist;
	
	float g = gl_TexCoord[0].y * 160 - int(gl_TexCoord[0].y * 160);
	
	gl_FragColor = vec4(vec3(0,0,0), dist+g*0.2);
}


