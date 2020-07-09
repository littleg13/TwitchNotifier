#version 410 core

// phong.fsh - a fragment shader that implements a Phong Lighting model.
const int MAX_NUM_LIGHTS = 3;

in PVA
{
	vec3 ecPosition;
	vec3 ecUnitNormal;
	vec2 texCoords;
} pvaIn;

// For lighing model:
uniform mat4 ec_lds; // so projection type and hence vHat can be determined

// Phong material properties (RGB reflectances);
uniform vec3 ka = vec3(0.8, 0.0, 0.0); // default: darkish red
uniform vec3 kd = vec3(0.8, 0.0, 0.0); // default: darkish red
uniform vec3 ks = vec3(0.8, 0.0, 0.0); // default: darkish red
uniform float m = 1;
uniform float alpha = 1.0;

uniform int actualNumLights;
uniform vec4 ecLightPosition[MAX_NUM_LIGHTS];
uniform vec3 lightStrength[MAX_NUM_LIGHTS];
uniform int lightEnabled[MAX_NUM_LIGHTS];
uniform vec3 globalAmbient = vec3(0.15, 0.15, 0.15);

uniform bool useTexture = false;
uniform bool useTextureAlpha = false;
uniform sampler2D textureImage;

uniform int sceneHasTranslucentObjects = 0;
uniform int drawingOpaqueObjects = 1;

// output color from the lighting model:
out vec4 fragmentColor;

float attenuation(vec3 Q, vec4 Li){
	float c0 = 1.0;
	float c2 = 0.0005;
	float d = length(Q - Li.xyz) * Li.w;
	d = ec_lds[0][0] * d;
	return 1/(c0 + c2 * d * d);
}

vec4 evaluateLightingModel(vec3 textureDiffuse, float a)
{
	vec3 tempKa = ka;
	vec3 tempKd = textureDiffuse;
	vec3 tempKs = ks;
	float tempM = m;
	if(pvaIn.snow > 0.0){
		tempKa = vec3(0.45, 0.45, 0.5);
		tempKd = vec3(0.55, 0.55, 0.6);
		tempKs = vec3(0.8, 0.8, 1);
		tempM = 1;
	}
		
	vec3 toViewer; 
	if(ec_lds[3][3] == 0){
		toViewer = normalize(-pvaIn.ecPosition);
	}
	else{
		toViewer = normalize(vec3(-ec_lds[2][0]/ec_lds[0][0], -ec_lds[2][1]/ec_lds[1][1], 1));
	}
	vec3 correctedNormal = pvaIn.ecUnitNormal;
	if(dot(pvaIn.ecUnitNormal, toViewer) < 0.0)
		correctedNormal = -correctedNormal;
	vec3 lightValue = vec3(0, 0, 0);
	for(int i=0;i<actualNumLights;i++){
		vec3 liHat = normalize(ecLightPosition[i].xyz - (pvaIn.ecPosition * ecLightPosition[i].w));
		if(dot(liHat, correctedNormal) > 0.0){
			vec3 rHat = normalize(2 * (dot(liHat, correctedNormal) * correctedNormal) - liHat);
			vec3 specular = tempKs * pow(max(0.0, dot(rHat, toViewer)), tempM);
			vec3 diffuse = tempKd * dot(liHat, correctedNormal);
			lightValue += attenuation(pvaIn.ecPosition, ecLightPosition[i]) * lightStrength[i] * (diffuse + specular) * lightEnabled[i];
		}
	}
	return vec4(tempKa * globalAmbient + lightValue, a);
}

void main ()
{
	vec4 diffuse = vec4(kd, alpha);
	if(useTexture){
		diffuse = texture(textureImage, pvaIn.texCoords);
		if(useTextureAlpha == false)
			diffuse.a = alpha;
	}
	fragmentColor = calculateTranslucency(evaluateLightingModel(diffuse.rgb, diffuse.a));
}
