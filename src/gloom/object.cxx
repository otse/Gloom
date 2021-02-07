#include <cstdarg>

#include <libs>

#include <gloom/Object.h>
#include <gloom/ObjectArray.h>

namespace gloom
{
	void gloom_objects_example(Record *record)
	{
		Object object(record);
		cassert(object.Type("REFR"), "object not refr");
		auto formId = object.Data<unsigned int *>("NAME");
		auto editorId = object.Data<const char *>("EDID");
		if (formId)
		{
			Object baseObject((Record *)formId);
			bool plant_or_weapon = baseObject.TypeAny({"FLOR", "WEAP"});
		}
		if (object.Count("MEOW") > 2)
		{
			// Get the third field
			auto third = object.Data("MEOW", 2);
		}
	}

	const char *GetEditorIdOnly(Record *record)
	{
		return (const char *)record->fields.subrecords[0]->data;
	}

	unsigned int Object::Count(const char *type) const
	{
		return fields.count(*(unsigned int *)type);
	}

	unsigned int Object::Size() const
	{
		return record->fields.size;
	}

	Object::Object(Record *record) : record(record)
	{
		if (record == nullptr)
			return;
		cassert(((TypeDud *)record)->x == 2, "Gloom/Object Not Record");
		for (unsigned int i = 0; i < record->fields.size; i++)
		{
			Subrecord *field = record->fields.subrecords[i];
			fields.emplace(field->hed->type, field);
		}
	}

	Subrecord *Object::GetSubrecordIndex(unsigned int i) const
	{
		return (Subrecord *)record->fields.subrecords[i];
	}

	Subrecord *Object::GetSubrecord(const char *type, int skip) const
	{
		Subrecord *sub = nullptr;
		auto st = fields.equal_range(*(unsigned int *)type);
		for (auto it = st.first; it != st.second; ++it)
		{
			sub = it->second;
			if (skip-- <= 0)
				break;
		}
		cassert(skip <= 0, "field skip unsuccessful\n");
		return sub;
	}

	const char *Object::EditorId()
	{
		return Data<const char *>("EDID", 0);
	}

	unsigned int *Object::BaseId()
	{
		return Data<unsigned int *>("NAME", 0);
	}

	/*::subrecord *Object::GetFrom(unsigned int type, int *n) const
	{
		for (int i = *n; i < record->fields.size; i++)
		{
			::subrecord *field = record->fields.subrecords[i];
			if (type == field->hed->type)
			{
				*n = i;
				return field;
			}
		}
		return nullptr;
	}*/

} // namespace gloom