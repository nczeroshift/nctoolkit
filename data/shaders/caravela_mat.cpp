
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
	float4 col = tex2D(gphTexture0, IN.tex).xyzw;
	return float4(zn*col.xyz,col.w);
}

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

uniform sampler2D tex1;

void main()
{
	float zn = normalize(gl_TexCoord[1].xyz).z;
	vec4 col = texture2D(tex1, gl_TexCoord[0].xy).xyzw;
	gl_FragColor = vec4(col.xyz*zn,col.w);
}


