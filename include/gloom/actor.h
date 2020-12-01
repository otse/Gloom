#ifndef GLOOM_ACTOR_H
#define GLOOM_ACTOR_H

#include <gloom/dark2.h>

#include "object.h"

namespace gloom
{
	Object GetRace(const char *);
	//void Export(const char *);
	void ExportRaceHkxToKf(const char *);

	class Actor
	{
	public:
		Actor(const char *, const char *);

		SkinnedMesh *smesh = nullptr;

		DrawGroup *drawGroup = nullptr;
		
		float merry = 0;
		
		void Step();
	};

} // namespace gloom

#endif