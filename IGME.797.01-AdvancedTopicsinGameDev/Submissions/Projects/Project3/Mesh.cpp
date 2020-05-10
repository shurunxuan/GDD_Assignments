#include <GL/freeglut.h>
#include "Mesh.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <limits>

using namespace std;
using namespace glm;



Triangle::Triangle(Vertex* v0, Vertex* v1, Vertex* v2, int index) {
	assert(v0 != v1 && v1 != v2 && v2 != v0);
	vertex[0] = v0;
	vertex[1] = v1;
	vertex[2] = v2;
	id = index;
	setNormal();

	for (int i = 0; i < 3; i++) {
		vertex[i]->neighborTris.push_back(this);
		for (int j = 0; j < 3; j++) if (i != j) {
			vertex[i]->addUniqueNeighbor(vertex[j]);
		}
	}
}
Triangle::~Triangle() {
	int i;
	//if (hasVertex(1570))
	//std::cout << "Delete " << this << std::endl;
	for (i = 0; i < 3; i++) {
		if (vertex[i]) {
			vector<Triangle*>::iterator it = find(vertex[i]->neighborTris.begin(), vertex[i]->neighborTris.end(), this);
			//if (vertex[i]->id == 1570 || vertex[i]->neighborTris[it - vertex[i]->neighborTris.begin()]->hasVertex(1570))
			//std::cout << "Remove neighbor " << vertex[i]->neighborTris[it - vertex[i]->neighborTris.begin()] << " of " << vertex[i] << std::endl;
			vertex[i]->neighborTris.erase(it);
		}
	}
	for (i = 0; i < 3; i++) {
		int i2 = (i + 1) % 3;
		if (!vertex[i] || !vertex[i2]) continue;
	}
}

void Triangle::setNormal() {
	vec3 v0 = vertex[0]->pos;
	vec3 v1 = vertex[1]->pos;
	vec3 v2 = vertex[2]->pos;
	fnormal = cross((v1 - v0), (v2 - v1));
	if (fnormal.length() == 0)
		return;
	fnormal = normalize(fnormal);
}

bool Triangle::hasVertex(Vertex* v)
{
	for (auto vert : vertex)
	{
		if (v == vert) return true;
	}
	return false;
}

bool Triangle::hasVertex(unsigned id)
{
	for (auto vert : vertex)
	{
		if (id == vert->id) return true;
	}
	return false;
}

void Triangle::replaceVertex(Vertex* orig, Vertex* target)
{
	if (!hasVertex(orig)) return;
	for (auto& i : vertex)
	{
		if (i == orig)
		{
			i = target;
			target->neighborTris.push_back(this);
		}
		else
		{
			if (std::find(i->neighborVerts.begin(), i->neighborVerts.end(), target) == i->neighborVerts.end())
			{
				i->neighborVerts.push_back(target);
				target->neighborVerts.push_back(i);
			}
		}
	}
}

Vertex::Vertex(vec3 p, int index) {
	pos = p;
	id = index;
	neighborVerts.clear();
	neighborTris.clear();
	collapseTarget = nullptr;
}

Vertex::~Vertex() {
	//assert(neighborTris.empty());
	vector<Vertex*>::iterator it;
	vector<Vertex*> neighborsToBeRemoved;
	for (auto neighborVert : neighborVerts)
	{
		neighborsToBeRemoved.push_back(neighborVert);
	}
	for (unsigned int i = 0; i < neighborsToBeRemoved.size(); ++i)
	{
		it = find(neighborsToBeRemoved[i]->neighborVerts.begin(), neighborsToBeRemoved[i]->neighborVerts.end(), this);
		neighborsToBeRemoved[i]->neighborVerts.erase(it);
		it = find(neighborVerts.begin(), neighborVerts.end(), neighborsToBeRemoved[i]);
		neighborVerts.erase(it);
	}
}

bool Vertex::hasNeighbor(Vertex* v)
{
	if (v == NULL)
		return true;
	for (unsigned int i = 0; i < neighborVerts.size(); i++) {
		if (v == neighborVerts[i])
			return true;
	}
	return false;
}

