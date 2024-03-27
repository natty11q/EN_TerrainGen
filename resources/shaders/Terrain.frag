#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Imports the current position from the Vertex Shader
in vec3 crntPos;
// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the color from the Vertex Shader
in vec3 color;
// // Imports the texture coordinates from the Vertex Shader
// in vec2 texCoord;



// Gets the Texture Units from the main function
uniform sampler2D diffuse0;
uniform sampler2D specular0;
// Gets the color of the light from the main function
uniform vec4 lightColor;
// Gets the position of the light from the main function
uniform vec3 lightPos;
// Gets the position of the camera from the main function
uniform vec3 camPos;

uniform float heightscale;
uniform float translate;


vec4 pointLight()
{	
	// used in two variables so I calculate it here to not have to do it twice
	vec3 lightVec = lightPos - crntPos;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 3.0;
	float b = 0.7;
	float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return vec4(color * (diffuse * inten + ambient),1.0f);
}

vec4 DirectLight()
{
	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - crntPos);
	float diffuse = max(abs(dot(normal, lightDirection)), 0.3f);

	// specular lighting
	float specularLight = 0.90f;
	vec3  viewDirection = normalize(camPos - crntPos);
	vec3  reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 2);
	float specular   = specAmount * specularLight;
	vec3 SpecColour = vec3(1.0f);

	return vec4((vec3(color * (diffuse + ambient)) + ( SpecColour * ( (diffuse + ambient) * specular ) ) ), 1.0f);
}

vec4 spotLight()
{
	// controls how big the area that is lit up is
	float outerCone = 0.90f;
	float innerCone = 0.95f;

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	return vec4(color * (diffuse * inten + ambient),1.0f);
}




float near = 0.1f;
float far = 100.0f;

float linearizeDepth(float depth)
{
	return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

float logisticDepth(float depth, float steepness, float offset)
{
	float zVal = linearizeDepth(depth);
	return (1 / (1 + exp(-steepness * (zVal - offset))));
}

void main()
{
	// outputs final color
	float depth = logisticDepth(gl_FragCoord.z , 0.1f , 0.1f);
	// FragColor = DirectLight() * lightColor * (1.0f - depth) + vec4(depth * vec3(0.85f, 0.85f, 0.90f), 1.0f);
	FragColor = DirectLight() * lightColor;
	// FragColor = vec4((crntPos.y + translate) / heightscale);
    
    // float angleValue   = dot(normalize(Normal),vec3(0,1,0));
    // float colourValue  = max( crntPos.y  / (heightscale*1.1f) , 1.0f);
    // FragColor = vec4(  vec3( (colourValue  * ( 1 - angleValue ) ) + translate) * color  ,1.0f ) /  1.5f;   /*+ vec4( (1.0f * abs(crntPos.x)) / 800.0f ,0.0f,(1.0f * abs(crntPos.z)) / 800.0f,0.0f);*/
    // vec3 normNorm = normalize(Normal);
	// FragColor = vec4(normNorm,1.0f) / 1.2f;  /*+ vec4( (1.0f * abs(crntPos.x)) / 800.0f ,0.0f,(1.0f * abs(crntPos.z)) / 800.0f,0.0f);*/
	// FragColor = direcLight();
}



//ask a mourition
//richard dawkings against all ods quote
