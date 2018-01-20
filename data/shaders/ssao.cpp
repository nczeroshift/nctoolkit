
#pragma vertex_shader_glx2

uniform mat4 gphModelMatrix;

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_FrontColor = gl_Color;
}

#pragma fragment_shader_glx2

uniform sampler2D gphTexture0;
uniform sampler2D gphTexture1;
uniform sampler2D gphTexture2;

vec2 rand2(vec2 co){
    float d = dot(co.xy ,vec2(12.9898,78.233));
    return (vec2(fract(sin(d) * 43758.5453),fract(cos(d) * 43758.5453))-vec2(0.5)) * 2.0;
}

void main()
{

    float max_dist = 5.0;
    float min_dist = 1.0;
    
    vec2 uv = gl_TexCoord[0].xy;

    vec4 ref_color = texture2D(gphTexture0,uv);
    vec3 ref_pos = texture2D(gphTexture1,uv).xyz;
    vec3 ref_nor = normalize(texture2D(gphTexture2,uv).xyz);
   
    float max_samples = 32.0;
   
    float r = 0.0;
    vec3 m_nor = vec3(0.0);
    
    float ssao_params = 0.1;
    
    for(float i=0.0;i<max_samples;i+=1.0)
    {
        float d = mix(min_dist,max_dist,i/max_samples);
        
        vec2 sample_uv = uv + (d * max_dist * rand2(uv*i)) / vec2(1000.0,600.0);
        vec3 sample_pos = texture2D(gphTexture1,sample_uv).xyz;

        vec3 dir = sample_pos - ref_pos;
        
        float len = length(dir);
		float f = dot(dir, ref_nor);
		
        if (f > 0.05 * len)
            r += f * 1.0 / (len * (1.0 + len * len * ssao_params));
     
    }
  
    r /= max_samples;
    
    r = pow(1.0 - r,2.0);
    
    gl_FragColor = vec4((vec3(0.2) + ref_color.xyz * 0.8)* r ,1.0); 
}

