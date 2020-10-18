/* Open GL Area
 *
 * Gtk::GLArea is a widget that allows custom drawing using OpenGL calls.
 *
 */

#include <iostream>
#include <string>
#include <vector>

#include <gtkmm/main.h>
#include <gtkmm.h>
#include <giomm/resource.h>
#include <epoxy/gl.h>

using std::cerr;
using std::endl;
using std::string;

enum {
  X_AXIS,
  Y_AXIS,
  Z_AXIS,

  N_AXIS
};

static const GLfloat vertex_data[] = {
  0.f,   0.5f,   0.f, 1.f,
  0.5f, -0.366f, 0.f, 1.f,
 -0.5f, -0.366f, 0.f, 1.f,
};

class Example_GLArea : public Gtk::Window
{
public:
  Example_GLArea();
  ~Example_GLArea() override;

protected:
  Gtk::Box m_VBox {Gtk::ORIENTATION_VERTICAL, false};
  Gtk::GLArea m_GLArea;
  Gtk::Box m_Controls {Gtk::ORIENTATION_VERTICAL, false};
  Gtk::Button m_Button {"Quit"};

  GLuint m_Vao {0};
  GLuint m_Buffer {0};
  GLuint m_Program {0};
  GLuint m_Mvp {0};

  std::vector<float> m_RotationAngles;

  void on_axis_value_change(int axis, const Glib::RefPtr<Gtk::Adjustment>& adj);

  void realize();
  void unrealize();
  bool render(const Glib::RefPtr<Gdk::GLContext>& context);

  Gtk::Box* create_axis_slider_box(int axis);
  void init_buffers();
  void init_shaders();
  void draw_triangle();
};

Gtk::Window* do_glarea()
{
  return new Example_GLArea();
}

Example_GLArea::Example_GLArea() : m_RotationAngles(N_AXIS, 0.0f)
{
  set_title("GL Area");
  set_default_size(400, 600);
  set_border_width(12);

  m_VBox.set_spacing(6);
  add(m_VBox);

  m_GLArea.set_required_version(2, 0);

  m_GLArea.set_hexpand(true);
  m_GLArea.set_vexpand(true);
  m_GLArea.set_auto_render(true);
  m_VBox.add(m_GLArea);

  // Connect gl area signals
  m_GLArea.signal_realize().connect(sigc::mem_fun(*this, &Example_GLArea::realize));
  // Important that the unrealize signal calls our handler to clean up
  // GL resources _before_ the default unrealize handler is called (the "false")
  m_GLArea.signal_unrealize().connect(sigc::mem_fun(*this, &Example_GLArea::unrealize), false);
  m_GLArea.signal_render().connect(sigc::mem_fun(*this, &Example_GLArea::render));

  m_VBox.add(m_Controls);
  m_Controls.set_hexpand(true);

  for(int i = 0 ; i < N_AXIS ; ++i)
  {
    auto sliderBox = create_axis_slider_box(i);
    m_Controls.add(*sliderBox);
  }

  m_Button.set_hexpand(true);
  m_VBox.add(m_Button);
  // Connect clicked to close of window
  m_Button.signal_clicked().connect(sigc::mem_fun(*this, &Gtk::Window::close));

  show_all();
}

Example_GLArea::~Example_GLArea()
{
}

void Example_GLArea::on_axis_value_change(int axis, const Glib::RefPtr<Gtk::Adjustment>& adj)
{
  m_RotationAngles[axis] = adj->get_value();
  m_GLArea.queue_draw();
}

void Example_GLArea::realize()
{
  m_GLArea.make_current();
  try
  {
    m_GLArea.throw_if_error();
    init_buffers();
    init_shaders();
  }
  catch(const Gdk::GLError& gle)
  {
    cerr << "An error occured making the context current during realize:" << endl;
    cerr << gle.domain() << "-" << gle.code() << "-" << gle.what() << endl;
  }
}

