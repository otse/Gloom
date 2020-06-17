#include "material"
#include "texture"

#include "camera"
#include "shader"
#include "scene"


int material_t::POOL = 0;

material_t *material_t::BOUND = nullptr;

#define PRETTY_GRAY 17.0f / 255


material_t::material_t()
{
	id = POOL++;

	shader = sha_simple;

	transparent = double_sided = blending = testing = decal = false;

	opacity = 1.f;
	treshold = 0;
	shininess = 20.f;
	glossiness = 1.f;
	rotation = 0;

	depth_func = {GL_ONE, GL_ONE_MINUS_SRC_ALPHA};

	emissive = vec3(PRETTY_GRAY);

	compose_uv_transform();

	/*color = vec3(
		(float)rand() / RAND_MAX,
		(float)rand() / RAND_MAX,
		(float)rand() / RAND_MAX);*/
}

void material_t::compose_uv_transform()
{
	set_uv_transform_directly(
		offset.x, offset.y,
		repeat.x, repeat.y,
		rotation,
		center.x, center.y);
}

void material_t::set_uv_transform_directly(
	float tx, float ty, float sx, float sy, float rotation, float cx, float cy)
{
	float x = cos(rotation);
	float y = sin(rotation);

	float a, b, c, d, e, f, g, h, i;
	a = sx * x;
	b = sx * y;
	c = -sx * (x * cx + y * cy) + cx + tx;
	d = -sy * y;
	e = sy * x;
	f = -sy * (-y * cx + c * cy) + cy + ty;
	g = 0;
	h = 0;
	i = 1;

	uv_transform = mat3(
		a, d, g,
		b, e, h,
		c, f, i);
}

void material_t::Bind()
{
	if (this == BOUND)
		return;

	Unbind();

	shader->use();

	shader->setVec3("color", color);
	shader->setVec3("specular", specular);
	shader->setFloat("opacity", opacity);
	shader->setFloat("shininess", shininess);
	shader->setFloat("glossiness", glossiness);

	shader->setMat3("uvTransform", uv_transform);

	if (map)
	{
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, map->tid);
		shader->setInt("map", 0);
	}
	if (normal_map)
	{
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, normal_map->tid);
		shader->setInt("normalMap", 1);
	}
	if (decal)
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(-2.f, -2.f);
	}
	if (blending)
	{
		glEnable(GL_BLEND);
		glBlendFunc(depth_func.sfactor, depth_func.dfactor);
	}
	if (testing)
	{
		shader->setFloat("alphaTest", treshold / 255.f);
		glDepthFunc(GL_LEQUAL);
	}
	if (transparent)
	{
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LESS);
	}
	if (double_sided)
	{
		shader->setBool("doubleSided", true);
		glDisable(GL_CULL_FACE);
	}

	BOUND = this;

	//if (checkGlError) {
	//	detect_opengl_error("bind material " + name);
	//}
}

void material_t::Unbind()
{
	if (!map)
	{
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	if (!normal_map)
	{
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	if (!decal)
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0, 0);
	}
	if (!blending)
	{
		glDisable(GL_BLEND);
	}
	if (!testing)
	{
		shader->setFloat("alphaTest", 0);
		glDepthFunc(GL_LEQUAL);
	}
	if (!transparent)
	{
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
	}
	if (!double_sided)
	{
		shader->setBool("doubleSided", false);
		glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
		//glFrontFace(GL_CCW);
	}

	//if (checkGlError) {
	//	detect_opengl_error("unbind material " + name);
	//}
}