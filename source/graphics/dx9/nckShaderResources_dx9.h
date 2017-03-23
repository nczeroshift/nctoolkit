
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_SHADER_RESOURCES_DX9_H
#define NCK_SHADER_RESOURCES_DX9_H

#include "../nckGraphicsConfig.h"

_GRAPHICS_BEGIN

static char Program_Resources[] = NCK_CODE(
	uniform float4x4 gphProjectionModelViewMatrix;
	uniform float4x4 gphModelViewMatrix;
	uniform float4x4 gphModelMatrix;
	uniform float4x4 gphViewMatrix;
	uniform float4x4 gphNormalMatrix;

	uniform float4 gphLampPosition[8]; 
	uniform float4 gphLampColor[8];

	uniform float4 gphDiffuseColor;
	uniform float4 gphSpecularColor;
	uniform float4 gphAmbientColor;
	uniform float gphSpecularPower;

	uniform	float3 gphCameraPosition;

	uniform float gphShadowMapSize;
	uniform float gphShadowMapEpsilon;
	uniform float4x4 gphSpotPVMatrix;

	sampler	gphTexture0:register(s0);
	sampler	gphTexture1:register(s1);
	sampler	gphTexture2:register(s2);
	sampler	gphTexture3:register(s3);
	sampler	gphTexture4:register(s4);
	sampler	gphTexture5:register(s5);
	sampler	gphTexture6:register(s6);
	sampler	gphTexture7:register(s7);

	struct ModelVertex{
		float4 pos		: POSITION;
		float3 nor		: NORMAL;
		float4 color	: COLOR0;
		float4 uv0		: TEXCOORD0;
		float4 uv1		: TEXCOORD1;	
		float4 uv2		: TEXCOORD2;
		float4 uv3		: TEXCOORD3;
		float4 uv4		: TEXCOORD4;	
		float4 uv5		: TEXCOORD5;
		float4 uv6		: TEXCOORD6;
		float4 uv7		: TEXCOORD7;
	};

	struct Rasterizer{
		float4 pos		: POSITION;
		float4 color	: COLOR0;
		float4 uv0		: TEXCOORD0;
		float4 uv1		: TEXCOORD1;	
		float4 uv2		: TEXCOORD2;
		float4 uv3		: TEXCOORD3;
		float4 uv4		: TEXCOORD4;	
		float4 uv5		: TEXCOORD5;
		float4 uv6		: TEXCOORD6;
		float4 uv7		: TEXCOORD7;
	};

	/** Compute cubemap reflection vector for the cubemap mapping.
	 @param pos Vertex position in model space.
	 @param pos Vertex normal in model space.
	 @param camera Camera position in world space.
	 @param ref Return vector for the reflection vector.
	 */
	void CubeMap_Reflect(float3 pos, float3 nor, float3 camera,out float3 ref)
	{
		float3 n = mul(float4(nor,0), gphModelMatrix).xyz;   
		float3 p  = (mul(float4(pos,1), gphModelMatrix)).xyz;
		float3 e = normalize(p - camera);
		ref = reflect(e,n);
	}

	/**
	* Lambert diffuse reflectance model.
	*@param N Normalized surface normal in MV space.
	*@param L Normalized (L-V) vector in MV space.
	*@param diffuse Diffuse color.
	*@param color Output color.
	*/
	void Shade_Diffuse_Lambert(
		in float3 N, 
		in float3 L,
		in float3 diffuse,
		in out float3 color)
	{
		float dval = dot(N,L);

		if(dval>0)
			color += diffuse * dval;
	}

	/**
	* Blinn-Phong specular reflectance model.
	*@param E Normalized (E-V) vector in MV space.
	*@param N Normalized surface normal in MV space.
	*@param L Normalized (L-V) vector in MV space.
	*@param specular Specular color.
	*@param shininess Specular strenght.
	*@param color Output color.
	*/
	void Shade_Specular_Blinn(
		in float3 E,
		in float3 N, 
		in float3 L,
		in float3 specular,
		in float shininess,
		in out float3 color)
	{
		float dval = dot(N,L);

		if(dval>0){
			float3 H = normalize(L+E);
			color += specular * pow( clamp(dot(H, N), 0.0f,1.0f), shininess );
		}
	}

	/**
	* Phong specular reflectance model.
	*@param E Normalized (E-V) vector in MV space.
	*@param N Normalized surface normal in MV space.
	*@param L Normalized (L-V) vector in MV space.
	*@param specular Specular color.
	*@param shininess Specular strenght.
	*@param color Output color.
	*/
	void Shade_Specular_Phong(
		in float3 E,
		in float3 N, 
		in float3 L,
		in float3 specular,
		in float shininess,
		in out float3 color
	)
	{
		float dval = dot(N,L);

		if(dval>0){
			float3 R = reflect(-L, N);
			color += specular * pow( clamp(dot(R, E), 0.0f,1.0f), shininess );				   
		}
	}

	/**
	* Compute material reflectance model.
	*@param E Normalized (E-V) vector in MV space.
	*@param N Normalized surface normal in MV space.
	*@param L Normalized (L-V) vector in MV space.
	*@param diffuse Diffuse color.
	*@param specular Specular color.
	*@param shininess Specular strenght.
	*@param color Output color.
	*/
	void Compute_Material_LB(
		in float3 E,
		in float3 N, 
		in float3 L,
		in float3 diffuse,
		in float3 specular,
		in float shininess,
		in out float3 color
	)
	{
		Shade_Diffuse_Lambert(N,L,diffuse,color);
		Shade_Specular_Blinn(E,N,L,specular,shininess,color);
	}

	/**
	* Sample and compare the fragments coordinates in the light clipping coordinates 
	* space (taken from the current point of view) and test them with the ones stored 
	* in the shadow buffer(taken from the lamp "point of view"), using 4 samples to
	* smooth the shadow.
	* @param sbuffer Shadow buffer sampler.
	* @param fragment Clipping coordinates.
	* @param mapsize Shadow map size.
	* @param epsilon Shadow map threshold.
	* @return shadow blending factor.
	*/
	float Shadow_Bilinear_Filter(sampler2D sbuffer,float4 fcoords, float mapsize, float epsilon)
	{
		// Convert the coordinates from clipping space to texture space.
		float2 l_ShadowUVCoord = 0.5 * fcoords.xy / fcoords.w + float2( 0.5, 0.5 );
		l_ShadowUVCoord.y = 1.0f - l_ShadowUVCoord.y;
		
		if(l_ShadowUVCoord.y > 1.0f || l_ShadowUVCoord.y < 0.0f || l_ShadowUVCoord.x > 1.0f || l_ShadowUVCoord.x < 0.0f)
			return 0;

		// Fragment depth value in lamp clipping space.
		float l_FragmentDepth = fcoords.z / fcoords.w;	
		
		// Distance to the nearest texel.
		float l_Inv_Mapsize = 1.0f/mapsize;
		
		// Sample the shadow buffer and compare.
		float l_Samples[4];
		l_Samples[0] = ( tex2D( sbuffer, l_ShadowUVCoord ) + epsilon < l_FragmentDepth)? 0.0f: 1.0f; 
		l_Samples[1] = ( tex2D( sbuffer, l_ShadowUVCoord + float2(l_Inv_Mapsize, 0) ) + epsilon < l_FragmentDepth )? 0.0f: 1.0f;
		l_Samples[2] = ( tex2D( sbuffer, l_ShadowUVCoord + float2(0, l_Inv_Mapsize) ) + epsilon < l_FragmentDepth )? 0.0f: 1.0f;
		l_Samples[3] = ( tex2D( sbuffer, l_ShadowUVCoord + float2(l_Inv_Mapsize,l_Inv_Mapsize) ) + epsilon < l_FragmentDepth )? 0.0f: 1.0f;
		
		// Get the decimal part to interpolate correcly the samples.
		float2 l_Lerps = frac( mapsize * l_ShadowUVCoord );	
		
		// Bilinear interpolation.
		return lerp(	lerp( l_Samples[0], l_Samples[1], l_Lerps.x ),
						lerp( l_Samples[2], l_Samples[3], l_Lerps.x ), l_Lerps.y );
	}

	float3x4 RotateY(float angle){
		return float3x4(float4(cos(angle),0,-sin(angle),0),float4(0,1,0,0),float4(sin(angle),0,cos(angle),0));
	}

	float3x4 RotateX(float angle){
		return float3x4(float4(1,0,0,0),float4(0,cos(angle),sin(angle),0),float4(0,-sin(angle),cos(angle),0));
	}

	float3x4 RotateZ(float angle){
		return float3x4(float4(cos(angle),sin(angle),0,0),float4(-sin(angle),cos(angle),0,0),float4(0,0,1,0));
	}

);


_GRAPHICS_END

#endif // #ifndef NCK_SHADER_RESOURCES_DX9_H
