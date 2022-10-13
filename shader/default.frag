#version 460

layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec2 frag_tex;

layout(location = 0) out vec4 out_color;

layout(binding = 1) uniform sampler2D tex_sampler;

void main()
{
    out_color = vec4(texture(tex_sampler, frag_tex).rgb * 0.5 + frag_color * 0.5, 1.0);
}