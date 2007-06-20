import os
import Blender
import Blender.Armature

os.system('clear')
arms = Blender.Armature.Get()
print ";;;;;;;;;;;;; Start of File ;;;;;;;;;;;;;;;;"

def vec2str(vec):
	return "%f %f %f" % (vec.x,vec.y,vec.z)

def quat2str(quat):
	return "%f %f %f %f" % (quat.x, quat.y, quat.z, quat.w)

def matrix2str(indent, m):
	return          ("%9f %9f %9f\n" % (m[0][0], m[0][1], m[0][2])) + \
           indent + ("%9f %9f %9f\n" % (m[1][0], m[1][1], m[1][2])) + \
           indent + ("%9f %9f %9f" % (m[2][0], m[2][1], m[2][2]))

def list2str(lst):
	str = ""
	for i in lst:
		str += " \"%s\"" % i
	return str

for arm in arms.values():	
	print "(armature"
	print "  (name \"%s\")" % (arm.name,)
	print "  (bones "
	
	for bone in arm.bones.values():
		if bone.name[:3] == "IK_": # ignore helper bones
			print "    ;; ignoring bone: %s" % bone.name
			print
		else:
			if bone.parent:
				print "    (bone"
			else:
				print "    (bone ;; Parentless bone, aka Master bone XXXXXXXXXXXXXXXXXXXXXXXXXXXX"
				
			print "      (name      \"%s\")" % bone.name
			
			print "      (children %s)" % list2str(map(lambda b: b.name, bone.children))
							
			# If a bone lacks parent its head is in armature space,
			# else in local bonespace
			if bone.parent:
				print "      (parent    \"%s\")" % bone.parent.name
				print "      (head      %s)" % (vec2str(bone.head['BONESPACE']),)
			else:
				print "      (parent )"
				print "      (head      %s)" % (vec2str(bone.head['ARMATURESPACE']),)				
			
			print "      (length    %s)" % bone.length
			# print "      (matrix   %s)" % matrix2str(" "*16, bone.matrix['BONESPACE'])
			print "      (quat      %s)" % quat2str(bone.matrix['BONESPACE'].toQuat())
			# print "###  BoneSpace:"
			# print bone.matrix['BONESPACE']
			# print
			# print "  ArmatureSpace:"
			#  print bone.matrix['ARMATURESPACE']
			print "     )\n"

	print "  ) ;; bones"
print " ) ;; armature"

# mesh = Blender.NMesh.GetRaw("Cube.001")
# print mesh.faces[0].v[0].index

# for v in mesh.verts:
#	mesh.getVertexInfluences(v.index)
	
# EOF #
