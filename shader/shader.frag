#version 330 core

in vec3     Color;
in vec2     TexCoord;

out vec4    FragColor;

uniform sampler2D   texture_id;
uniform float       foo;

void    main()
{
	FragColor = mix(texture(texture_id, TexCoord), vec4(Color, 1.0), foo * foo);
}