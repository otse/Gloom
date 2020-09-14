#ifndef OPENGL_GROUP_H
#define OPENGL_GROUP_H

#include <opengl/types.h>
#include <opengl/aabb.h>

// A basic 3d object with a transformation

struct Group
{
	static int num;

	std::string name; 
	std::vector<Group *> groups, flat;

	mat4 matrix, matrixWorld;

	Group *parent;

	Geometry *geometry, *axis;

	Group();
	~Group();
	void Add(Group *);
	void Update();
	void Draw(const mat4 &);
	void DrawClassic(const mat4 &);
	void Flatten(Group *);
};

#endif