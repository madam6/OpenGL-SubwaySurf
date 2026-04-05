layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in ivec4 boneIDs;
layout (location = 4) in vec4 weights;

out vec2 vTexCoord;
out vec3 vNormal;
out vec3 vPosition;

uniform mat4 u_BoneTransforms[200];
uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
} matrices;

void main()
{
    mat4 boneTransform =
        weights.x * u_BoneTransforms[boneIDs.x] +
        weights.y * u_BoneTransforms[boneIDs.y] +
        weights.z * u_BoneTransforms[boneIDs.z] +
        weights.w * u_BoneTransforms[boneIDs.w];


    vec4 localPosition = boneTransform * vec4(position, 1.0);

    gl_Position = matrices.projMatrix * matrices.modelViewMatrix * localPosition;

    vTexCoord = texCoord;

    mat3 boneNormalTransform = mat3(boneTransform);
    vNormal = normalize(matrices.normalMatrix * boneNormalTransform * normal);

    vPosition = vec3(matrices.modelViewMatrix * localPosition);
}