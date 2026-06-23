#version 450

layout(push_constant) uniform PushConstants {
    mat4 mvp;
    int isWireframe;
} pc;

layout(location = 0) out vec3 fragColor;

const vec3 pos[8] = vec3[8](
	vec3(-1.0,-1.0, 1.0), vec3( 1.0,-1.0, 1.0), vec3( 1.0, 1.0, 1.0), vec3(-1.0, 1.0, 1.0),
	vec3(-1.0,-1.0,-1.0), vec3( 1.0,-1.0,-1.0), vec3( 1.0, 1.0,-1.0), vec3(-1.0, 1.0,-1.0)
);

const vec3 col[8] = vec3[8](
	vec3( 1.0, 0.0, 0.0), vec3( 0.0, 1.0, 0.0), vec3( 0.0, 0.0, 1.0), vec3( 1.0, 1.0, 0.0),
	vec3( 1.0, 1.0, 0.0), vec3( 0.0, 0.0, 1.0), vec3( 0.0, 1.0, 0.0), vec3( 1.0, 0.0, 0.0)
);

const uint indices[36] = uint[36](
	2, 1, 0, 0, 3, 2, // front  (+Z)
	6, 5, 1, 1, 2, 6, // right  (+X)
	5, 6, 7, 7, 4, 5, // back   (-Z)
	3, 0, 4, 4, 7, 3, // left   (-X)
	1, 5, 4, 4, 0, 1, // bottom (-Y)
	6, 2, 3, 3, 7, 6  // top    (+Y)
);

void main() {
	uint idx = indices[gl_VertexIndex];
	gl_Position = pc.mvp * vec4(pos[idx], 1.0);
	fragColor = col[idx];
}

