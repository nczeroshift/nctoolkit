
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#pragma vertex_shader_dx9
struct InputVSH{float4 pos:POSITION;float2 tex:TEXCOORD0;float4 col:COLOR0;};
struct OutputVSH{float4 pos:POSITION;float2 tex:TEXCOORD0;float4 col:TEXCOORD1;};
void gphMainVSH(in InputVSH IN,out OutputVSH OUT){
	OUT.pos = mul(IN.pos,gphProjectionModelViewMatrix);
	OUT.tex = IN.tex;OUT.col = IN.col;
}
#pragma pixel_shader_dx9
uniform float4 properties;
float4 test(float2 p, float2 d, float range){
	if(p.x >= range || p.y >= range) return float4(1.0,1.0,1.0,1.0);
	return tex2D(gphTexture0,(p*0.5)/float2(range,range));
}
struct InputPSH{float2 tex:TEXCOORD0;float4 col:TEXCOORD1;};
float4 gphMainPSH(InputPSH IN):COLOR{
	if(IN.tex.x>0.5)IN.tex.x = 1.0-IN.tex.x;
	if(IN.tex.y>0.5)IN.tex.y = 1.0-IN.tex.y;
	IN.tex*=properties.xy;
	return IN.col*test(IN.tex,properties.xy,properties.z);
}
#pragma vertex_shader_glx2
void main(){
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_FrontColor = gl_Color;
}
#pragma fragment_shader_glx2
uniform vec4 properties;
uniform sampler2D texture_0;
vec4 test(vec2 p, vec2 d, float range){
	if(p.x >= range || p.y >= range) return vec4(1,1,1,1);
	return texture2D(texture_0,(p*0.5)/vec2(range,range));
}
void main(){
	vec2 pos = gl_TexCoord[0].xy;
	if(pos.x>0.5)pos.x = 1.0-pos.x;
	if(pos.y>0.5)pos.y = 1.0-pos.y;
	pos*=properties.xy;
	gl_FragColor = gl_Color*test(pos,properties.xy,properties.z);
}