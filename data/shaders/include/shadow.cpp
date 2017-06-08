
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

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
