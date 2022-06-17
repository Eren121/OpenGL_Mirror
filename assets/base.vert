#version 330 core

uniform float u_Time;
uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_LightMatrix;
uniform sampler2D u_ShadowMap;

layout (location = 0) in vec4 in_Pos;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_UV;

out FS {
    vec4 pos;
    vec3 nor;
    vec2 uv;
} fs;

void main()
{
    vec4 worldNor = vec4(in_Normal, 0);
    worldNor = normalize(u_ModelMatrix * worldNor);

    vec4 worldPos = in_Pos;
    worldPos = u_ModelMatrix * worldPos;

    gl_Position = u_ProjectionMatrix * u_ViewMatrix * worldPos;

    fs.pos = worldPos;
    fs.uv = in_UV;
    fs.nor = vec3(worldNor);
}