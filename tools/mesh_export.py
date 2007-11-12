import Blender
from Blender import *

class MeshData:
    """Data structure that holds a list of faces"""

    def __init__(self, texture_filename):
        # Filename of the used texture
        self.texture_filename = texture_filename

        # [FaceData, ...]
        self.faces    = []

        # [VerticeData, ...]
        self.vertices = []

        # { orig_index : [VerticeData, ...], ... }
        self.vertmap = {}

        # { (bonename, weight) : VertexGroup, ...}
        self.groups = {}

    def add(self, face):
        self.faces.append(face)
        
        for vert in face.verts:
            if not self.vertmap.has_key(vert.orig_idx):
                self.vertmap[vert.orig_idx] = []
            self.vertmap[vert.orig_idx].append(vert)

    def add_influences(self, orig_idx, influences):
        """influences -> [(bonename, weight), ...]"""
        if not self.vertmap.has_key(orig_idx):
            pass # this is normal due to MeshData not being a complete
                 # Blender mesh when multiple textures are in play
                 
        else:
            # orig_idx is part of this mesh
            for v in self.vertmap[orig_idx]:
                # print v.orig_idx
                v.influences = influences
            
            for (bone,weight) in influences:
                key = (bone, weight)
                if not self.groups.has_key(key):
                    self.groups[key] = VertexGroup(bone, weight)
                self.groups[key].add(self.vertmap[orig_idx])

    def finalize(self):
        """Reorders vertex indexes and merge vertexes which have the
        same UV coordinates, thus bringing the MeshData into a stage
        where it is ready to be written out to file"""

        # Generate a list of uniq vertices
        uniq_vertices = {}
        for face in self.faces:
            for vert in face.verts:
                uniq_vertices[vert.key()] = vert

        # Merge duplicate vertices
        for face in self.faces:
            for i in xrange(0, len(face.verts)):
                face.verts[i] = uniq_vertices[face.verts[i].key()]

        for group in self.groups.values():
            for i in xrange(0, len(group.vertices)):
                group.vertices[i] = uniq_vertices[group.vertices[i].key()]

        # Generate new index numbering
        self.vertices = uniq_vertices.values()
        for i, vert in enumerate(self.vertices):
            vert.index = i

        ## Remove '//' infront of the filename that Blender inserts there
        self.texture_filename = self.texture_filename[2:]
      
class FaceData:
    def __init__(self, verts):
        self.verts   = verts

class VertexData:
    def __init__(self, orig_idx, co, uv, normal):
        self.orig_idx   = orig_idx
        self.co         = co
        self.uv         = uv
        self.normal     = normal
        self.index      = -1
        self.influences = []

    def key(self):
        """Return something that can be used as a key to compare if two vertices are equal"""
        return (self.co[0], self.co[1], self.co[2],
                self.normal[0], self.normal[1], self.normal[2],
                self.uv[0], self.uv[1])
    

class VertexGroup:
    def __init__(self, bone, weight):
        self.bone     = bone
        self.weight   = weight
        self.vertices = [] # [VertexData,...]

    def add(self, v):
        self.vertices.extend(v)

