
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

vec2 rand2(vec2 co){
    float d = dot(co.xy ,vec2(12.9898,78.233));
    return (vec2(fract(sin(d) * 43758.5453),fract(cos(d) * 43758.5453))-vec2(0.5)) * 2.0;
}

void main()
{
    float focus_distance = 5.0;
    float apperture_distance = 60.0;
    
    float max_dist = 16.0;
    float min_dist = 0.0;
    
    vec2 uv = gl_TexCoord[0].xy;

    float depth_a = abs((-texture2D(gphTexture1,uv).z) - focus_distance) / apperture_distance;
    depth_a = clamp(depth_a, 0.0, 1.0);
    float dist = mix(min_dist, max_dist, depth_a);
  
    vec3 res = vec3(0.0);
    
    float max_samples = 32.0;
    
    float samples = floor(mix(1.0,max_samples,depth_a));

    dist = floor(dist);
    for(float i = 0.0;i<samples;i+=1.0){
        vec2 uv_r = rand2(uv + vec2(i/samples)) * dist;
        if(i>0.0)
            res += texture2D(gphTexture0,uv+uv_r*vec2(1.0/1024,1.0/600.0)).xyz/samples;
        else
            res += texture2D(gphTexture0,uv).xyz/samples;
    }
    
    gl_FragColor = vec4(res,1.0);
}


