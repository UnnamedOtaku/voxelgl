#version 400 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

vec4 lightColor = vec4 (1.f, 1.f, 1.f, 1.f);

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform vec3 camPos;

vec4 pointLight(vec3 lightPos)
{   
    // used in two variables so I calculate it here to not have to do it twice
    vec3 lightVec = lightPos - FragPos;

    // intensity of light with respect to distance
    float dist = length(lightVec);
    float a = 0.0032;
    float b = 0.07;
    float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

    // ambient lighting
    float ambient = 0.01f;

    // diffuse lighting
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(lightVec);
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    // specular lighting
    float specularLight = 0.50f;
    vec3 viewDirection = normalize(camPos - FragPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
    float specular = specAmount * specularLight;

    return (1.f * (diffuse * inten + ambient) + 1.f * specular * inten) * lightColor;
}

vec4 direcLight(vec3 lightPos)
{   
    // used in two variables so I calculate it here to not have to do it twice
    vec3 lightVec = lightPos - FragPos;

    // intensity of light with respect to distance
    float dist = length(lightVec);
    float a = 0.0000032;
    float b = 0.00007;
    float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

    // ambient lighting
    float ambient = 0.20f;

    // diffuse lighting
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 1.0f));
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    // specular lighting
    float specularLight = 0.50f;
    vec3 viewDirection = normalize(camPos - FragPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
    float specular = specAmount * specularLight;

    return (texture(tex0, TexCoord) * (diffuse * inten + ambient) + texture(tex1, TexCoord).r * specular * inten) * lightColor;
}

vec4 spotLight(vec3 lightPos)
{   
    // used in two variables so I calculate it here to not have to do it twice
    vec3 lightVec = lightPos - FragPos;

    // intensity of light with respect to distance
    float dist = 0;
    float a = 0.00032;
    float b = 0.007;
    float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

    // controls how big the area that is lit up is
    float outerCone = 0.90f;
    float innerCone = 0.95f;

    // ambient lighting
    float ambient = 0.20f;

    // diffuse lighting
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(lightPos - FragPos);
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    // specular lighting
    float specularLight = 0.50f;
    vec3 viewDirection = normalize(camPos - FragPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
    float specular = specAmount * specularLight;

    // calculates the intensity of the FragPos based on its angle to the center of the light cone
    float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
    inten += clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

    return (texture(tex0, TexCoord) * (diffuse * inten + ambient) + texture(tex1, TexCoord).r * specular * inten) * lightColor;
}

void main()
{
    // outputs final color
    FragColor = pointLight(vec3(1.5f, 1.5f, 1.5f));
}