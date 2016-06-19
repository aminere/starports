
uniform mat4 uWorldViewProjection;

attribute vec3 aPosition;

void main()
{
	gl_Position = uWorldViewProjection * vec4(aPosition, 1.0);
}
