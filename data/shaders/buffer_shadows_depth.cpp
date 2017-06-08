
#pragma vertex_shader_glx2

uniform mat4 gphModelMatrix;

varying vec4 projPos;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;	
	projPos = gl_ModelViewProjectionMatrix * gl_Vertex;
}

#pragma fragment_shader_glx2

varying vec4 projPos;

void main()
{
	float depth = projPos.z / projPos.w;
	gl_FragColor = vec4(vec3(depth),1.0);
}


