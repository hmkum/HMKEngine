#version 330 core

in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skybox;

void main()
{   
	vec3 texColor = pow(texture(skybox, TexCoords).rgb, vec3(2.2f));
    FragColor = vec4(texColor, 1.0f);
}
