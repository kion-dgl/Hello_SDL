attribute vec3 coord3d;
attribute vec2 texcoord;
uniform mat4 perspective, lookat, mvp;
varying vec2 f_texcoord;

void main(void) {

	gl_Position = perspective * lookat * mvp * vec4(coord3d, 1.0);
	f_texcoord = texcoord;

}
