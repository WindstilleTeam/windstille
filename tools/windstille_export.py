#!BPY

"""
Name: 'Windstille Exporter'
Blender: 233
Group: 'Export'
Tip: 'Export meshes/actions to windstille format'
"""

##  $Id: windstille_main.hxx,v 1.4 2003/11/07 13:00:39 grumbel Exp $
## 
##  Windstille - A Jump'n Shoot Game
##  Copyright (C) 2005 Matthias Braun <matze@braunis.de>,
##                     Ingo Ruhnke <grumbel@gmx.de>
##
##  This program is free software; you can redistribute it and/or
##  modify it under the terms of the GNU General Public License
##  as published by the Free Software Foundation; either version 2
##  of the License, or (at your option) any later version.
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
## 
##  You should have received a copy of the GNU General Public License
##  along with this program; if not, write to the Free Software
##  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

# Simple Python script that shows how to export animations from
# Blender
# Put this script in your $HOME/.blender/scripts directory.
#
# See windstille/docs/models.txt for more details

### TODO ###
# - add handling of meshes with armatures, but without actions
# - change the code to use some more MathUtil instead of its own quaternion/matrix code 
############

import string
import struct, shlex, os.path, math
import Blender
from Blender import NMesh
from Blender import Window

# SAMPLEFRAMES, only export every nth frame of the animation
DEFAULT_SAMPLERATE = 5
# ZOOM, is multiplied with all vertex coordinates
ZOOM = 32.0
DEFAULT_SPEED = 1.0
SPEED_MULTIPLIER = 9.8
# DO NOT change this
FORMAT_VERSION = 2

def progress(percent, str):
#    print "%3.2f%% - %s" % (percent*100, str)
    Window.DrawProgressBar(percent, str)

### Some math helper functions ###
def matrix2quaternion(m):
  tr = 1.0 + m[0][0] + m[1][1] + m[2][2]
  if tr > .00001:
    s = math.sqrt(tr)
    w = s / 2.0
    s = 0.5 / s
    x = (m[1][2] - m[2][1]) * s
    y = (m[2][0] - m[0][2]) * s
    z = (m[0][1] - m[1][0]) * s
  elif m[0][0] > m[1][1] and m[0][0] > m[2][2]:
    s = math.sqrt(1.0 + m[0][0] - m[1][1] - m[2][2])
    x = s / 2.0
    s = 0.5 / s
    y = (m[0][1] + m[1][0]) * s
    z = (m[2][0] + m[0][2]) * s
    w = (m[1][2] - m[2][1]) * s
  elif m[1][1] > m[2][2]:
    s = math.sqrt(1.0 + m[1][1] - m[0][0] - m[2][2])
    y = s / 2.0
    s = 0.5 / s
    x = (m[0][1] + m[1][0]) * s
    z = (m[1][2] + m[2][1]) * s
    w = (m[2][0] - m[0][2]) * s
  else:
    s = math.sqrt(1.0 + m[2][2] - m[0][0] - m[1][1])
    z = s / 2.0
    s = 0.5 / s
    x = (m[2][0] + m[0][2]) * s
    y = (m[1][2] + m[2][1]) * s
    w = (m[0][1] - m[1][0]) * s

  return quaternion_normalize([w, x, y, z])

def quaternion_normalize(q):
  l = math.sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3])
  return q[0] / l, q[1] / l, q[2] / l, q[3] / l

def quaternion_to_axisangle(q):
  cos_a = q[0]
  angle = math.acos(cos_a) * 2.0
  sin_a = math.sqrt(1.0 - cos_a * cos_a)
  if(sin_a < .0005 or sin_a > .0005): sin_a = 1
  return angle, q[1]/sin_a, q[2]/sin_a, q[3]/sin_a

def get_text(textname):
  """Little shortcut function to return the content of
  Blender.Text.get(textname) as a single string and do a little error
  handling in addition"""
  try:
    textobj = Blender.Text.Get(textname)
  except Exception, err:
    print "WARNING: ", err
    return ""
  else:
    return string.join(textobj.asLines(), "\n")

