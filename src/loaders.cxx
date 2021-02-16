#include <Gloom/Dark2.h>

#include <libs>

#include <Gloom/Mesh.h>
#include <Gloom/Files.h>

#include <OpenGL/Group.h>
#include <OpenGL/Camera.h>
#include <OpenGL/Scene.h>


#include <algorithm>

namespace gloom
{
	const char *dataFolder = "Data/";

	Rc *loadRc(const char *prepend = "meshes\\", const char *path = "", unsigned long flags = 0x1)
	{
		std::string str = prepend;
		str += path;
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
		Rc *rc = bsa_find_more(str.c_str(), flags);
		bsa_read(rc);
		return rc;
	}

	Nifp *loadNifp(Rc *rc, bool useCache)
	{
		cassert(rc, "mh no rc");
		Nifp *nif;
		nif = nifp_saved(rc);
		if (useCache && nif != NULL)
			return nif;
		bsa_read(rc);
		nif = malloc_nifp();
		nif->path = rc->path;
		nif->buf = rc->buf;
		nifp_read(nif);
		if (useCache)
			nifp_save(rc, nif);
		return nif;
	}

	Esp *loadEsp(const char *filename)
	{
		printf("Load Plugin %s\n", filename);
		std::string path = pathToOldrim + dataFolder + filename;
		char *buf;
		int ret;
		if ((ret = fbuf(filename, &buf)) == -1)
			ret = fbuf(path.c_str(), &buf);
		if (ret == -1)
		{
			printf(" Couldn't resolve (Data/)%s \n", filename);
			exit(1);
			return nullptr;
		}
		Esp *esp = plugin_slate();
		esp->name = filename;
		esp->buf = buf;
		esp->filesize = ret;
		plugin_load(esp);
		return esp;
	}

	Bsa *loadBsa(const char *filename)
	{
		printf("Load Archive %s\n", filename);
		std::string path = pathToOldrim + dataFolder + filename;
		if (exists(path.c_str()))
			return bsa_load(path.c_str());
		else
			return bsa_load(filename);
		return nullptr;
	}

	Mesh *loadMesh() {
		return nullptr;
	}

	void View(Rc *rc)
	{
		static Mesh *mesh = nullptr;
		static DrawGroup *drawGroup = nullptr;
		if (mesh)
		{
			scene->Remove(drawGroup);
			delete mesh;
			delete drawGroup;
		}
		Nifp *nif = loadNifp(rc, false); // Note no use of cache
		nifp_save(rc, nif);
		mesh = new Mesh;
		mesh->Construct(nif);
		drawGroup = new DrawGroup(mesh->baseGroup, glm::translate(mat4(1.0), first_person_camera->pos));
		scene->Add(drawGroup);
		HideCursor();
		first_person_camera->disabled = true;
		viewer_camera->disabled = false;
		camera = viewer_camera;
		viewer_camera->pos = drawGroup->aabb.center();
		//viewer_camera->pos = first_person_camera->pos;
		viewer_camera->radius = drawGroup->aabb.radius2() * 2;
	}
} // namespace gloom