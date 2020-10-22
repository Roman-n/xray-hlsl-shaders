/*
	Height based fog
	- Basic sun scattering (totally-not-physically-correct)
	
	Author: LVutner.	
	License: Fork this code, improve it - share with others ;)	
*/

#ifndef HEIGHT_FOG_H
#define HEIGHT_FOG_H

#include "common.h"

/*///////////////////////////////////////
	Fog Settings
///////////////////////////////////////*/
#define FOG_DENSITY 0.04 //Fog density
#define FOG_HEIGHT 0.5 //Base height of fog
#define FOG_SCATTERING_INT 0.8 //Scattering intensity
#define FOG_SCATTERING_EXP 4.0 //Scattering exponent

/*///////////////////////////////////////
	Fog shader
///////////////////////////////////////*/

//View space position
float4 compute_height_fog(float3 P_view)
{
	/*//////////////////////////////
		Density
	//////////////////////////////*/
		
    //Transform view space position into world space
    float3 P_world = mul(m_v2w, float4(P_view, 1.f)).xyz;
    
    //Get height factor and fog distance
    float height_factor = height - P_world.y;    
    float pos_distance = length(P_view.xyz) * height_factor;
	
    //Calculate fog density
    float fog = 1.0 - exp(-pos_distance * density);
	fog = saturate(fog);
	
	/*//////////////////////////////
		Scattering
	//////////////////////////////*/
	
	float3 v2point = normalize(P_world - eye_position);
	
	//Get scattering contribution
	float scattering = saturate(dot(-v2point, normalize(L_sun_dir_w)));
	
	//Pow pow pow
	scattering = saturate(pow(scattering, FOG_SCATTERING_EXP));

	//Set intensity of scattering
	scattering *= FOG_SCATTERING_INT;
	
	//Mix fog colour with sun
	float3 color = lerp(fog_color.xyz, L_sun_color.xyz, scattering);

    //Output
    return float4(color, fog);
}

#endif //HEIGHT_FOG_H