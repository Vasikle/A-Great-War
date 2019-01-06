#ifdef TEXMORPH
	float4 morphK : packoffset(pscMorphK);
#endif

#ifdef TFACTOR
	float4 tFactor	: packoffset(pscTFactor);
#endif

#ifdef ENVMAP
 #ifdef WATER
	float4 env_amount	: packoffset(pscEnvmapAmount);
 #elif !defined(MASK)
		float4 env_amount : packoffset(pscEnvmapAmount);
 #endif
#endif //ENVMAP
#ifdef SPEC
	float4 lightSpecular0 : packoffset(pscLightSpecular(0));
 #ifdef LAYER1
	float4 lightSpecular1 : packoffset(pscLightSpecular(1));
 #endif
#endif
#ifdef WATER
	float4	waterDiffuse : packoffset(pscMaterialDiffuse);
#endif
