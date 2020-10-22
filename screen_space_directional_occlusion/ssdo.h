/*
	Improved SSDO shader
	- Corrected sampling among hemisphere
	- Scalable occlusion radius
	- Half-resolution depth buffer support
	
	Author: KD87 (OGSE Team), LVutner
	License: Fork this code, improve it - share with others ;)	
*/

#ifndef SSDO_H
#define SSDO_H

#include "common.h"
#include "ssdo_functions.h"

//View space position, view space normals, texcoord...
#ifndef SSAO_QUALITY
float calc_ssdo (float3 P, float3 N, float2 tc, float4 pos2d, uint iSample)
{
	return 1.0;
}

#else // SSAO_QUALITY

//Main SSDO shader
float calc_ssdo (float3 P, float3 N, float2 tc, float4 pos2d, uint iSample)
{
	//Get scalable radius
	float SSDO_RADIUS = smoothstep(SSDO_RADIUS_MIN, SSDO_RADIUS_MAX, length(P.xyz)); //Scalable "true" radius
		
	//Microoptimization :)
	if(P.z >= SSDO_RENDER_DIST)
		return 1.0;

	//Sample jitter texture
	float3 noise = jitter0.Sample(smp_jitter, tc * SSDO_JITTER_TILE).xyz;
	
	//Accumulate our AO.
	float occ = 0.0;	

	[unroll (SSDO_SAMPLE)]
	for (int i = 0; i < SSDO_SAMPLE; i++)
	{
		//Get reflection sample from hemisphere points and noise, later put normals for moar details
		float3 reflection_sample = reflect(hemisphere[i], noise) + N.xyz;
		
		float3 occ_pos_view = P.xyz + reflection_sample * SSDO_RADIUS;
		float4 occ_pos_screen = proj_to_screen(mul(m_P, float4(occ_pos_view, 1.0)));
		occ_pos_screen.xy /= occ_pos_screen.w;
		
		//Sample depth buffer
		float screen_occ = sample_depth(occ_pos_screen.xy);	

		//Return 0.0 if ray hits the sky
		screen_occ = lerp(screen_occ, 0.0f, is_sky(screen_occ));
			
		//Get occlusion factor
		float is_occluder = step(occ_pos_view.z, screen_occ);
		float occ_coeff = saturate(is_occluder + saturate(1.0 - screen_occ) + step(SSAO_DEPTH_THRESHOLD, abs(P.z-screen_occ)));
		occ += occ_coeff;
	}
	occ /= SSDO_SAMPLE;
	occ = saturate(occ);
	return occ;
}
#endif //SSAO_QUALITY
#endif //SSDO_H