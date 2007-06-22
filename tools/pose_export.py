import Blender
from Blender import *

def export_pose(out, obj):
	pose = obj.getPose()
	
	out.write(";; -*- scheme -*-\n")
	out.write("(pose\n")
	out.write("  (name \"%s\")\n" % obj.getName())
	out.write("  (bones\n")
	for bone in pose.bones.values():
		out.write("    (bone\n")
		out.write("      (name \"%s\")\n" % bone.name)
		out.write("      (quat %f %f %f %f))\n" % 
			(bone.quat.w, bone.quat.x, bone.quat.y, bone.quat.z))
	out.write("   ) ;; bones\n")
	out.write(" ) ;; pose\n")
	out.write("\n;; EOF ;;\n")

print "Windstille Pose Exporter:"
for obj in Blender.Object.Get():
	if obj.type == "Armature":
                filename = "/tmp/pose-%s.scm" % obj.getName()
		out = file(filename, "w")
		export_pose(out, obj)
		out.close()
                print "Wrote:", filename

# EOF #
