
uniform mat4 uWorldMatrix;
uniform mat4 uViewProjection;

uniform mat4 uLightVPs;//[4];
//uniform int uLightCount;

attribute vec3 aPosition;
attribute vec2 aUV;
attribute vec3 aNormal;
attribute vec4 aTangent; // w component used to determine the direction of the Binormal ( Binormal = TxN * w )

varying vec3 vVertPos;
varying vec2 vTexCoord;
varying vec3 vVertNorm;
varying vec3 vVertTangent;
varying vec3 vVertBinormal;
varying vec4 vLightSpacePositions;//[4];

void main()
{
	vec4 vertPos = uWorldMatrix * vec4(aPosition, 1.0);
	gl_Position = uViewProjection * vertPos;
		
	//for(int i=0; i<uLightCount; i++)
	//{
		vLightSpacePositions = uLightVPs * vertPos;
	//}
		
	// convert to normalized device coords (NDC)
	vLightSpacePositions.xyz = vLightSpacePositions.xyz / vLightSpacePositions.w;
	// convert xy to [0 - 1] sampler space and z to [0 - 1] depth range
	vLightSpacePositions.xyz = vLightSpacePositions.xyz*0.5 + 0.5;		
		
	vTexCoord = aUV;
	vVertPos = vertPos.xyz;
	
	vVertNorm = normalize(uWorldMatrix * vec4(aNormal, 0.0)).xyz;
	vVertTangent = normalize(uWorldMatrix * vec4(aTangent.xyz, 0.0)).xyz;
	vec3 aBinormal = cross(aNormal, aTangent.xyz) * aTangent.w;
	vVertBinormal = normalize(uWorldMatrix * vec4(aBinormal, 0.0)).xyz;	
}