bool Vertex::hasNeighbor(Triangle* t)
{
	if (t == NULL)
		return true;
	for (unsigned int i = 0; i < neighborTris.size(); i++) {
		if (t == neighborTris[i])
			return true;
	}
	return false;
}


void Vertex::addUniqueNeighbor(Vertex* v)
{
	if (!hasNeighbor(v)) {
		neighborVerts.push_back(v);
	}
}

void Vertex::addUniqueNeighbor(Triangle* t)
{
	if (!hasNeighbor(t)) {
		neighborTris.push_back(t);
	}
}

Mesh::Mesh()
{
	vertices.clear();
	triangles.clear();

	amb = diff = spec = vec3(0.5);
	shinness = 5.0f;

	viewMode = SHADED;
	nextCollapse = nullptr;
}

Mesh::~Mesh()
{

}

void Mesh::loadFromOBJFile(const char* filename) {

	ifstream fs(filename);

	char c;
	vec3 pos;
	int index[3];
	int vid = 0;

	std::string line;
	while (std::getline(fs, line)) {
		std::istringstream iss(line);

		iss >> c;

		switch (c) {
		case 'v':
		{
			// read a vertex
			iss >> pos.x;
			iss >> pos.y;
			iss >> pos.z;
			ori_vertices.push_back(pos);
			Vertex* v = new Vertex(pos, vid++);
			vertices.push_back(v);
			break;
		}
		case 'f':
		{
			// read a triangle's vertex indices
			iss >> index[0];
			iss >> index[1];
			iss >> index[2];
			// NOTE: index in obj files starts from 1
			ori_triangles.push_back(uvec3(index[0] - 1, index[1] - 1, index[2] - 1));
			break;
		}
		default:
			// skip the line

			break;
		}
	}
	fs.close();

	// form the triangles for mesh simplification
	for (unsigned int i = 0; i < ori_triangles.size(); i++) {
		Triangle* t = new Triangle(vertices[ori_triangles[i].x], vertices[ori_triangles[i].y], vertices[ori_triangles[i].z], i);
		triangles.push_back(t);
	}
}

void Mesh::draw() {

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	switch (viewMode) {

	case SHADED:
		drawShadedMesh();
	case WIREFRAME:
		drawWireframeMesh();
		break;

	default:
		break;
	}

	glPopMatrix();
}


void Mesh::drawShadedMesh()
{
	if (vertices.size() <= 0 && triangles.size() <= 0)
		return;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	glPolygonMode(GL_FRONT, GL_FILL);

	glMaterialfv(GL_FRONT, GL_AMBIENT, value_ptr(amb));
	glMaterialfv(GL_FRONT, GL_DIFFUSE, value_ptr(diff));
	glMaterialfv(GL_FRONT, GL_SPECULAR, value_ptr(spec));
	glMaterialf(GL_FRONT, GL_SHININESS, shinness);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	for (unsigned int i = 0; i < triangles.size(); i++) {
		glNormal3fv(value_ptr(triangles[i]->fnormal));
		glVertex3fv(value_ptr(triangles[i]->vertex[0]->pos));
		glVertex3fv(value_ptr(triangles[i]->vertex[1]->pos));
		glVertex3fv(value_ptr(triangles[i]->vertex[2]->pos));
	}
	glEnd();
	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	if (nextCollapse != nullptr && nextCollapse->collapseTarget != nullptr)
	{
		glLineWidth(10.0f);
		glBegin(GL_LINE_STRIP);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv(value_ptr(nextCollapse->pos));
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3fv(value_ptr(nextCollapse->collapseTarget->pos));
		glEnd();
		glLineWidth(1.0f);
	}

	glPopMatrix();
}


