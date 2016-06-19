
uniform mat4 uWorldMatrix;
uniform mat4 uViewProjection;
uniform mat4 uTextureMatrix;

attribute vec3 aPosition;
attribute vec2 aUV;

varying vec2 vTexCoord;
varying vec3 vVertPos;

void main()
{	
	vec4 vertPos = uWorldMatrix * vec4(aPosition, 1.0);
	gl_Position = uViewProjection * vertPos;	
	vTexCoord = (uTextureMatrix * vec4(aUV, 1.0, 1.0)).st;
	vVertPos = vertPos.xyz;
}
