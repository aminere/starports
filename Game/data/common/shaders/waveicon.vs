
uniform mat4 uWorldViewProjection;

attribute vec3 aPosition;
attribute vec2 aUV;

varying vec2 vPosition;
varying vec2 vTexCoord;

void main()
{
	vec4 pos = uWorldViewProjection * vec4(aPosition, 1.0);
	gl_Position = pos;
	vPosition = pos.xy;
	vTexCoord = aUV;
}
