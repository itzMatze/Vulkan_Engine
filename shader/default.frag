#version 460

layout(location = 0) in vec3 frag_normal;
layout(location = 1) in vec3 frag_color;
layout(location = 2) in vec2 frag_tex;

layout(location = 0) out vec4 out_color;

layout(binding = 1) uniform sampler2D tex_sampler;

void main()
{
    out_color = max(0.01, dot(vec3(1.0, 1.0, -1.0), frag_normal)) * texture(tex_sampler, frag_tex);
}