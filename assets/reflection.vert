#version 330 core

uniform float u_Time;
uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_LightMatrix;
uniform mat4 u_ReflectionMatrixLocal; // Transform any point on it's reflection, in mirror coord system
uniform mat4 u_ReflectionMatrix;
uniform sampler2D u_ShadowMap;

layout (location = 0) in vec4 in_Pos;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_UV;

out FS {
    float reflectionDepth; // Depth into the mirror. If negative, the object comes from behind the mirror.
    vec4 pos;
    vec3 nor;
    vec2 uv;
} fs;

uniform float u_Outline;

void main()
{
    vec4 worldNor = vec4(in_Normal, 0);
    worldNor = normalize(u_ModelMatrix * worldNor);

    vec4 worldPos = in_Pos;
    worldPos = u_ModelMatrix * worldPos;

    gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_ReflectionMatrix * worldPos;

    fs.pos = worldPos;
    fs.uv = in_UV;
    fs.nor = vec3(worldNor);
    fs.reflectionDepth = -(u_ReflectionMatrixLocal * worldPos).z;
}