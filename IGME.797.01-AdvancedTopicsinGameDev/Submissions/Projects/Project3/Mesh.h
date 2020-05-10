#ifndef MESH_H
#define MESH_H

#define GLM_FORCE_RADIANS

#include <vector>
#include <set>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtx/constants.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/quaternion.hpp>
#include <ostream>

using namespace std;
using namespace glm;

enum ViewMode {SHADED, WIREFRAME};

class Triangle;
class Vertex;
class Mesh;

class Triangle
{
public:
	Vertex*		 vertex[3];		// the three vertices of this triangle
	vec3		 fnormal;		// face normal 
	unsigned int id;			// index in original triangle set

	Triangle(Vertex* v0, Vertex* v1, Vertex* v2, int index);
	~Triangle();
	void setNormal();
	bool hasVertex(Vertex* v);
	bool hasVertex(unsigned int id);
	void replaceVertex(Vertex* orig, Vertex* target);
};

class Vertex 
{
public:
	vec3				pos;			// position xyz of this vertex 
	unsigned int		id;				// index in orginal vertex set
	vector<Vertex*>		neighborVerts;	// adjacent vertices 
	vector<Triangle*>	neighborTris;	// adjacent triangles
	
	Vertex(vec3 p, int index);
	~Vertex();
	
	bool hasNeighbor(Vertex* v);
	bool hasNeighbor(Triangle* t);

	void addUniqueNeighbor(Vertex* v);
	void addUniqueNeighbor(Triangle* t);

	Vertex* collapseTarget;
	float collapseCost;
};

std::ostream& operator<<(std::ostream& os, Vertex* vert);

std::ostream& operator<<(std::ostream& os, Triangle* trig);

class Mesh
{
public:		
	/**mesh data: loaded from the file as arrays, not used for mesh simplification**/
	vector<vec3> ori_vertices;
	vector<uvec3> ori_triangles;

	/** mesh color **/
	vec3 amb, diff, spec;
	float shinness;

	ViewMode viewMode;

	/** data for mesh simplification, formed based on the loaded original data **/
	vector<Vertex*> vertices;
	vector<Triangle*> triangles;
	Vertex* nextCollapse;

public:
    Mesh();
    ~Mesh();

    /* Load the mesh from an '.obj' file. All faces need to be triangles.
	   Attributes like edge lengths and curvatures should be computed when simplifying the mesh.
    */
    void loadFromOBJFile (const char* filename);

    void draw();

	/** mutiple drawing calls **/
	void drawShadedMesh();		// draw shaded mesh and can be used to draw inteermediate versions of the mesh
	void drawWireframeMesh();	// draw the mesh in wireframe and can be used to draw intermediate versions of the mesh
	void calculateCollapseVertex();
	bool collapse();
};

#endif
