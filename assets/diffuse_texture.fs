#version 330 core

in vec2 vTexCoords;
in vec4 vColor;

out vec4 final;

uniform sampler2D uTexture;

void main() {
  final = texture(uTexture, vTexCoords);
}
