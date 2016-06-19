
uniform mat4 uWorldViewProjection;

attribute vec3 aPosition;
attribute vec2 aUV;

varying vec2 vScreenCoord;
varying vec2 vLocalCoord;

void main()
{	
	vec4 screenPos = uWorldViewProjection * vec4(aPosition, 1.0);
	gl_Position = screenPos;
	vScreenCoord = screenPos.xy / screenPos.w;
	vLocalCoord = aUV;
}
