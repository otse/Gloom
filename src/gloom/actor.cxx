#include <skyrim_units>
#include <libs>

#include <Gloom/Gloom.h>
#include <Gloom/Actor.h>
#include <Gloom/Collision.h>
#include <Gloom/Object.h>
#include <Gloom/ObjectArray.h>

#include <Gloom/Files.h>
#include <Gloom/Skeleton.h>
#include <Gloom/Interior.h>
#include <Gloom/Mesh.h>

#include <OpenGL/Group.h>
#include <OpenGL/DrawGroup.h>
#include <OpenGL/Scene.h>
#include <OpenGL/Camera.h>

namespace gloom
{
	Record *GetRace(const char *raceId)
	{
		Record *race = nullptr;
		ObjectArray array;
		array(esp_top_grup(get_plugins()[0], "RACE")).forEach([&](unsigned int &i) {
			Record *record = array.getRecord(i);
			auto editorId = getEditorIdOnly(record);
			if (strcmp(editorId, raceId) == 0)
			{
				race = record;
				array.stop = true;
			}
		});
		cassert(array.stop, "No such raceId !");
		return race;
	}

	Actor::Actor(const char *raceId, const char *model)
	{
		//printf("actor of race %s\n", raceId);

		Object race = Object(GetRace(raceId));

		ExportRaceHkxToKf(raceId);

		// cassert(race.Count("ANAM") == 2, "race count anam");

		auto ANAM = race.data<char *>("ANAM", 0);
		auto MODL = race.data<unsigned short *>("MODL", 2);
		auto MODLCHAR = race.data<char *>("MODL", 2);

		Field *modl = race.equalRange("MODL", 2);

		//printf("modl size %u str %s offset %u", modl->hed->type, (char *)modl->data, modl->offset);

		cassert(ANAM, "no actor race anam sub");

		Rc *rc = bsa_find(get_archives()[0], model);
		bsa_read(rc);
		Nif *character = malloc_nifp();
		character->path = model;
		character->buf = rc->buf;
		nifp_read(character);
		nifp_save((void *)model, character);

		skeleton = new Skeleton();
		skeleton->load(ANAM);
		skeleton->construct();
		skeleton->baseBone->group->visible = false;

		mesh = new Mesh();
		mesh->nif = character;
		mesh->construct();

		smesh = new SkinnedMesh();
		smesh->mesh = mesh;
		smesh->skeleton = skeleton;
		smesh->construct();

		if (raceId == "DraugrRace")
		{
			animation = new Animation(draugrAttack);
			animation->skeleton = skeleton;
			skeleton->animation = animation;
		}

		if (raceId == "ImperialRace")
		{
			animation = new Animation(humanIdle);
			animation->skeleton = skeleton;
			skeleton->animation = animation;
		}
	}

	void Actor::PutDown(const char *q)
	{
		auto ref = dungeon->editorIds.find(q);

		if (ref != dungeon->editorIds.end())
		{
			Group *group = new Group();
			group->Add(skeleton->baseBone->group);
			group->Add(mesh->baseGroup);
			//printf("make smesh->skeleton drawGroup!\n");
			drawGroup = new DrawGroup(group, ref->second->matrix);
			sceneDefault->drawGroups.Add(drawGroup);
		}
		else
		{
			printf("actor put down cant find %s\n", q);
		}
	}

	void Actor::step()
	{
		if (smesh)
			smesh->forward();
		//const float merry = 0.002;
		//if (drawGroup)
		//drawGroup->matrix = glm::rotate(drawGroup->matrix, merry, vec3(0, 0, 1));
	}

