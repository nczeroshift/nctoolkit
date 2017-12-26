
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
    gl_FragData[0] = vec4(gl_TexCoord[1].xyz,1.0);
    gl_FragData[1] = vec4(vec3(zn),1.0);
}


