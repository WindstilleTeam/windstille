import Blender
from Blender import *

print "-------------------------------------------"
for obj in Blender.Object.Get():
	if obj.type == "Armature":
		print "-------------", obj.getName()
		pose = obj.getPose()
		for bone in pose.bones.values():
			print bone.quat

def export_pose(out, obj):
	pose = obj
	for bone in pose.bones.values():
		

for obj in Blender.Object.Get():
	if obj.type == "Armature":
		out = file("/tmp/pose-%s.scm" % obj.getName(), "w")
		export_pose(out, obj)
		out.close()

# EOF #