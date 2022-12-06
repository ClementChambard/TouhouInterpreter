#version 130

in vec3 vertexPosition;
in vec4 vertexColor;
in vec2 vertexTexCoords;

out vec4 fragmentColor;
out vec3 fragmentPosition;
out vec2 fragmentTexCoords;

uniform mat4 NSCamera;

void main()
{
    gl_Position = (NSCamera * vec4(vertexPosition, 1.0));
    //gl_Position.w = 1.f;

    fragmentColor = vertexColor;
    fragmentPosition = vertexPosition.xyz;
    fragmentTexCoords = vertexTexCoords;
}
