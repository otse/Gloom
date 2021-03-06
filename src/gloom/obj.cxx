#include <Gloom/Obj.h>

#include <Attic.hpp>

namespace gloom
{
	Objs *globalObjs;

	void objs_init()
	{
		globalObjs = new Objs;
	}

	void Objs::Add(Obj *obj)
	{
		VectorAdd<Obj *>(obj, objs);
	}

	void Objs::Remove(Obj *obj)
	{
		VectorRemove<Obj *>(obj, objs);
	}

	Obj::Obj()
	{
	}

	Obj::Obj(Ref *ref) : ref(ref)
	{
		
	}
} // namespace gloom