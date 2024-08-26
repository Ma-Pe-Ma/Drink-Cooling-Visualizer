
out vec4 FragColor;
in vec3 vertColor;

uniform int colorMode;

void main() {
	if (colorMode == 0) {
		FragColor = vec4(vertColor.x, vertColor.y, vertColor.z, 1.0);
	}
	else if (colorMode == 1) {
		FragColor = vec4(0.0, 1.0f, 0.0f, 1.0);
	}
	else if (colorMode == 2) {
		FragColor = vec4(0.0, 0.0f, 2.0f, 1.0);
	}
}