	Human::Human()
	{
		hat = head = body = hands = feet = nullptr;

		const bool beggar = true;
		const bool greybeard = false;
		if (beggar)
		{
			hat = new Actor("ImperialRace", "meshes\\clothes\\beggarclothes\\hatm_0.nif");
			head = new Actor("ImperialRace", "meshes\\actors\\character\\character assets\\malehead.nif");
			body = new Actor("ImperialRace", "meshes\\clothes\\prisoner\\prisonerclothes_0.nif");
			hands = new Actor("ImperialRace", "meshes\\clothes\\prisoner\\prisonercuffs_0.nif");
			feet = new Actor("ImperialRace", "meshes\\clothes\\prisoner\\prisonershoes_0.nif");
		}
		if (greybeard)
		{
			//head = new Actor("ImperialRace", "meshes\\clothes\\graybeardrobe\\greybeardhat_0.nif");
			//body = new Actor("ImperialRace", "meshes\\clothes\\graybeardrobe\\greyboardrobe_0.nif");
			//feet = new Actor("ImperialRace", "meshes\\clothes\\graybeardrobe\\greybeardboots_0.nif");
		}

		group = new Group;
		//group->matrix = glm::translate(mat4(1), vec3(0, 0, 200));
		if (hat)
			group->Add(hat->mesh->baseGroup);
		if (head)
			group->Add(head->mesh->baseGroup);
		if (body)
			group->Add(body->mesh->baseGroup);
		if (hands)
			group->Add(hands->mesh->baseGroup);
		if (feet)
			group->Add(feet->mesh->baseGroup);

		drawGroup = new DrawGroup(group, mat4());

		csphere = nullptr;
	};

	void Human::Place(const char *q)
	{
		auto ref = dungeon->editorIds.find(q);
		if (ref == dungeon->editorIds.end())
			return;
		drawGroup->matrix = ref->second->matrix;
		csphere = new CSphere(vec3(drawGroup->matrix[3]) /*+vec3(0, 0, 1)*/);
		sceneDefault->drawGroups.Add(drawGroup);
		// Create an offsetted mirror of Man
		/*DrawGroup *mirror = new DrawGroup(group, mat4());
		mirror->matrix = drawGroup->matrix;
		mirror->matrix = glm::translate(mirror->matrix, vec3(50, 0, 0));*/
		//sceneDefault->Add(mirror);
	}

	void Human::step()
	{
		if (hat)
			hat->step();
		if (head)
			head->step();
		if (body)
			body->step();
		if (hands)
			hands->step();
		if (feet)
			feet->step();
		if (csphere)
		{
			//drawGroup->matrix = translate(drawGroup->matrix, csphere->GetWorldTransform());
			//drawGroup->matrix = translate(mat4(1.0), csphere->GetWorldTransform());
			//drawGroup->Reset();
		}
	}

	Player::Player()
	{
		human = new Human();
		//human->Place("gloomgenman");
		drawGroup = new DrawGroup(human->group, mat4());
		drawGroup->group->visible = false;
		sceneDefault->drawGroups.Add(drawGroup);
		//cameraCurrent->group->Add(human->group);
		//fpc = new FirstPersonCamera;
		pose = cameraCurrent->pos;

		thirdPersonCamera = new ViewerCamera;
	}

	void Player::step()
	{
		move();

		cameraCurrent->pos = pose;

		human->step();

		//if (!dynamic_cast<FirstPersonCamera *>(cameraCurrent))
		//	return;

		vec3 down = vec3(0, 0, SU_TO_CM(-150));
		drawGroup->matrix = glm::translate(mat4(1.0), down + pose);
		drawGroup->matrix = rotate(drawGroup->matrix, -cameraCurrent->yaw, vec3(0, 0, 1));
		drawGroup->Reset();
		//human->drawGroup->matrix = drawGroup->matrix;

	}

	void Player::toggleView()
	{
		thirdPerson = ! thirdPerson;

		if (thirdPerson) {
			cameraCurrent = thirdPersonCamera;
			drawGroup->group->visible = true;
			thirdPersonCamera->disabled = false;
			thirdPersonCamera->pos = pose;
			thirdPersonCamera->radius = 200;
		}
		else
		{
			cameraCurrent = firstPersonCamera;
			firstPersonCamera->pos = pose;
			drawGroup->group->visible = false;
		}
	}

	void Player::move()
	{
		using namespace MyKeys;

		yaw = cameraCurrent->yaw;

		auto forward = [&](float n) {
			pose.x += n * sin(yaw);
			pose.y += n * cos(yaw);
		};

		auto strafe = [&](float n) {
			pose.x += n * cos(-yaw);
			pose.y += n * sin(-yaw);
		};

		float speed = 100.f * gloom::delta;

		if (shift)
			speed /= 10;

		if (w && !s)
			forward(speed);
		if (s && !w)
			forward(-speed / 2);

		if (a && !d)
			strafe(-speed);
		if (d && !a)
			strafe(speed);

		if (r)
			pose.z += speed / 2;
		if (f)
			pose.z -= speed / 2;
	}


} // namespace gloom