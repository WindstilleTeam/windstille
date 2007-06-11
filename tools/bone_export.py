import Blender
import Blender.Armature

arms = Blender.Armature.Get()
print "####################################"
for arm in arms.values():
	print "Armature: %s" % arm.name
	for bone in arm.bones.values():
		print "  Bone: %s" % bone
		print "  BoneSpace:"
		print bone.matrix['BONESPACE']
		print
		print "  ArmatureSpace:"
		print bone.matrix['ARMATURESPACE']
		print
	print

mesh = Blender.NMesh.GetRaw("Cube.001")
print mesh.faces[0].v[0].index

for v in mesh.verts:
	mesh.getVertexInfluences(v.index)
	
# EOF #

