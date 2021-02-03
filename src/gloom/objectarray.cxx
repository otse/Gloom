#include <libs>

#include <Gloom/Object.h>
#include <Gloom/ObjectArray.h>

namespace gloom
{
	void gloom_object_array_example(Grup *grup)
	{
		using Objects = ObjectArray;

		ObjectArray objectArray(grup);
		
		bool stop = false;
		
		objectArray.ForEach(RECORD, stop, [&](Objects &oa, size_t &i) {
			Object object(oa.GetRecord(i));
		});

		Objects(grup).ForEach(0, stop, [&](Objects &oa, size_t &i) {
			i += 4;
			int type = oa.Type(i);
			stop = true;
		});
	}

	ObjectArray::ObjectArray(Grup *grup) : grup(grup)
	{
		cassert(grup, "objectarray grup null");
	}
} // namespace gloom