void Example_GLArea::unrealize()
{
  m_GLArea.make_current();
  try
  {
    m_GLArea.throw_if_error();

    // Delete buffers and program
    glDeleteBuffers(1, &m_Vao);
    glDeleteProgram(m_Program);
  }
  catch(const Gdk::GLError& gle)
  {
    cerr << "An error occured making the context current during unrealize" << endl;
    cerr << gle.domain() << "-" << gle.code() << "-" << gle.what() << endl;
  }
}

bool Example_GLArea::render(const Glib::RefPtr<Gdk::GLContext>& /* context */)
{
  try
  {
    m_GLArea.throw_if_error();
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    draw_triangle();

    glFlush();

    return true;
  }
  catch(const Gdk::GLError& gle)
  {
    cerr << "An error occurred in the render callback of the GLArea" << endl;
    cerr << gle.domain() << "-" << gle.code() << "-" << gle.what() << endl;
    return false;
  }
}

Gtk::Box* Example_GLArea::create_axis_slider_box(int axis)
{
  auto box = Gtk::manage(new Gtk::Box{Gtk::ORIENTATION_HORIZONTAL, false});

  const char* text;

  switch(axis)
  {
    case X_AXIS:
    {
      text = "X axis";
      break;
    }
    case Y_AXIS:
    {
      text = "Y axis";
      break;
    }
    case Z_AXIS:
    {
      text = "Z axis";
      break;
    }
    default:
    {
      g_assert_not_reached();
    }
  }

  auto label = Gtk::manage(new Gtk::Label{text});
  box->add(*label);
  label->show();

  auto adj = Gtk::Adjustment::create(0.0, 0.0, 360.0, 1.0, 12.0, 0.0);

  adj->signal_value_changed().connect(
    sigc::bind(sigc::mem_fun(*this, &Example_GLArea::on_axis_value_change), axis, adj)
                                      );
  auto slider = Gtk::manage(new Gtk::Scale{adj, Gtk::ORIENTATION_HORIZONTAL});
  box->add(*slider);
  slider->set_hexpand(true);
  slider->show();

  box->show();

  return box;
}

void Example_GLArea::init_buffers()
{
  glGenVertexArrays(1, &m_Vao);
  glBindVertexArray(m_Vao);

  glGenBuffers(1, &m_Buffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static GLuint create_shader(int type, const char *src)
{
  auto shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);

  int status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if(status == GL_FALSE)
  {
    int log_len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);

    string log_space(log_len+1, ' ');
    glGetShaderInfoLog(shader, log_len, nullptr, (GLchar*)log_space.c_str());

    cerr << "Compile failure in " <<
      (type == GL_VERTEX_SHADER ? "vertex" : "fragment") <<
      " shader: " << log_space << endl;

    glDeleteShader(shader);

    return 0;
  }

  return shader;
}

