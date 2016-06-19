
uniform mat4 uWorldMatrix;
uniform mat4 uViewProjection;

attribute vec3 aPosition;
attribute vec2 aUV;
attribute vec3 aNormal;

varying vec3 vVertPos;
varying vec3 vVertNorm;
varying vec2 vTexCoord;

void main()
{
	vec4 vertPos = uWorldMatrix * vec4(aPosition, 1.0);
	gl_Position = uViewProjection * vertPos;	
	
	vVertNorm = normalize(uWorldMatrix * vec4(aNormal, 0.0)).xyz;
	vTexCoord = aUV;
	vVertPos = vertPos.xyz;	
}
