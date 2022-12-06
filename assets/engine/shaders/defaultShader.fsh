#version 130

in vec4 fragmentColor;
in vec3 fragmentPosition;
in vec2 fragmentTexCoords;

out vec4 color;

uniform sampler2D NSTexture;

void main() {

    vec4 col = texture(NSTexture, fragmentTexCoords);

    color = col * fragmentColor;
}
