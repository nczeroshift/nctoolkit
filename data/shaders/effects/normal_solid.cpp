
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

uniform vec4 gphSpecularColor;
uniform vec4 gphDiffuseColor;
uniform vec4 gphAmbientColor;

void main()
{
	vec3 n = normalize(gl_TexCoord[1].xyz);
	float zN = clamp(n.z,0,1);
	float zS1 = clamp(dot(n,vec3(-0.607,0.1,0.707)),0,1);
	float zS2 = clamp(dot(n,vec3(0.407,-0.607,0.2)),0,1);
	zS1 = zS1 * zS1;
	zS2 = zS2 * zS2;
	vec3 spec = gphSpecularColor.xyz * (zS1 + zS2 * 0.5);
	vec3 diff = gphDiffuseColor.xyz * zN; //gphAmbientColor.xyz + diff +
	gl_FragColor = vec4(clamp( spec+gphAmbientColor.xyz + diff ,0,1),1.0);
}
