
uniform mat4 uWorldViewProjection;

attribute vec3 aPosition;
attribute vec2 aUV;

varying vec2 vUV;

void main()
{	
	gl_Position = uWorldViewProjection * vec4(aPosition, 1.0);	
	vUV = aUV;
}
