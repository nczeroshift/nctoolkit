
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifdef VSH

uniform mat4 shadow_pmv;
varying vec4 shadow_p_pmv;

void shadow_transform_to_light(vec4 P){
    shadow_p_pmv = shadow_pmv * gphModelMatrix * P;
}

#endif 

#ifdef FSH

uniform vec4 shadow_params;
varying vec4 shadow_p_pmv;

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

void shadow_cast(sampler2D tex){
    return texture2DShadowLerp(tex,shadow_params.xy,shadow_p_pmv,shadow_params.z);
}

#endif