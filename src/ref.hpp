#ifndef __REF_HPP__
#define __REF_HPP__

/** This class behaves like a pointer to a refcounted object, but increments the
 * reference count when new objects are assigned and decrements the refcounter
 * when it's lifetime has experied. (similar to std::auto_ptr)
 */
template<typename T>
class Ref
{
public:
  Ref(T* object = 0)
    : object(object)
  {
    if(object)
      object->ref();
  }
  Ref(const Ref<T>& other)
    : object(other.object)
  {
    if(object)
      object->ref();
  }
  ~Ref()
  {
    if(object)
      object->unref();
  }

  void operator= (const Ref<T>& other)
  {
    *this = other.get();
  }

  void operator= (T* object)
  {
    if(object)
      object->ref();
    if(this->object)
      this->object->unref();
    this->object = object;
  }

  T* operator ->() const
  {
    return object;
  }

  T& operator* () const
  {
    return *object;
  }

  operator const T* () const
  {
    return object;
  }

  T* get() const
  {
    return object;
  }

private:
  T* object;
};

#endif

