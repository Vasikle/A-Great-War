#ifdef __cplusplus
#pragma once
namespace Render
{
	namespace Shader
	{
		struct eConstId
		{
			eConstId(word g, word i) : group(g), index(i) {}
			eConstId operator+(int i) const { return eConstId(group, index + i); }
			word group;
			word index;
		};
		struct eConstGroupDef
		{
			eConstGroupDef(word gid, word cnt) : group(gid), count(cnt) {}
			word group;
			word count;
		};
	}
}
#endif

//////////////////////////////////////////////////////
// ***************** VERTEX SHADER **************** //
//////////////////////////////////////////////////////
#ifdef __cplusplus
#define _VSC(g, i) Render::Shader::eConstId(g, i)
#define _VSG(g, i) Render::Shader::eConstGroupDef(g, i)
#else
#define _VSC(g, i) c[i]
#define _VSG(g, i) b##g
#endif

#define MAX_SKIN_BONE_COUNT 25
#define MAX_PARAMS_PER_LAYER 2
#define MAX_LIGHTS_PER_PASS 8
#define MAX_MTL_LAYERS 2

//***************************
// vertex shader constant groups
//***************************
#define vscgScene 			0
#define vscgPrimitive		1
#define vscgPrimitiveMisc	2
#define vscgSkin			3

//***************************
// per scene/layer (buffer 0)
//***************************
#define vscHazeParams		_VSC(vscgScene, 0)
#define vscHazeBound		_VSC(vscgScene, 1)
#define vscCameraOrigin		_VSC(vscgScene, 2)
#define vscViewProj			_VSC(vscgScene, 3)		//c3:c6
#define vscWorldToFogmap	_VSC(vscgScene, 7)		//c7:c10
#define vscShadowAttenuation	_VSC(vscgScene, 11)
#define vscShadowViewProjLight0	_VSC(vscgScene, 12)	//[4*NUM_SLICES]
#define vscWindParams		_VSC(vscgScene, 24)
#define vscOceanPlaneNormal	_VSC(vscgScene, 25)		
#define vscOceanPlaneWSPos	_VSC(vscgScene, 26)		
#define vscInverseViewProj	_VSC(vscgScene, 27)
#define vscUVRotationMatrix _VSC(vscgScene, 31)
#define vscOceanWaveParams	_VSC(vscgScene, 35)		//Per shader params: 
#define vscOceanWaveData	_VSC(vscgScene, 36)		//Array with wave specific data 1 (max 16) c36 - c51:
//---------------------------
#define vscSceneGroup		_VSG(vscgScene, 52)
//***************************

//***************************
// per primitive (buffer 1)
//***************************
//#define vscWorld0			_VSC(vscgPrimitive, 0)	//c0:c2
//#define vscWindParamsPrim	_VSC(vscgPrimitive, 3)
//---------------------------
#define vscPrimitiveGroup	_VSG(vscgPrimitive, 4)
//***************************

//***************************
// per primitive misc (buffer 2)
//***************************
#define vscTexXform			_VSC(vscgPrimitiveMisc, 0)	//c0:c1  	//@todo: setup only if not identity, provide shader flag/define
#define vscTexXformX		_VSC(vscgPrimitiveMisc, 0)
#define vscTexXformY		_VSC(vscgPrimitiveMisc, 1)
//---------------------------
#define vscPrimitiveMiscGroup	_VSG(vscgPrimitiveMisc, 2)
//***************************

//***************************
// mesh skinning (buffer 3)
//***************************
#define vscWorld0Skinned	_VSC(vscgSkin, 0)		//[3 * MAX_SKIN_BONE_COUNT]
//---------------------------
#define vscSkinGroup		_VSG(vscgSkin, 3 * MAX_SKIN_BONE_COUNT)
//***************************


//////////////////////////////////////////////////////
// ***************** PIXEL SHADER ***************** //
//////////////////////////////////////////////////////
#ifdef __cplusplus
#define _PSC(g, i) Render::Shader::eConstId(g, i)
#define _PSG(g, i) Render::Shader::eConstGroupDef(g, i)
#else
#define _PSC(g, i) c[i]
#define _PSG(g, i) b##g
#endif

//***************************
// pixel shader constant groups
//***************************
#define pscgScene 			0
#define pscgMaterial 		1
#define pscgDynamicLights	2
#define pscgMaterialFlags	3