void Example_GLArea::init_shaders()
{
  auto vshader_bytes = "#version 330\n"
    "layout(location = 0) in vec4 position;\n"
    "uniform mat4 mvp;\n"
    "void main() {\n"
    "  gl_Position = mvp * position;\n"
    "}\n";

  if(!vshader_bytes)
  {
    cerr << "Failed fetching vertex shader resource" << endl;
    m_Program = 0;
    return;
  }
  gsize vshader_size = sizeof(vshader_bytes);
  auto vertex = create_shader(GL_VERTEX_SHADER, (const char*)vshader_bytes);

  if(vertex == 0)
  {
    m_Program = 0;
    return;
  }

  auto fshader_bytes = "#version 330\n"
    "out vec4 outputColor;\n"
    "void main() {\n"
    "  float lerpVal = gl_FragCoord.y / 500.0f;\n"
    "  outputColor = mix(vec4(1.0f, 0.85f, 0.35f, 1.0f), vec4(0.2f, 0.2f, 0.2f, 1.0f), lerpVal);\n"
    "}\n";

  if(!fshader_bytes)
  {
    cerr << "Failed fetching fragment shader resource" << endl;
    glDeleteShader(vertex);
    m_Program = 0;
    return;
  }
  gsize fshader_size = sizeof(fshader_bytes);
  auto fragment = create_shader(GL_FRAGMENT_SHADER, (const char*)fshader_bytes);

  if(fragment == 0)
  {
    glDeleteShader(vertex);
    m_Program = 0;
    return;
  }

  m_Program = glCreateProgram();
  glAttachShader(m_Program, vertex);
  glAttachShader(m_Program, fragment);

  glLinkProgram(m_Program);

  int status;
  glGetProgramiv(m_Program, GL_LINK_STATUS, &status);
  if(status == GL_FALSE)
  {
    int log_len;
    glGetProgramiv(m_Program, GL_INFO_LOG_LENGTH, &log_len);

    string log_space(log_len+1, ' ');
    glGetProgramInfoLog(m_Program, log_len, nullptr, (GLchar*)log_space.c_str());

    cerr << "Linking failure: " << log_space << endl;

    glDeleteProgram(m_Program);
    m_Program = 0;
  }
  else
  {
    /* Get the location of the "mvp" uniform */
    m_Mvp = glGetUniformLocation(m_Program, "mvp");

    glDetachShader(m_Program, vertex);
    glDetachShader(m_Program, fragment);
  }
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

static void compute_mvp(float *res,
                        float phi,
                        float theta,
                        float psi)
{
  float x       {phi * ((float)G_PI / 180.f)};
  float y       {theta * ((float)G_PI / 180.f)};
  float z       {psi * ((float)G_PI / 180.f)};
  float c1      {cosf (x)};
  float s1      {sinf (x)};
  float c2      {cosf (y)};
  float s2      {sinf (y)};
  float c3      {cosf (z)};
  float s3      {sinf (z)};
  float c3c2    {c3 * c2};
  float s3c1    {s3 * c1};
  float c3s2s1  {c3 * s2 * s1};
  float s3s1    {s3 * s1};
  float c3s2c1  {c3 * s2 * c1};
  float s3c2    {s3 * c2};
  float c3c1    {c3 * c1};
  float s3s2s1  {s3 * s2 * s1};
  float c3s1    {c3 * s1};
  float s3s2c1  {s3 * s2 * c1};
  float c2s1    {c2 * s1};
  float c2c1    {c2 * c1};

  /* apply all three rotations using the three matrices:
   *
   * ⎡  c3 s3 0 ⎤ ⎡ c2  0 -s2 ⎤ ⎡ 1   0  0 ⎤
   * ⎢ -s3 c3 0 ⎥ ⎢  0  1   0 ⎥ ⎢ 0  c1 s1 ⎥
   * ⎣   0  0 1 ⎦ ⎣ s2  0  c2 ⎦ ⎣ 0 -s1 c1 ⎦
   */
  res[0] = c3c2;  res[4] = s3c1 + c3s2s1;  res[8] = s3s1 - c3s2c1; res[12] = 0.f;
  res[1] = -s3c2; res[5] = c3c1 - s3s2s1;  res[9] = c3s1 + s3s2c1; res[13] = 0.f;
  res[2] = s2;    res[6] = -c2s1;         res[10] = c2c1;          res[14] = 0.f;
  res[3] = 0.f;   res[7] = 0.f;           res[11] = 0.f;           res[15] = 1.f;
}

void Example_GLArea::draw_triangle()
{
  float mvp[16];

  compute_mvp(mvp,
              m_RotationAngles[X_AXIS],
              m_RotationAngles[Y_AXIS],
              m_RotationAngles[Z_AXIS]);

  glUseProgram(m_Program);

  glUniformMatrix4fv(m_Mvp, 1, GL_FALSE, &mvp[0]);

  glBindBuffer(GL_ARRAY_BUFFER, m_Vao);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

  glDrawArrays(GL_TRIANGLES, 0, 3);

  glDisableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glUseProgram(0);
}


int main (int argc, char *argv[])
{
  Glib::RefPtr<Gtk::Application> app =
    Gtk::Application::create(argc, argv);


  Example_GLArea window;


  return app->run(window);
}
