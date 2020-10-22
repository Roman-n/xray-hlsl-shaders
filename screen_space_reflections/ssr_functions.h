/*
	Screen space reflections shader
	- Cheap raymarching
	- Sky intersection (You can lerp cubemap reflections with SSR output :))
	- Half-resolution depth buffer support
	
	Author: ForHaxed, BD194, Xerxes1138, LVutner.
	References:
	https://habr.com/ru/post/244367/
	https://github.com/Xerxes1138/UnitySSR/
	https://www.amk-team.ru/forum/topic/14078-sslr
	
	License: Fork this code, improve it - share with others ;)	
*/

#ifndef SSR_FUNCTIONS_H
#define SSR_FUNCTIONS_H

#include "common.h"

/*///////////////////////////////////////
	SSR quality tokens
///////////////////////////////////////*/
#if !defined(SSR_QUALITY) || (SSR_QUALITY <= 1)
	#define SSR_SAMPLES int(8) // Low
	#define SSR_DISTANCE float(150.0)	
#elif SSR_QUALITY==2
	#define SSR_SAMPLES int(12) // Medium
	#define SSR_DISTANCE float(200.0)
#elif SSR_QUALITY==3
	#define SSR_SAMPLES int(16) // High
	#define SSR_DISTANCE float(300.0)	
#endif

/*///////////////////////////////////////
	SSR Settings
///////////////////////////////////////*/
#define SSR_HALF_DEPTH		//Enables half-res depth buffer support
#define SSR_EDGE_ATTENUATION float(0.09) //Edge attenuation size

/*///////////////////////////////////////
	Misc functions
///////////////////////////////////////*/
float RayAttenBorder(float2 pos, float value)
{	//Ray border attenuation
	float borderDist = min(1.0 - max(pos.x, pos.y), min(pos.x, pos.y));
	return saturate(borderDist > value ? 1.0 : borderDist / value);
}

float sample_depth(float2 texcoord)
{	//Samples depth buffer
	#ifndef SSR_HALF_DEPTH	
		#ifndef USE_MSAA
			float depth = s_position.SampleLevel(smp_nofilter, refl_tc.xy, 0).z;
		#else
			float depth = s_position.Load(int3(refl_tc.xy * pos_decompression_params2.xy,0),0).z;
		#endif
	#else
			float depth = s_half_depth.SampleLevel(smp_nofilter, refl_tc.xy, 0).x;
	#endif 
	
	return depth;
}


float4 proj_to_screen(float4 proj)
{	//Transforms projection space into screen space
	float4 screen = proj;
	screen.x = (proj.x + proj.w);
	screen.y = (proj.w - proj.y);
	screen.xy *= 0.5;
	return screen;
}

float is_sky(float depth)
{	//Checks for sky depth
	return step(abs(depth - 10000.f), 0.001);
}

#endif