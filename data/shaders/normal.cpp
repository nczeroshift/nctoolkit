
#pragma vertex_shader_dx9

struct InputVSH{
	float4 pos:POSITION;
	float3 nor:NORMAL;
	float2 tex:TEXCOORD0;
};

struct OutputVSH{
	float4 pos:POSITION;
	float3 vnor:TEXCOORD0;
	float2 tex:TEXCOORD1;
};

void gphMainVSH(in InputVSH IN,out OutputVSH OUT){
	OUT.pos = mul(IN.pos,gphProjectionModelViewMatrix);
	OUT.vnor = normalize(mul(float4(IN.nor,0.0),gphNormalMatrix).xyz);
	OUT.tex = IN.tex;
}

#pragma pixel_shader_dx9

struct InputPSH{
	float3 vnor:TEXCOORD0;
	float2 tex:TEXCOORD1;
};

float4 gphMainPSH(InputPSH IN):COLOR
{
	float zn = normalize(IN.vnor.xyz).z;
	return float4(zn,zn,zn,1.0);
}

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
	gl_FragColor = vec4(zn,zn,zn,1.0);
}


