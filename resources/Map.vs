
layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec3 aColor;

out vec3 vertColor;

void main() {
	gl_Position = vec4(aPosition.x, aPosition.y, 0.0f, 1.0f);
	vertColor = aColor;
}