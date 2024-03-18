#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D texture1;
uniform vec4 color;
void main() {
    vec4 temp = texture(texture1, TexCoord);
    FragColor = vec4(color.xyz, temp.a);
}
