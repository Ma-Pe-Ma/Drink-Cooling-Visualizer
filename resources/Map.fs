	
out vec4 FragColor;
in vec3 vertColor;

void main() {
	FragColor = vec4(vertColor.x, vertColor.y, vertColor.z, 1.0f);
}