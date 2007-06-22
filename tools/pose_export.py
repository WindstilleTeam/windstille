import Blender
from Blender import *

def is_identity(quat):
    return quat.w == 1.0 and quat.x == 0.0 and quat.y == 0.0 and quat.z == 0.0

def is_null_loc(loc):
    return loc.x == 0 and loc.y == 0 and loc.z == 0

def export_pose(out, obj):
    pose = obj.getPose()

    out.write(";; -*- scheme -*-\n")
    out.write("(pose\n")
    out.write("  (name \"%s\")\n" % obj.getName())
    out.write("  (bones\n")
    for bone in pose.bones.values():
        if bone.name[:3] == "IK_":
            out.write("    ;; ignoring '%s', IK control bone\n" % bone.name)
        elif is_identity(bone.quat) and is_null_loc(bone.loc):
            out.write("    ;; ignoring '%s', neutral position\n" % bone.name)        
        else: 
            out.write("    (bone\n")
            out.write("      (name \"%s\")\n" % bone.name)
            out.write("      (loc %f %f %f)\n" %
                      (bone.loc.x, bone.loc.y, bone.loc.z))
            out.write("      (quat %f %f %f %f))\n" % 
                      (bone.quat.w, bone.quat.x, bone.quat.y, bone.quat.z))
    out.write("   ) ;; bones\n")
    out.write(" ) ;; pose\n")
    out.write("\n;; EOF ;;\n")

print "Windstille Pose Exporter:"
for frame in xrange(1, 20):
    Blender.Set("curframe", frame)
    for obj in Blender.Object.Get():
        if obj.type == "Armature":
            filename = "/tmp/pose-%s-frame%03d.scm" % (obj.getName(), frame)
            out = file(filename, "w")
            export_pose(out, obj)
            out.close()
            print "Wrote:", filename

Draw.PupMenu("Export ok")

# EOF #
