#version 130

in vec4 fragmentColor;
in vec3 fragmentPosition;
in vec2 fragmentTexCoords;

out vec4 color;

uniform sampler2D NSTexture;

uniform vec4 FogColor;
uniform vec3 EyeLocation;
uniform float FogMax;
uniform float FogMin;

float getFogFactor(float d)
{
    if (d>=FogMax) return 1.f;
    if (d<=FogMin) return 0.f;
    return 1.f-(FogMax-d)/(FogMax-FogMin);
}

void main() {
    float d = distance(EyeLocation,fragmentPosition);
    vec4 col = texture(NSTexture, fragmentTexCoords);
    float alpha = getFogFactor(d);
    col = col * fragmentColor;
    color = mix(col, FogColor, alpha);
    color.a = min(color.a,col.a);
}
