/*
	Improved SSDO shader
	- Corrected sampling among hemisphere
	- Scalable occlusion radius
	- Half-resolution depth buffer support

	Author: KD87 (OGSE Team), LVutner
	License: Fork this code, improve it - share with others ;)
*/
#ifndef SSDO_FUNCTIONS_H
#define SSDO_FUNCTIONS_H

/*///////////////////////////////////////
	SSDO quality tokens
///////////////////////////////////////*/
#if !defined(SSAO_QUALITY) || (SSAO_QUALITY <= 1)
	#define SSDO_SAMPLE 8 // Low	
#elif SSAO_QUALITY==2
	#define SSDO_SAMPLE 16 // Medium
#elif SSAO_QUALITY==3
	#define SSDO_SAMPLE 32 // High
#endif

/*///////////////////////////////////////
	SSDO settings
///////////////////////////////////////*/
#define SSAO_DEPTH_THRESHOLD 1.0 //Depth threshold
#define SSDO_RADIUS_MIN  0.2 //Minimal radius 
#define SSDO_RADIUS_MAX 30.0 //Maximum radius
#define SSDO_RENDER_DIST 125.0 //Rendering distance
#define SSDO_JITTER_TILE 64.0 //Noise texture tiling
	
/*///////////////////////////////////////
	SSDO hemisphere points
///////////////////////////////////////*/
static const float3 hemisphere[32] =
{
	float3(-0.134, 0.044, -0.825),	float3(0.045, -0.431, -0.529),	float3(-0.537, 0.195, -0.371),
	float3(0.525, -0.397, 0.713),	float3(0.895, 0.302, 0.139),	float3(-0.613, -0.408, -0.141),
	float3(0.307, 0.822, 0.169),	float3(-0.819, 0.037, -0.388),	float3(0.376, 0.009, 0.193),
	float3(-0.006, -0.103, -0.035),	float3(0.098, 0.393, 0.019),	float3(0.542, -0.218, -0.593),
	float3(0.526, -0.183, 0.424),	float3(-0.529, -0.178, 0.684),	float3(0.066, -0.657, -0.570),
	float3(-0.214, 0.288, 0.188),	float3(-0.689, -0.222, -0.192),	float3(-0.008, -0.212, -0.721),
	float3(0.053, -0.863, 0.054),	float3(0.639, -0.558, 0.289),	float3(-0.255, 0.958, 0.099),
	float3(-0.488, 0.473, -0.381),	float3(-0.592, -0.332, 0.137),	float3(0.080, 0.756, -0.494),
	float3(-0.638, 0.319, 0.686),	float3(-0.663, 0.230, -0.634),	float3(0.235, -0.547, 0.664),
	float3(0.164, -0.710, 0.086),	float3(-0.009, 0.493, -0.038),	float3(-0.322, 0.147, -0.105),
	float3(-0.554, -0.725, 0.289),	float3(0.534, 0.157, -0.250),
};

/*///////////////////////////////////////
	Misc functions
///////////////////////////////////////*/
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
{	//Checks sky depth
	return step(depth, 0.001);
}
#endif