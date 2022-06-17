#version 330 core

// Generic
uniform float u_Time;

// Matrices
uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;

// Material
uniform sampler2D u_Texture; // Texture for ambiant AND diffuse color
uniform float u_AmbientIntensity;
uniform float u_DiffuseIntensity;
uniform vec4 u_DiffuseColor;
uniform float u_SpecularExponent;
uniform vec4 u_SpecularColor;
uniform float u_Opacity;

// Lighting
uniform vec3 u_LightDirection;

in FS {
    float reflectionDepth; // Depth into the mirror. If negative, the object comes from behind the mirror.
    vec4 pos;
    vec3 nor;
    vec2 uv;
} fs;

out vec4 out_Color;

vec3 getCameraPos()
{
    // In view space, origin is position of the camera
    vec4 origin = vec4(vec3(0), 1);

    return vec3(inverse(u_ViewMatrix) * origin);
}

float getSpecularIntensity()
{
    vec3 eyes = normalize(fs.pos.xyz - getCameraPos());
    vec3 r = reflect(u_LightDirection, fs.nor);

    float specular = dot(r, -eyes);
    if(specular < 0)
    {
        specular = 0;
    }

    specular = pow(specular, u_SpecularExponent);

    return specular;
}


void main()
{
    float ambiant = u_AmbientIntensity;
    vec4 ambiantColor = texture(u_Texture, fs.uv) * u_DiffuseColor;

    float diffuse = dot(-u_LightDirection, fs.nor);
    if(diffuse < 0)
    {
        // The ray comes from back,
        // We don't want to reduce the luminosity with a negative amount, just don't illuminate with diffuse
        diffuse = 0;
    }

    diffuse *= u_DiffuseIntensity;
    vec4 diffuseColor = texture(u_Texture, fs.uv) * u_DiffuseColor;

    float specular = getSpecularIntensity();

    out_Color = (ambiant * ambiantColor
    + diffuse * diffuseColor
    + specular * u_SpecularColor);

    out_Color.a = u_Opacity;
    // Discard if the object is behind the mirror (negative depth)
    if(fs.reflectionDepth < 0.0)
    {
        discard;
    }
}