//***************************
// per scene (buffer 0)
//***************************
#define pscFogAmount 		_PSC(pscgScene, 0)
#define pscFogBrightness	_PSC(pscgScene, 1)
#define pscHazeColor		_PSC(pscgScene, 2)
#define pscCameraOrigin		_PSC(pscgScene, 3)		
#define pscViewProj 		_PSC(pscgScene, 4)		//c4:c7
#define pscShadowmapParams	_PSC(pscgScene, 8)
#define pscShadowSliceDepth _PSC(pscgScene, 9)
#define pscGBCSat			_PSC(pscgScene, 10)		// gamma, brightness, contrast, saturation
#define pscTone				_PSC(pscgScene, 11)
#define pscFGRenRes			_PSC(pscgScene, 12)		//*FH: Vec3 Render resolution for film grain(width,height) and .y = timer
#define pscFGParameters		_PSC(pscgScene, 13)		//*FH: Vec3: x = grainAmount, y = grainParticleSize, z = lumAmount, w = colorAmount
#define pscLEParams			_PSC(pscgScene, 14)		//*FH: lenseffects.hlsl : Vec4: x = view factor, y = light scattering intensity, z = light scattering ray length, .w = aspect ratio // sky.hlsl: .x = draw sun/moon, .y = sun/moon radius, .z = is night?
#define pscLEParams2		_PSC(pscgScene, 15)		//*FH: lenseffects.hlsl : Vec2: x = brightening intensity, y = delta time
#define pscLEColor			_PSC(pscgScene, 16)		//*FH: Lens effects tone color: Vec3 color, sun/moon radius 
#define pscLELightPos		_PSC(pscgScene, 17)		//*FH: Vec4: x,y->screen coords of sun. bloom.hlsl : .z/.w -> low res half texel.x/y size // sky.hlsl z./.w -> Device Resolution width,height
#define pscInvViewProj 		_PSC(pscgScene, 18)		//*FH: 4x4 InverseMatrix of ViewProjection (c18:c21)
#define pscCSColorTone		_PSC(pscgScene, 22)		//*FH: A Vec3 describing the color tone used by color shading
#define pscDOFZPlanes		_PSC(pscgScene, 23)		//*FH: .x = nearBlurryPlaneZ, .y = nearSharpPlaneZ, .z = farSharpPlaneZ, .w = farBlurryPlaneZ
#define pscDOFParams		_PSC(pscgScene, 24)		//*FH: Used to hold different Data, dependent on DOF pass, which will get rendered
#define pscSSAOParams		_PSC(pscgScene, 25)		//*FH: tmp
#define pscSSAOParams2		_PSC(pscgScene, 26)		//*FH: tmp
#define pscInvView			_PSC(pscgScene, 27)		//*FH: c25:c28
#define pscZoomParams		_PSC(pscgScene, 31)		//*FH: Zoom specific data
//---------------------------
#define pscSceneGroup	_PSG(pscgScene, 32)		//*FH: modified (12 - 30)
//***************************

//***************************
// per material (buffer 1)
//***************************
#define pvscWorld0				_PSC(pscgMaterial, 0)	//c0:c2
#define pvscWindParamsPrim		_PSC(pscgMaterial, 3)

#define pscLightAmbient			_PSC(pscgMaterial, 4)
#define pscLightDiffuse			_PSC(pscgMaterial, 5)
#define pscLightDir 			_PSC(pscgMaterial, 6)
#define pscTFactor				_PSC(pscgMaterial, 7)
#define pscMorphK				_PSC(pscgMaterial, 8)
#define pscEnvmapAmount			_PSC(pscgMaterial, 9)
#define pscMaterialParams		_PSC(pscgMaterial, 10)
#define pscMaterialDiffuse		_PSC(pscgMaterial, 11)
#define pscParallaxParams(i)	_PSC(pscgMaterial, 12 + i * MAX_PARAMS_PER_LAYER)   // MAX_PARAMS_PER_LAYER * MAX_MTL_LAYERS
#define pscLightSpecular(i)		_PSC(pscgMaterial, 13 + i * MAX_PARAMS_PER_LAYER)
#define pscFadeawayParams		_PSC(pscgMaterial, 15)
#define pscTranslucencyParams	_PSC(pscgMaterial, 16)		//rgb -> color, a -> amount
#define pscWaterParams			_PSC(pscgMaterial, 17)		//*FH: x = pollutionFactor, y = maxWaterDepth, z = costalDepth, w = fogEnd
#define pscWaterParams2			_PSC(pscgMaterial, 18)		//*FH: x = reflectionAmount

//---------------------------
#define pscMaterialGroup	_PSG(pscgMaterial, 19)
//***************************

//***************************
// dynamic lights (buffer 2)
//***************************
#define pscDynamicLight0		_PSC(pscgDynamicLights, 0) // [4 * MAX_LIGHTS_PER_PASS]
//---------------------------
#define pscDynamicLightsGroup	_PSG(pscgDynamicLights, 4 * MAX_LIGHTS_PER_PASS)
//***************************

//***************************
// material flags (buffer 3)
//***************************
#define pscMaterialFlag0		_PSC(pscgMaterialFlags, 0)
#define pscDynamicLightOn0		_PSC(pscgMaterialFlags, 6)
//---------------------------
#define pscMaterialFlagsGroup	_PSG(pscgMaterialFlags, 14)
//***************************
