#ifndef __TEST_OBJECT_HPP__
#define __TEST_OBJECT_HPP__

#include <vector>
#include "game_object.hpp"
#include "sprite3d/sprite3d.hpp"
#include "math/vector.hpp"
#include "lisp/lisp.hpp"

class TestObject : public GameObject
{
public:
  TestObject(FileReader& reader);
  virtual ~TestObject();

  void draw(SceneContext& context);
  void update(float delta);

  void set_sprite(const std::string& filename);
  void set_action(const std::string& action);
  void set_vflip(bool vflip);
  void set_pos(const Vector& pos);
  void attach(const std::string& spritename,
		      const std::string& attachement_point);

private:
  Sprite3D sprite;
  struct AttachedSprite {
    Sprite3D sprite;
    Sprite3D::PointID attachpoint;
  };
  std::vector<AttachedSprite> attached_sprites;
  Vector pos;
};

#endif

