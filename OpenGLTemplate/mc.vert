uniform mat4 u_BoneTransforms[100];
uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
} matrices;

in vec3 position;
in vec4 boneIDs;
in vec4 weights;

void main()
{
    mat4 boneTransform =
        weights.x * u_BoneTransforms[int(boneIDs.x)] +
        weights.y * u_BoneTransforms[int(boneIDs.y)] +
        weights.z * u_BoneTransforms[int(boneIDs.z)] +
        weights.w * u_BoneTransforms[int(boneIDs.w)];

    gl_Position = matrices.projMatrix * matrices.modelViewMatrix * boneTransform * vec4(position,1.0);
}