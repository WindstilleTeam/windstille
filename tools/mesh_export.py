import Blender
from Blender import *

print "---------------------------------------------------"

def export_mesh(out, mesh):
	print ""

	out.write("  (vertices\n")
	for v in mesh.verts:
		out.write("    %f %f %f ;; %d\n" % (v.co[0], v.co[1], v.co[2], v.index))
	out.write("   ) ;; vertices\n")

	out.write("  (texcoords\n")
	for v in mesh.verts:
		out.write("    %f %f" % (v.uvco.x, v.uvco.y))	
	out.write("   ) ;; texcoords\n")

	out.write("  (faces\n")
	for face in mesh.faces:	
		out.write("    ")
		for v in face.verts:
			out.write("%d " % v.index)
		out.write("\n")
	out.write("   ) ;; faces\n")

print "Windstille Mesh Exporter:"
for mesh in Blender.Mesh.Get():
	filename = "/tmp/mesh-%s.scm" % mesh.name
	out = file(filename, "w")
	print "  - exporting to '%s'..." % filename,
 	export_mesh(out, mesh)
 	out.close()
	print "done"
print


# Draw.PupMenu("Export ok")

# EOF #