class WindstilleModel:
    """WindstilleMesh is used to collect data vertex and face data
    from Blender internals and convert it to a form usable by
    OpenGL/Windstille"""  

    def __init__(self):
        # dictionary of { "texture_filename.png" : MeshData, ... } 
        self.mesh_data   = {}

    def add(self, blender_mesh):
        """Convert Blender data structures into something that is used by
        this export script"""
        
        # Convert blender meshes to MeshData
        for face in blender_mesh.faces:
            if face.image:
                texture_filename = face.image.filename
            else:
                texture_filename = "//404.png"
            
            if not self.mesh_data.has_key(texture_filename):
                mesh = self.mesh_data[texture_filename] = MeshData(texture_filename)
            else:
                mesh = self.mesh_data[texture_filename]

            if face.uv == []:
                raise "Face doesn't have UV"

            verts = []
            for i in [0, 1, 2]:
                verts.append(VertexData(face.v[i].index,
                                        [face.v[i].co[0], face.v[i].co[1], face.v[i].co[2]],
                                        [face.uv[i][0], 1.0 - face.uv[i][1]],
                                        [face.v[i].no[1], face.v[i].no[1], face.v[i].no[2]]))
            mesh.add(FaceData(verts))

            # Write out another triangle in case we have a quad
            if len(face.v) == 4:
                verts = []
                for i in [0, 2, 3]:
                    verts.append(VertexData(face.v[i].index,
                                            [face.v[i].co[0], face.v[i].co[1], face.v[i].co[2]],
                                            [face.uv[i][0], 1.0 - face.uv[i][1]],
                                            [face.v[i].no[0], face.v[i].no[1], face.v[i].no[2]]))
                mesh.add(FaceData(verts))

        for mesh in self.mesh_data.values():
            for vert in blender_mesh.verts:
                #if blender_mesh.getVertexInfluences(vert.index) == []:
                #    print "No influences on", vert.index, "this sounds like a bug"
                #else:
                #    print vert.index, "Ok"
                mesh.add_influences(vert.index, blender_mesh.getVertexInfluences(vert.index))


    def finalize(self):
        for (texture, mesh) in self.mesh_data.iteritems():
            mesh.finalize()

    def write(self, out):
        out.write(";; -*- scheme -*-\n")
        out.write("(windstille-model\n")
        out.write("  (name \"\")\n")
    
        for mesh in self.mesh_data.values():
            out.write("  (mesh\n")
            out.write("    (name \"\")\n")
            out.write("    (texture \"%s\")\n\n" % mesh.texture_filename)
            out.write("    (vertices\n")
            for v in mesh.vertices:
                out.write("      %10f %10f %10f ;; %d was %d\n" % (v.co[0], v.co[1], v.co[2],
                                                                   v.index, v.orig_idx))
            out.write("     ) ;; vertices\n\n")

            out.write("    (normals\n") 
            for vert in mesh.vertices:
                out.write("      %9f %9f %9f\n" % (vert.normal[0],
                                              vert.normal[1],
                                              vert.normal[2]))
            out.write("     ) ;; normals\n\n")

            out.write("    (texcoords\n")
            for vert in mesh.vertices:
                out.write("      %f %f\n" % (vert.uv[0], vert.uv[1]))
            out.write("     ) ;; texcoords\n\n")

            out.write("    (triangles\n")
            for face in mesh.faces:
                out.write("      %d %d %d\n" % (face.verts[0].index,
                                                face.verts[1].index,
                                                face.verts[2].index))
            out.write("     ) ;; triangles\n\n")

            out.write("    (groups\n")
            for group in mesh.groups.values():
                out.write("      (group\n")
                out.write("        (bone    \"%s\")\n" % group.bone)
                out.write("        (weight   %f)\n" % group.weight)
                out.write("        (vertices")
                for v in set(map(lambda v: v.index, group.vertices)):
                    out.write(" %d" % v) 
                out.write(")\n")
                out.write("       )\n")
            out.write("     ) ;; groups\n\n")
             
#             out.write("    (influences\n")
#             for vert in mesh.vertices:
#                 if vert.influences != []:
#                     out.write("      (vertex\n")
#                     out.write("        (index %d)\n" % vert.index)
#                     out.write("        (influeces")
#                     for (bone, weight) in vert.influences:
#                         out.write("\n          (influence (weight %f) (bone \"%s\"))" % (weight, bone))
#                     out.write("))\n")
#             out.write("     ) ;; influencs\n\n")
            
            out.write("   ) ;; mesh\n\n")
        out.write(" ) ;; windstille-model\n")
        out.write("\n;; EOF ;;\n")

    def print_stats(self):
        """Print some stats, vertex count, face count and such"""
        print "+===================================================================="
        print "| WindstilleSprite"
        print "+===================================================================="
        print "| Model:" 
        for i, mesh in enumerate(self.mesh_data.values()):
            print "|   Mesh:        %d/%d" % (i+1, len(self.mesh_data))
            print "|     Texture:  ", mesh.texture_filename
            print "|     Faces:    ", len(mesh.faces)
            print "|     Vertices: ", len(mesh.vertices)
            print "|"
        print "+===================================================================="


def export(filename):
    model = WindstilleModel()

    layers = Blender.Scene.getCurrent().Layers

    for obj in Blender.Object.Get():
        mesh = obj.getData()
        if (type(mesh) is not Blender.Types.NMeshType) or not mesh.faces:
            pass # ignore anything that isn't a mesh
        elif (obj.Layers & layers) == 0:
            print "Skipping \"%s\" because it is on an inactive layer" % obj.getName()
        else:
            model.add(mesh)

    model.finalize()

    file = open(filename, "w")

    model.write(file)
    file.close()

    model.print_stats()

export("/tmp/mesh.scm")

Draw.PupMenu("Export ok")

# EOF #
