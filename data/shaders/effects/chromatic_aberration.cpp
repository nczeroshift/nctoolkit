 
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#pragma vertex_shader_glx2

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

#pragma fragment_shader_glx2

uniform sampler2D gphTexture0;

uniform vec4 imageSize;
uniform vec4 distortion;
uniform vec4 offset;

void main()
{
	vec2 scale = 1.0/imageSize.xy;

	float dist = length(gl_TexCoord[0].xy-vec2(0.5f,0.5f));
	
	float factor = (distortion.x + distortion.y * dist + (distortion.z + distortion.w * dist) * dist * dist);

	float cR = texture2D(gphTexture0, gl_TexCoord[0].xy+offset.xy-factor*scale).x;
	float cG = texture2D(gphTexture0, gl_TexCoord[0].xy+offset.xy+factor*scale).y;
	vec2 cBA = texture2D(gphTexture0, gl_TexCoord[0].xy+offset.xy).zw;
	
	gl_FragColor = vec4(cR,cG,cBA.x,cBA.y);
}


