#version 460 core

in vec4     vertexColor;
in vec2     texCoord;
out vec4    fragColor;

uniform sampler2D   tex;

void main() {
    vec3 pixel = texture(tex, texCoord).rgb;
    fragColor = vec4(pixel, 0.0);
}