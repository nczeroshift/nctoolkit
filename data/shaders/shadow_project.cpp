/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#pragma vertex_shader_glx2

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_FrontColor = gl_Color;
}

#pragma fragment_shader_glx2

uniform mat4 shadow_pmv;
uniform vec4 shadow_params;

uniform sampler2D gphTexture0; // Diffuse
uniform sampler2D gphTexture1; // (MV)P
uniform sampler2D gphTexture2; // (MV)N

uniform sampler2D gphTexture3; // Lamp Depth 1
uniform sampler2D gphTexture4; // Lamp Depth 2
uniform sampler2D gphTexture5; // Lamp Depth 3
uniform sampler2D gphTexture6; // Lamp Depth 4
uniform sampler2D gphTexture7; // Lamp Depth 5

uniform mat4 lamp_pmv[6];
uniform vec4 lamp_params[6];
uniform vec4 lamp_dir;

float texture2DCompare(sampler2D depths, vec2 uv, float compare){
    float depth = texture2D(depths, uv).r;
    return step(compare, depth);
}

float texture2DShadowLerp(sampler2D depths, vec2 size, vec4 coord, float bias){
	vec2 uv = 0.5 * coord.xy / coord.w + vec2( 0.5 );
	
	if(uv.y > 1.0 || uv.y < 0.0 || uv.x > 1.0 || uv.x < 0.0)
		return 0.0;
		
    vec2 texelSize = vec2(1.0)/size;
    vec2 f = fract(uv*size+0.5);
    vec2 centroidUV = floor(uv*size+0.5)/size;
	
	float depth = coord.z / coord.w - bias;
	
    float lb = texture2DCompare(depths, centroidUV+texelSize*vec2(0.0, 0.0), depth);
    float lt = texture2DCompare(depths, centroidUV+texelSize*vec2(0.0, 1.0), depth);
    float rb = texture2DCompare(depths, centroidUV+texelSize*vec2(1.0, 0.0), depth);
    float rt = texture2DCompare(depths, centroidUV+texelSize*vec2(1.0, 1.0), depth);
	
    float a = mix(lb, lt, f.y);
    float b = mix(rb, rt, f.y);
    float c = mix(a, b, f.x);
	
    return c;
}


void main()
{
    vec2 uv = gl_TexCoord[0].xy;
    vec3 col = texture2D(gphTexture0, uv).rgb;
    vec3 p = texture2D(gphTexture1, uv).rgb;
    vec3 n = normalize(texture2D(gphTexture2, uv).rgb);
    
    vec4 p_proj = shadow_pmv * vec4(p,1.0);
    
    float tt = texture2DShadowLerp(gphTexture3, shadow_params.xy, p_proj, shadow_params.z);
     
     float dz = dot(normalize(lamp_dir.xyz), -n) > 0.0 ? 1.0 : tt ;
     
	gl_FragColor = vec4(col * vec3(dz* 0.5 + 0.5),1.0); //vec4(vec3(dz),1.0);//
}


