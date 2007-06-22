import Blender
from Blender import *

arms = Blender.Armature.Get()

def vec2str(vec):
    return "%f %f %f" % (vec.x,vec.y,vec.z)

def euler2str(euler):
    return "%f %f %f" % (euler.x, euler.y, euler.z)

def quat2str(quat):
    return "%f %f %f %f" % (quat.w, quat.x, quat.y, quat.z)

def matrix2str(indent, m):
        return          ("%9f %9f %9f %9f\n" % (m[0][0], m[0][1], m[0][2], 0)) + \
               indent + ("%9f %9f %9f %9f\n" % (m[1][0], m[1][1], m[1][2], 0)) + \
               indent + ("%9f %9f %9f %9f\n" % (m[2][0], m[2][1], m[2][2], 0)) + \
               indent + ("%9f %9f %9f %9f"   % (      0,       0,       0, 1))

def list2str(lst):
    str = ""
    for i in lst:
        str += " \"%s\"" % i
    return str

def export_armature(out, armature):
    out.write(";; -*- scheme -*-\n")
    out.write("(armature\n")
    out.write("  (name \"%s\")\n" % armature.name)
    out.write("  (bones\n")

    for bone in armature.bones.values():
        if bone.name[:3] == "IK_": # ignore helper bones
            out.write("    ;; ignoring bone: %s\n" % bone.name)
            out.write("\n")
        else:
            if bone.parent:
                out.write("    (bone\n")
            else:
                out.write("    (bone ;; a root bone\n")

            out.write("      (name      \"%s\")\n" % bone.name)

            out.write("      (children %s)\n" % list2str(map(lambda b: b.name, bone.children)))

            # If a bone lacks parent its head is in armature space,
            # else in local bonespace
            if bone.parent:
                out.write("      (parent    \"%s\")\n" % bone.parent.name)
                out.write("      (head      %s)\n" % (vec2str(bone.head['BONESPACE']),))
            else:
                out.write("      (parent )\n")
                out.write("      (head      %s)\n" % (vec2str(bone.head['ARMATURESPACE'])))

            out.write("      (length    %s)\n" % (bone.length))
            out.write("      (matrix   %s)\n" % matrix2str(" "*16, bone.matrix['BONESPACE']))
            out.write("      (quat      %s)\n" % quat2str(bone.matrix['BONESPACE'].toQuat()))
            out.write("      (euler     %s)\n" % euler2str(bone.matrix['BONESPACE'].toEuler()))
            # out.write("###  BoneSpace:"
            # out.write(bone.matrix['BONESPACE']
            # print
            # out.write("  ArmatureSpace:"
            #  out.write(bone.matrix['ARMATURESPACE']
            out.write("     )\n\n")
    out.write("  ) ;; bones\n")
    out.write(" ) ;; armature\n")

print "Windstille Bone Exporter:"
for armature in arms.values():
    filename = "/tmp/bone-%s.scm" % armature.name
    out = file(filename, "w")
    print "  - exporting to '%s'..." % filename,
    export_armature(out, armature)
    out.close()
    print "done"
print

Draw.PupMenu("Export ok")

# EOF #
