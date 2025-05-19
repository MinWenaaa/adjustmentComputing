#version 330 core
flat in int color;
out vec4 FragColor;
void main()
{
    switch(color) {
	case 0:
		FragColor = vec4(0.73, 0.78, 0.84, 1.0);
		break;
	case 1:
		FragColor = vec4(0.16, 0.54, 0.87, 1.0);
		break;
	default:
		FragColor = vec4(0.6, 0.6, 0.6, 1.0);
		break;
	}
}