
/**
* Lambert diffuse reflectance model.
*@param N Normalized surface normal in MV space.
*@param L Normalized (L-V) vector in MV space.
*@param color Diffuse color.
*@param k Diffuse intensity.
*@return Output color.
*/
vec3 Diffuse_Lambert(
		vec3 N, 
		vec3 L,
		vec3 color,
		float k)
{
	return color * clamp(dot(N,L),0.0,1.0) * k;
}

/**
* Blinn-Phong specular reflectance model.
*@param E Normalized (E-V) vector in MV space.
*@param N Normalized surface normal in MV space.
*@param L Normalized (L-V) vector in MV space.
*@param color Specular color.
*@param hardness Specular hardness.
*@param k Specular intensity.
*@return Output color.
*/
vec3 Specular_Blinn(
	vec3 E,
	vec3 N, 
	vec3 L,
	vec3 color,
	float hardness,
	float k)
{
	float a = dot(N,L);
	vec3 H = normalize(L+E);
	return color * pow( clamp(dot(H, N),0.0,1.0), hardness) * k;
}

/**
* Phong specular reflectance model.
*@param E Normalized (E-V) vector in MV space.
*@param N Normalized surface normal in MV space.
*@param L Normalized (L-V) vector in MV space.
*@param color Specular color.
*@param hardness Specular hardness.
*@param k Specular intensity.
*@return Output color.
*/
vec3 Specular_Phong(
	vec3 E,
	vec3 N, 
	vec3 L,
	vec3 color,
	float hardness,
	float k)
{
	vec3 R = reflect(-L, N);
	return color * pow( clamp(dot(R, E), 0.0,1.0), hardness ) * k;				   	
}

	
vec3 material_diffuse_phong(vec3 lamp_pos, vec2 attFactors){
	vec3 N = normalize(v_nm_nor);
	vec3 L = lamp_pos.xyz - v_mv_pos;
	float dist = length(L);
	L *= 1.0 / dist;
	float diff = clamp(dot(N,L),0.0,1.0);
	float attenuation = 1.0 / (1.0 + dist * attFactors.x + dist * dist * attFactors.y);
	
	vec3 E = -normalize(v_mv_pos);
	vec3 R = normalize(reflect(-L,N));
	
	float spec_phong = pow(clamp(dot(R,E),0.0,1.0),gphSpecularPower) * gphSpecularColor.w ;
	
	vec3 res = gphDiffuseColor.xyz * diff;
	/*res += spec_phong * gphSpecularColor.xyz;*/
	res += Specular_Phong(E,N,L,gphSpecularColor.xyz,gphSpecularPower,gphSpecularColor.w);
	
	
	return res * attenuation + gphAmbientColor.xyz;
}

vec3 material_diffuse_blinn(vec3 lamp_pos, vec2 attFactors){
	vec3 N = normalize(v_nm_nor);
	vec3 L = lamp_pos.xyz - v_mv_pos;
	float dist = length(L);
	L *= 1.0 / dist;
	float diff = clamp(dot(N,L),0.0,1.0);
	float attenuation = 1.0 / (1.0 + dist * attFactors.x + dist * dist * attFactors.y);
	
	vec3 E = -normalize(v_mv_pos);
	vec3 H = normalize(normalize(lamp_pos.xyz) + E);
	
	float spec_blinn_phong = pow(clamp(dot(N,H),0.0,1.0),gphSpecularPower) *gphSpecularColor.w ;

	vec3 res = gphDiffuseColor.xyz * diff;
	vec3 spec= spec_blinn_phong * gphSpecularColor.xyz;
	res += Specular_Blinn(E,N,L,gphSpecularColor.xyz,gphSpecularPower,gphSpecularColor.w);
	
	
	return res * attenuation + gphAmbientColor.xyz;
}