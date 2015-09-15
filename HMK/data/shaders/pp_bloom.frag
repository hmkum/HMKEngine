#version 420 core

out vec4 FinalColor;
in vec2 TexCoords;

uniform float uBloomIntensity;

layout(binding = 0) uniform sampler2D PostProcessBuffer;
layout(binding = 1) uniform sampler2D BlurredBuffer;

void main()
{
	vec3 color1 = texture(PostProcessBuffer, TexCoords).rgb;
	vec3 color2 = texture(BlurredBuffer, TexCoords).rgb * uBloomIntensity;
    
	FinalColor = vec4(color1 + color2, 1.0f);
}

/*
	vec4 sum = vec4(0);
    vec2 texcoord = TexCoords.xy;
    int j;
    int i;
    
    for( i= -4 ;i < 4; i++)
    {
         for (j = -3; j < 3; j++)
         {
             sum += texture(PostProcessBuffer, texcoord + vec2(j, i)*0.004) * 0.25;
         }
    }
    
	if (texture(PostProcessBuffer, texcoord).r < 0.3)
    {
       FinalColor = sum*sum*0.012 + texture(PostProcessBuffer, texcoord);
    }
    else
    {
        if (texture(PostProcessBuffer, texcoord).r < 0.6)
        {
            FinalColor = sum*sum*0.009 + texture(PostProcessBuffer, texcoord);
        }
        else
        {
            FinalColor = sum*sum*0.0075 + texture(PostProcessBuffer, texcoord);
        }
    }*/