
#pragma vertex_shader_dx9

#pragma pixel_shader_dx9

#pragma vertex_shader_glx2

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_Position;
	gl_FrontColor = gl_Color;
	gl_TexCoord[1].xyz = normalize(gl_NormalMatrix*gl_Normal);
	gl_TexCoord[2].xy = gl_MultiTexCoord0.xy;
}

#pragma fragment_shader_glx2

uniform sampler2D diffuse;
uniform sampler3D volume;

void main()
{
	vec2 uv = (gl_TexCoord[0].xy+vec2(32.0,24.0))*vec2(1.0/64.0,1.0/48.0);
	uv *= 32.0;

	const float mpv = 1.0/6.0;
	
	vec3 tex = texture2D(diffuse, gl_TexCoord[2].xy).xyz;
	float gray = 0.8*(tex.x + tex.y + tex.z)/3.0 + 0.2;

	float norval = normalize(gl_TexCoord[1].xyz).z;

	float zn = 1.0-clamp(norval*gray,0.0,1.0);

	zn = zn * 5.0 * mpv + mpv * 0.5;
	vec4 vtex = texture3D(volume,vec3(uv.xy,zn));

	gl_FragColor= vec4(vtex.xyz,1.0);
}