### Data Structures to hold the Mesh ###
class MeshData:
  def __init__(self, texture_filename):
    # Filename of the used texture
    self.texture_filename = texture_filename

    # [FaceData, ...]
    self.faces            = []

    self.vertices         = []

  def merge(self, mesh):
      """Merges a mesh with self"""
      if self.texture_filename != mesh.texture_filename:
          raise Exception, "Error: MeshData:merge: meshes can only be merged if they have the same texture"
      elif self.vertices != []:
          raise Exception, "Error: MeshData:merge: Must merge the meshes before finalization"
      else:
          self.faces += mesh.faces

  def finalize(self):
      """Reorders vertex indexes and merge vertexes which have the
      same UV coordinates, thus bringing the MeshData into a stage
      where it is ready to be written out to file"""

      # Merge vertices with the same UV
      vertices = {}
      for face in self.faces:
          for vert in face.verts:
              key = (vert.uv[0], vert.uv[1])
              vertices[key] = vert

      print "Vertices: ", len(vertices)

      # FIXME: This might not work with vertices that have the same
      # uv, but different positions
      for face in self.faces:
          for vi in range(0, len(face.verts)):
              key = (face.verts[vi].uv[0], face.verts[vi].uv[1])
              face.verts[vi] = vertices[key]

      self.vertices = vertices.values()

      # Sort the vertices by object to allow faster export in collect_frame_data()
      self.vertices.sort(lambda x, y: cmp(x.object.getName(), y.object.getName()))
      
      # Generate new index numbering
      for i, vert in enumerate(self.vertices):
          vert.new_index = i

      ## Remove '//' infront of the filename that Blender inserts there
      self.texture_filename = self.texture_filename[2:]
      
class FaceData:
    def __init__(self, verts, normal):
        self.verts   = verts
        self.normal  = normal

class VertexData:
    def __init__(self, object, index, uv, normal):
        self.object    = object
        self.index     = index
        self.uv        = uv
        self.normal    = normal
        self.new_index = -1

class AttachmentPointData:
    """Data for an attachment point, its location and its rotation"""
    def __init__(self, loc, quat):
        self.loc  = loc
        self.quat = quat

class FrameData:
    """ Data used for a single frame in an action """
    def __init__(self, vertex_locs, attachment_points):
        # Format: [[[x,y,z], ...], [[x,y,z],  ...], ...] (one list for each Mesh)
        self.vertex_locs        = vertex_locs

        # [AttachmentPointData, ...]
        self.attachment_points = attachment_points

class ActionConfig:
    """ActionConfig handles the properties of a single
    action, ie. when it starts, when it stops, its speed, how many
    spamles should be taken, etc."""

    def __init__(self, first_frame, last_frame, speed, samplerate, markers):
        self.first_frame = first_frame
        self.last_frame  = last_frame
        self.speed       = speed
        self.samplerate  = samplerate
        self.markers     = markers
        self.numframes   = last_frame - first_frame + 1

    def __str__(self):
        return "Frames: %3i - %3i, speed: %3.2f, Samplerate: %3d" % (self.first_frame, self.last_frame,
                                                                         self.speed, self.samplerate)

    # config entry (first_frame, last_frame, speed, samplerate, markers[])
    #  a marker is (name, frame)
    def parse(text):     
        def expect_string():
            res = lex.get_token()
            if res == lex.eof:
                raise Exception, "Expected string, got EOF"
            return res

        def expect_int():
            res = lex.get_token()
            if res == lex.eof:
                raise Exception, "Expected in, got EOF"
            return int(res)

        def expect_float():
            res = lex.get_token()
            if res == lex.eof:
                raise Exception, "Expected float, got EOF"
            return float(res)

        def expect_dash():
            res = lex.get_token()
            if res == lex.eof:
                raise Exception, "Expected '-', got EOF"
            elif res != "-":
                raise Exception, "Expected '-', got '%s'" % res

        lex = shlex.shlex(text)
        lex.wordchars += "."

        actionconfig = {}
        while True:
            token = lex.get_token()
            if token == lex.eof:
                break
            lex.push_token(token)
            action_name = expect_string()
            first_frame = expect_int()
            expect_dash()
            last_frame = expect_int()

            token = lex.get_token()
            if token == "speed":
                speed = expect_float()
            else:
                lex.push_token(token)
                speed = DEFAULT_SPEED

            token = lex.get_token()
            if token == "samplerate":
                samplerate = expect_int()
            else:
                lex.push_token(token)
                samplerate = DEFAULT_SAMPLERATE

            token = lex.get_token()
            markers = []
            while token == "marker":
                marker_name = expect_string()
                marker_frame = expect_int()
                markers.append( (marker_name, marker_frame) )
                token = lex.get_token()
            lex.push_token(token)

            actionconfig[action_name] = ActionConfig(first_frame, last_frame, speed, samplerate, markers)

        return actionconfig
    parse = staticmethod(parse)

