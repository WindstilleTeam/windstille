import Blender
import Blender.Armature
from Blender import Ipo

def vec2str(vec):
    return "%f %f %f" % (vec.x,vec.y,vec.z)

def export_action(out, action):
    out.write(";; -*- scheme -*-\n")
    out.write("(action\n")
    out.write("  (name \"%s\")\n" % action.getName())
    out.write("  (bones\n")
    for ipo in action.getAllChannelIpos().values():
        out.write("    (bone\n")
        out.write("      (name \"%s\")\n" % ipo.getName())
        for (v, k) in [('quatw', Ipo.PO_QUATW), ('quatx', Ipo.PO_QUATX),
                       ('quaty', Ipo.PO_QUATY), ('quatz', Ipo.PO_QUATZ)]:
            if ipo[k]:
                out.write("      (%s " % v)
                for point in ipo[k].bezierPoints:
                    for v in point.vec:
                        out.write("%f %f %f" % (v[0], v[1], v[2]))
                out.write(")\n")                    
        out.write("     ) ;; bone \n")
    out.write("   ) ;; bones \n")
    out.write(" ) ;; action\n")    

for action in Blender.Armature.NLA.GetActions().values():
    out = file("/tmp/action-%s.scm" % action.getName(), "w")
    export_action(out, action)
    out.close

Draw.PupMenu("Export ok")

# EOF #
