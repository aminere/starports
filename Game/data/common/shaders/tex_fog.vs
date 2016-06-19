
uniform mat4 uWorldMatrix;
uniform mat4 uViewProjection;
uniform vec4 uAmbientColor;

attribute vec3 aPosition;
attribute vec2 aUV;
attribute vec4 aColor;

varying vec2 vTexCoord;
varying vec3 vVertPos;
varying vec4 vColor;

void main()
{
	vec4 vertPos = uWorldMatrix * vec4(aPosition, 1.0);
	gl_Position = uViewProjection * vertPos;
	vTexCoord = aUV;
	vVertPos = vertPos.xyz;
	vColor = aColor * uAmbientColor;
}