class ActionData:
  def __init__(self, name, config, frame_data):
    # name as string
    self.name       = name

    # ActionConfig
    self.config     = config

    # FrameData (filled out later in the WindstilleExporter)
    self.frame_data = frame_data

### end: Data Structures to hold the Mesh ###

class WindstilleSprite:
  ########################################################
  def __init__(self):
      """ Create a WindstilleSprite from the current scene"""
      ### References to Blender internals
      # List of Blender mesh objects that should get exported (everything on
      # seperate layer is ignored)
      self.mesh_objects    = []

      # Blender armature object
      self.armature_object = None

      # Blender attachment objects, ie. Empties
      self.attachment_objects = []

      # Name of the actions as string
      self.actions = []

      # { actionname : ActionConfig, ... }
      self.actionconfigs = {}
      
      ### Converted Data
      # List of mesh_data, ie. data we collected and converted ourself from Blender
      self.mesh_data   = []
            
      # Used to store animation data (vertex position and such)
      self.action_data = []

      ### Start collecting data
      # Collect Action Config
      self.collect_actionconfig()
      # Fill out Blender internal references
      self.collect_data()
      # Data for all Meshes
      self.collect_object_data()
      # Collect data for the Actions and Animations
      self.collect_animation_data()

  def collect_actionconfig(self):
      try:
          self.actionconfigs = ActionConfig.parse(get_text("actionconfig"))
      except Exception, message:
          raise Exception, "Error: Parse Error in actionconfig:" + str(message)
      
  def collect_data(self):
      """Convert Blender data structures into something that is used by
      this export script"""

      scene  = Blender.Scene.GetCurrent()
      layers = scene.Layers

      # compose list of meshs to export
      for obj in Blender.Object.Get():
          data = obj.getData()
          if (type(data) is not Blender.Types.NMeshType) or not data.faces:
              pass # ignore anything that isn't a mesh
          elif (obj.Layers & layers) == 0:
              print "Skipping \"%s\" because it is on different layer" % obj.getName()
          else:
              texture = data.faces[0].image
              self.mesh_objects.append(obj)

      # search for armature object
      armatures = [obj for obj in Blender.Object.Get() if obj.getType() == "Armature"]
      if len(armatures) > 1:
          raise Exception, "Need to have at most 1 armature in the scene"
      elif len(armatures) == 0:
          self.armature_object = None
      else:
          self.armature_object = armatures[0]

      # compose list of objects for attachment points
      self.attachment_objects += [obj for obj in Blender.Object.Get() 
                                  if (obj.getType() == 'Empty' and obj.getName().startswith("A:"))]

      # compose list of actions to export
      if not self.armature_object:
          self.actions.append("Default")
      else:
          for action in Blender.Armature.NLA.GetActions().iteritems():
              self.actions.append(action[1])
        
  ### END: def collect_data()

  def collect_animation_data(self):
      for index, action in enumerate(self.actions):
          progress(float(index)/len(self.actions)*0.5, "Collecting Action %s" % (action.name))
          
          action.setActive(self.armature_object)

          # find/autodetect config
          if self.actionconfigs.has_key(action.getName()):
              actioncfg = self.actionconfigs[action.getName()]
          else:
              print "Error: No config for action '%s' defined." % action.getName()
              actioncfg = ActionConfig(1, 1, DEFAULT_SPEED, DEFAULT_SAMPLERATE, [])

          frame_data = []
          for frame in range(actioncfg.first_frame, actioncfg.last_frame+1, actioncfg.samplerate):
              #progress((index + (float(frame)/(actioncfg.last_frame+1 - actioncfg.first_frame)))
              #         /len(self.actions)*0.5,
              #         "Collecting Action %s" % (action.name))
              Blender.Set("curframe", int(frame))
              frame_data.append(self.collect_frame_data())

          self.action_data.append(ActionData(action.getName(), actioncfg, frame_data))

  def collect_object_data(self):
    self.mesh_data = {}
    for obj in self.mesh_objects:
      ### Convert mesh_objects to MeshData and merge all meshes with
      ### the same texture
      for (texture, mesh) in self.collect_mesh_data(obj).iteritems():
        if self.mesh_data.has_key(texture):
          self.mesh_data[texture].merge(mesh)
        else:
          self.mesh_data[texture] = mesh
    self.mesh_data = self.mesh_data.values()

    # Optimize the meshs by merging meshes and vertices
    for mesh in self.mesh_data:
      mesh.finalize()

  def collect_mesh_data(self, obj): # returns { texture_filename : MeshData, ...}
      """
      Returns mesh_data as dict with format:
      key: texture_filename
      val: MeshData
      """
      mesh_data = {}

      for face in obj.getData().faces:
          if face.image:
              texture_filename = face.image.filename
          else:
              texture_filename = "//404.png"

          if not mesh_data.has_key(texture_filename):
              mesh_data[texture_filename] = MeshData(texture_filename)

          faces = []
          for v in [0, 1, 2]:
              faces.append(VertexData(obj, face.v[v].index,
                                      [face.uv[v][0], 1.0-face.uv[v][1]],
                                      [face.normal[1], -face.normal[2], -face.normal[0]]))
          mesh_data[texture_filename].faces.append(FaceData(faces,
                                                            [face.normal[1], -face.normal[2], -face.normal[0]]))

          # Write out another triangle in case we have a quad: index, u, v
          faces = []
          if len(face.v) == 4:
              for v in [0, 2, 3]:
                  faces.append(VertexData(obj, face.v[v].index,
                                          [face.uv[v][0], 1.0-face.uv[v][1]],
                                          [face.normal[1], -face.normal[2], -face.normal[0]]))
              mesh_data[texture_filename].faces.append(FaceData(faces,
                                                                [face.normal[1], -face.normal[2], -face.normal[0]]))

      return mesh_data

  def collect_frame_data(self):
      """
      Collect all data for the given object in a single frame, frame
      has to be selected outside of this function.
      Format:
      [[[vertex_pos_x, vertex_pos_y, vertex_pos_z], ...],
      [[attachment_pos_x, attachment_pos_y, attachment_pos_z,
      attachment_quat1, attachment_quat2, attachment_quat3, attachment_quat4], ...]]
       """
      # FIXME: This is by far the slowest function of all, optimizing might help
      meshs = []
      attachment_points = []

      obj = None

      # no triangles here (those are global), just vertexes (those are local)
      for mesh_data in self.mesh_data:
          vertex_positions = []
          for vertex in mesh_data.vertices:
              if obj != vertex.object:
                  obj  = vertex.object
                  data = Blender.NMesh.GetRawFromObject(obj.getName())
                  m    = obj.getMatrix()

              # location: action/frame/mesh/vertices
              v = data.verts[vertex.index]
              vertex_positions.append([+(m[0][1]*v[0] + m[1][1]*v[1] + m[2][1]*v[2] + m[3][1]) * ZOOM,
                                       -(m[0][2]*v[0] + m[1][2]*v[1] + m[2][2]*v[2] + m[3][2]) * ZOOM,
                                       -(m[0][0]*v[0] + m[1][0]*v[1] + m[2][0]*v[2] + m[3][0]) * ZOOM])
          meshs.append(vertex_positions)

      # attachment points
      for obj in self.attachment_objects:
          m    = obj.matrixWorld
          loc  = (m[3][0] * ZOOM, m[3][1] * ZOOM, m[3][2] * ZOOM)
          quat = matrix2quaternion(m)      
          attachment_points.append(AttachmentPointData([loc[1], -loc[2], -loc[0]],
                                                       [quat[0], quat[2], quat[3], quat[1]]))

      return FrameData(meshs, attachment_points)

  def write(self, out):
      """This is a new version of write_file(), instead of using blender
      internals, it uses the collected data"""

      ### Write magic, version and counts for mesh, attachment_points and actions
      out.write(struct.pack("=4sHHHH", "W3DS",
                            FORMAT_VERSION, \
                            len(self.mesh_data),
                            len(self.attachment_objects),
                            len(self.action_data)))

      ### Mesh Header:
      for mesh in self.mesh_data:
          out.write(struct.pack("=64sHH",
                                mesh.texture_filename,
                                len(mesh.faces),
                                len(mesh.vertices)))
          ### Mesh Data:
          ## Vertex indices of triangles
          for face in mesh.faces:
              out.write(struct.pack("=HHH",
                                    face.verts[0].new_index,
                                    face.verts[1].new_index,
                                    face.verts[2].new_index))

          ## Normal
          for face in mesh.faces:
              out.write(struct.pack("=fff", face.normal[0], face.normal[1], face.normal[2]))

          ## UV Coordinates
          for vert in mesh.vertices:
            out.write(struct.pack("=ff", vert.uv[0], vert.uv[1]))

      ### Attachment points
      for obj in self.attachment_objects:
          out.write(struct.pack("=64s", obj.getName()[2:]))

      ## Action Header
      for index, action in enumerate(self.action_data):
          progress(0.5 + float(index)/len(self.action_data) * 0.5,
                   "Writing Action %s" % (action.name))

          out.write(struct.pack("=64sfHH",
                                action.name, 
                                action.config.speed * SPEED_MULTIPLIER,
                                len(action.config.markers),
                                len(action.frame_data)))
          
          ## Marker
          for (marker_name, marker_frame) in action.config.markers:
              out.write(struct.pack("=64sH",
                                    marker_name, 
                                    (marker_frame - action.config.first_frame) / action.config.samplerate))

          ## Action Data
          for frame in action.frame_data:
              ### Vertex positions
              for mesh in frame.vertex_locs:
                  for vert in mesh:
                      out.write(struct.pack("=fff", vert[0], vert[1], vert[2]))
              ### Attachment Points
              for at in frame.attachment_points:
                  out.write(struct.pack("=fffffff",
                                        at.loc[0],  at.loc[1],  at.loc[2],
                                        at.quat[0], at.quat[1], at.quat[2], at.quat[3]))
    ### DONE ###

  def print_stats(self):
      """Print some stats, vertex count, face count and such"""
      print "+===================================================================="
      print "| WindstilleSprite"
      print "+===================================================================="
      print "| Mesh Count:     ", len(self.mesh_data)
      print "| Mesh Objects:   ", self.mesh_objects
      print "| Actions:        ", self.actions
      print "| AttachmentObjs: ", self.attachment_objects
      print "| Meshs: "
      for mesh in self.mesh_data:
          print "|   Mesh:       ", mesh
          print "|     Texture:  ", mesh.texture_filename
          print "|     Faces:    ", len(mesh.faces)
          print "|     Vertices: ", len(mesh.vertices)
          print "|"
      print "| Actions: "
      for action in self.action_data:
          print "|   Name:        ", action.name
          print "|   Config:      ", action.config
          print "|   Frames:      ", len(action.frame_data)
          print "|   Vertices:    ", [len(x) for x in action.frame_data[0].vertex_locs]
          print "|   Attachments: ", len(action.frame_data[0].attachment_points)
          print "|"
      print "+===================================================================="
    
def export(filename):
    data = WindstilleSprite()

    file = open(filename, "wb")
    data.write(file)
    file.close()

    data.print_stats()

def fs_callback(filename):
    print "=== Exporting: %s ===" % (filename)
    export(filename)
    Window.DrawProgressBar(1.0, "Finished writing %s" % (filename)) 
    print "=== Windstille Exporter done, wrote %s ===" % (filename)

defaultname = Blender.Get("filename")
if defaultname.endswith(".blend"):
    defaultname = defaultname[0:len(defaultname) - len(".blend")] + ".wsprite"
Window.FileSelector(fs_callback, "Windstille Export", defaultname)

# EOF #