void Mesh::drawWireframeMesh()
{
	if (vertices.size() <= 0 && triangles.size() <= 0)
		return;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glColor3f(0.1f, 0.1f, 0.1f);
	glBegin(GL_TRIANGLES);
	for (unsigned int i = 0; i < triangles.size(); i++) {
		glNormal3fv(value_ptr(triangles[i]->fnormal));
		glVertex3fv(value_ptr(triangles[i]->vertex[0]->pos));
		glVertex3fv(value_ptr(triangles[i]->vertex[1]->pos));
		glVertex3fv(value_ptr(triangles[i]->vertex[2]->pos));
	}
	glEnd();

	if (nextCollapse != nullptr && nextCollapse->collapseTarget != nullptr)
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		glLineWidth(10.0f);
		glBegin(GL_LINE_STRIP);
		glVertex3fv(value_ptr(nextCollapse->pos));
		glVertex3fv(value_ptr(nextCollapse->collapseTarget->pos));
		glEnd();
		glLineWidth(1.0f);
	}

	glPopMatrix();
}

float ComputeEdgeCollapseCost(Vertex* u, Vertex* v) {
	// if we collapse edge uv by moving u to v then how
	// much different will the model change, i.e. the �error�.
	float edgelength = glm::distance(v->pos, u->pos);
	float curvature = 0;
	// find the �sides� triangles that are on the edge uv
	vector<Triangle*> sides;
	for (auto& neighborTri : u->neighborTris)
	{
		if (neighborTri->hasVertex(v)) {
			sides.push_back(neighborTri);
		}
	}
	if (sides.size() < 2)
	{
		return std::numeric_limits<float>::infinity();
	}
	// use the triangle facing most away from the sides
	// to determine our curvature term
	for (auto& neighborTri : u->neighborTris)
	{
		float mincurv = 1;
		for (auto& side : sides)
		{
			// use dot product of face normals.
			float dotprod =
				glm::dot(neighborTri->fnormal, side->fnormal);
			mincurv = min(mincurv, (1 - dotprod) / 2.0f);
		}
		curvature = max(curvature, mincurv);
	}
	return edgelength * curvature;
}

void ComputeEdgeCostAtVertex(Vertex* v) {
	v->collapseCost = std::numeric_limits<float>::infinity();
	v->collapseTarget = nullptr;
	if (v->neighborVerts.empty()) {
		return;
	}
	// search all neighboring edges for �least cost� edge
	for (int i = 0; i < v->neighborVerts.size(); i++) {
		float c = ComputeEdgeCollapseCost(v, v->neighborVerts[i]);
		if (c < v->collapseCost) {
			v->collapseTarget = v->neighborVerts[i];
			v->collapseCost = c;
		}
	}
}

void Mesh::calculateCollapseVertex()
{
	float cost = std::numeric_limits<float>::infinity();
	nextCollapse = nullptr;
	for (auto vert : vertices)
	{
		ComputeEdgeCostAtVertex(vert);

		if (vert->collapseCost < cost)
		{
			nextCollapse = vert;
			cost = vert->collapseCost;
		}
	}

	//Vertex* u = nextCollapse;
	//Vertex* v = nextCollapse->collapseTarget;
	//
}

bool Mesh::collapse()
{
	
	if (nextCollapse == nullptr || vertices.size() == 6)
	{
		std::cout << "Can't find a vertex to collapse." << std::endl;
		return false;
	}


	Vertex* u = nextCollapse;
	Vertex* v = nextCollapse->collapseTarget;
	
	vector<Triangle*> trisToDelete;
	// delete triangles on edge uv:
	for (int i = u->neighborTris.size() - 1; i >= 0; i--) {
		if (u->neighborTris[i]->hasVertex(v)) {
			trisToDelete.push_back(u->neighborTris[i]);
		}
	}

	for (auto& i : trisToDelete)
	{
		triangles.erase(std::find(triangles.begin(), triangles.end(), i));
		delete i;
	}

	// update remaining triangles to have v instead of u
	for (int i = u->neighborTris.size() - 1; i >= 0; i--) {
		u->neighborTris[i]->replaceVertex(u, v);
	}
	vertices.erase(std::find(vertices.begin(), vertices.end(), u));
	delete u;
	return true;
}
