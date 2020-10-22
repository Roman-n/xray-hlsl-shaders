/*
	Screen space reflections shader
	- Cheap raymarching
	- Sky intersection (You can lerp cubemap reflections with SSR output :))
	- Half-resolution depth buffer support
	
	Authors: ForHaxed, BD194, Xerxes1138, LVutner, KD87.
	References:
	https://habr.com/ru/post/244367/
	https://github.com/Xerxes1138/UnitySSR/
	https://www.amk-team.ru/forum/topic/14078-sslr
	
	License: Fork this code, improve it - share with others ;)	
*/

#ifndef SSR_H
#define SSR_H

#include "common.h"
#include "ssr_functions.h"

//World space position and normals
#ifndef SSR_QUALITY
float4 compute_ssr(float3 position, float3 normal)
{
	return float4(0.f,0.f,0.f,0.f);
}

#else

float4 compute_ssr(float3 position, float3 normal)
{
	//Initialize step size and error
	float step_size = 1.0f / float(SSR_SAMPLES);
	int is_hit = 1;
	
	//Initialize reflected TC
	float2 refl_tc = (0,0);	
	
	//Prepare ray direction and reflection vector
	float3 v2point = normalize(position - eye_position);
	float3 vreflect = reflect(v2point, normal);

	//Transform world space position into view space
	float depth = mul(m_V, float4(position, 1.f)).z;	
		
	//Main loop
	[unroll (SSR_SAMPLES)] 
	for(int i = 0; i < SSR_SAMPLES; i++)
	{
		//Prepare new position - world space
		float3 new_position = position + vreflect * step_size;

		//Convert new position into texture coordinates
		float4 proj_position = mul(m_VP, float4(new_position, 1.f));
		float4 p2ss = proj_to_screen(proj_position);
		refl_tc.xy = p2ss.xy /= p2ss.w;

		//Sample hit depth
		float hit_depth = sample_depth(refl_tc.xy);
		
		//Intersect sky from hit depth
		hit_depth = lerp(hit_depth, 0.f, is_sky(hit_depth));
		
		//Fix incorrect tracing, attenuate SSR at distance
		if((depth - hit_depth) > 0.001f || (hit_depth > SSR_DISTANCE))
			is_hit = 0;
		
		//Depth difference 
		step_size = length(hit_depth - depth);		
	}

	//Edge attenuation
	float edge = RayAttenBorder(refl_tc.xy, SSR_EDGE_ATTENUATION);
	
	//Sample image with reflected TC	
	float3 img = s_image.Load(int3(refl_tc.xy * pos_decompression_params2.xy,0),0);
	
	//Reflection.rgb, Hit.a
	return float4(img.xyz, is_hit*edge);
}
#endif //SSR quality
#endif //Header