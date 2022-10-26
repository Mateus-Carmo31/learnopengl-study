#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform int mult_amount;
uniform float mix_amount;

void main()
{
    vec2 newCoord = vec2(1 - TexCoord.x, TexCoord.y); // Exercise 1
    newCoord *= mult_amount;
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, newCoord), mix_amount);
}
