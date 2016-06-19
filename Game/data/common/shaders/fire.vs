
uniform mat4 uWorldViewProjection;
uniform vec4 uAmbientColor;
uniform mat4 uTextureMatrix;
uniform vec2 uScreenSize;
uniform vec2 uTexSize;

attribute vec3 aPosition;
attribute vec2 aUV;
attribute vec4 aColor;

varying vec4 vColor;
varying vec2 vTexCoord;
varying vec2 vScreenCoord;

void main()
{
	vec4 vVertPos = uWorldViewProjection * vec4(aPosition, 1.0);
	gl_Position = vVertPos;
	
	vec2 vScreenPos = vVertPos.xy / vVertPos.w;
	// convert xy to [0 - 1] sampler space
	vScreenPos = vScreenPos*0.5 + 0.5;
	vScreenPos.y = 1.0-vScreenPos.y; // make y go from top to bottom
	vScreenCoord = vScreenPos * (uScreenSize / uTexSize);
	vScreenCoord = (uTextureMatrix * vec4(vScreenCoord, 1.0, 1.0)).st;
	
	vTexCoord = aUV;
	vColor = aColor * uAmbientColor;
}
