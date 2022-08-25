#version 450 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;

out vec3 vertColor;

uniform mat4 cameraMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform int colorMode = 0;

void main() {	
	gl_PointSize = 3.0f;
	gl_Position = projectionMatrix * cameraMatrix * modelMatrix * vec4(aPosition.x, aPosition.y, aPosition.z, 1.0);
	vertColor = aColor;
}