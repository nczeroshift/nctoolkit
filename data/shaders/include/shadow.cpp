
/**
* 
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

float Shadow_Bilinear_Filter(sampler2D sbuffer,vec4 fcoords, float zn, float mapsize, float epsilon)
{
	/* Convert the coordinates from clipping space to texture space. */
	vec2 l_ShadowUVCoord = 0.5 * fcoords.xy / fcoords.w + vec2( 0.5, 0.5 );
	/*l_ShadowUVCoord.y = 1.0 - l_ShadowUVCoord.y;*/
	
	if(l_ShadowUVCoord.y > 1.0 || l_ShadowUVCoord.y < 0.0 || l_ShadowUVCoord.x > 1.0 || l_ShadowUVCoord.x < 0.0)
		return 0;

	/* Fragment depth value in lamp clipping space. */
	/*fcoords.z / fcoords.w*/;
	float l_FragmentDepth = zn;	
		
	/*	Distance to the nearest texel. */
	float l_Inv_Mapsize = 1.0/mapsize;
	
	/* Sample the shadow buffer and compare. */
	float l_Samples[4];
	l_Samples[0] = ( texture2D( sbuffer, l_ShadowUVCoord ).x + epsilon ) < l_FragmentDepth ? 0.0: 1.0; 
	l_Samples[1] = ( texture2D( sbuffer, l_ShadowUVCoord + vec2(l_Inv_Mapsize, 0.0) ).x + epsilon) < l_FragmentDepth  ? 0.0 : 1.0;
	l_Samples[2] = ( texture2D( sbuffer, l_ShadowUVCoord + vec2(0.0, l_Inv_Mapsize) ).x + epsilon) < l_FragmentDepth  ? 0.0 : 1.0;
	l_Samples[3] = ( texture2D( sbuffer, l_ShadowUVCoord + vec2(l_Inv_Mapsize,l_Inv_Mapsize) ).x + epsilon ) < l_FragmentDepth  ? 0.0 : 1.0;
	
	/* Get the decimal part to interpolate correcly the samples. */
	vec2 l_Lerps = frac( mapsize * l_ShadowUVCoord );	
	
	/* Bilinear interpolation. */
	return mix(	mix( l_Samples[0], l_Samples[1], l_Lerps.x ),
					mix( l_Samples[2], l_Samples[3], l_Lerps.x ), l_Lerps.y );
}
