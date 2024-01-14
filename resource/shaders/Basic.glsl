#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord;
uniform mat4 u_MVP;


void main()
{
    gl_Position = u_MVP * position;
    v_TexCoord = texCoord;
}


#shader fragment
#version 330 core
in vec2 v_TexCoord;
layout(location = 0) out vec4 color;
uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main()
{
    vec4 texColor = texture(u_Texture, v_TexCoord);
    color = mix(texColor, u_Color, 0.5); // 0.5 表示两者的比例，可以根据需要调整
}