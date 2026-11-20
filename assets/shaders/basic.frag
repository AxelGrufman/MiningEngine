#version 330 core

out vec4 FragColor;

in vec3 vNormal;
in vec2 vUV;

uniform vec3 uColor;
uniform vec3 uLightDirection;
uniform sampler2D uTexture;
uniform bool uUseTexture;

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 lightDirection = normalize(uLightDirection);
    float ambient = 0.2;
    
    float diffuse = max(dot(normal, lightDirection), 0.0);
    float brightness = ambient + diffuse * 0.8;
    
    vec3 baseColor = uColor;
    if (uUseTexture)
    {
        baseColor = texture(uTexture, vUV).rgb;
    }
   
    vec3 finalColor = baseColor * brightness;

    FragColor = vec4(finalColor, 1.0);
}
