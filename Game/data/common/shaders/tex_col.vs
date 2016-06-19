
uniform mat4 uWorldViewProjection;
uniform vec4 uAmbientColor;
uniform mat4 uTextureMatrix;

attribute vec3 aPosition;
attribute vec2 aUV;
attribute vec4 aColor;

varying vec4 vColor;
varying vec2 vTexCoord;

void main()
{	
	gl_Position = uWorldViewProjection * vec4(aPosition, 1.0);
	vTexCoord = (uTextureMatrix * vec4(aUV, 1.0, 1.0)).st;	
	vColor = aColor * uAmbientColor;
}
