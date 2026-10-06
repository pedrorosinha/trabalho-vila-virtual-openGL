#version 400

out vec4 FragColor;

in vec2 TexCoord;

uniform vec3 objectColor;

uniform sampler2D textura;
uniform bool usarTextura;

void main()
{
    if (usarTextura)
    {
        FragColor = texture(textura, TexCoord);
    }
    else
    {
        FragColor = vec4(objectColor, 1.0);
    }
}