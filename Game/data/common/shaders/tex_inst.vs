
uniform mat4 uViewProjection;

attribute vec3 aPosition;
attribute vec2 aUV;
attribute mat4 aWorldMatrix;

varying vec2 vTexCoord;

void main()
{
	gl_Position = uViewProjection * (aWorldMatrix * vec4(aPosition, 1.0));
	vTexCoord = aUV;
}
