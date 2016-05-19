#include <cmath>
#include <float.h>
#include <algorithm>
#include <assert.h>
#include "trimesh.h"
#include "../ui/TraceUI.h"
#include "../ui/GraphicalUI.h"
#include "../scene/material.h"

extern TraceUI* traceUI;
class GraphicalUI;

using namespace std;

Trimesh::~Trimesh()
{
	for( Materials::iterator i = materials.begin(); i != materials.end(); ++i )
		delete *i;
}

// must add vertices, normals, and materials IN ORDER
void Trimesh::addVertex( const Vec3d &v )
{
    vertices.push_back( v );
}

void Trimesh::addMaterial( Material *m )
{
    materials.push_back( m );
}

void Trimesh::addNormal( const Vec3d &n )
{
    normals.push_back( n );
}

// Returns false if the vertices a,b,c don't all exist
bool Trimesh::addFace( int a, int b, int c )
{
    int vcnt = vertices.size();

    if( a >= vcnt || b >= vcnt || c >= vcnt ) return false;

    TrimeshFace *newFace = new TrimeshFace( scene, new Material(*this->material), this, a, b, c );
    newFace->setTransform(this->transform);
    if (!newFace->degen) faces.push_back( newFace );


    // Don't add faces to the scene's object list so we can cull by bounding box
    // scene->add(newFace);
    return true;
}

char* Trimesh::doubleCheck()
// Check to make sure that if we have per-vertex materials or normals
// they are the right number.
{
    if( !materials.empty() && materials.size() != vertices.size() )
        return "Bad Trimesh: Wrong number of materials.";
    if( !normals.empty() && normals.size() != vertices.size() )
        return "Bad Trimesh: Wrong number of normals.";

    return 0;
}

bool Trimesh::intersectLocal(ray& r, isect& i) const
{
	double tmin = 0.0;
	double tmax = 0.0;
	typedef Faces::const_iterator iter;
	bool have_one = false;
	for( iter j = faces.begin(); j != faces.end(); ++j )
	  {
	    isect cur;
	    if( (*j)->intersectLocal( r, cur ) )
	      {
		if( !have_one || (cur.t < i.t) )
		  {
		    i = cur;
		    have_one = true;
		  }
	      }
	  }
	if( !have_one ) i.setT(1000.0);
	return have_one;
}

bool TrimeshFace::intersect(ray& r, isect& i) const {
  return intersectLocal(r, i);
}

// Intersect ray r with the triangle abc.  If it hits returns true,
// and put the parameter in t and the barycentric coordinates of the
// intersection in u (alpha) and v (beta).

bool TrimeshFace::intersectLocal(ray& r, isect& i) const
{
	const Vec3d& a = parent->vertices[ids[0]];
	const Vec3d& b = parent->vertices[ids[1]];
	const Vec3d& c = parent->vertices[ids[2]];
	//Muller-Trumbore
	Vec3d ab = b - a;
	Vec3d ac = c - a;
	Vec3d dir = r.getDirection();
	Vec3d pvec = dir^ac;
	double det = ab*pvec;
	if(traceUI->Backfacecull()){
		if(det<0) // face culling
		    return false;
	}
	if(fabs(det)<RAY_EPSILON) // parallel, no intersect
	    return false;
	double invDet = 1/det;
	
	Vec3d tvec = r.getPosition() - a;
	double u = (pvec*tvec)*invDet;
	if(u<0||u>1) return false;
	Vec3d qvec = tvec^ab;
	double v = (dir*qvec)*invDet;
	if(v<0||v>1) return false;
	if(1-u-v<0||1-u-v>1) return false;
	double t = (ac*qvec)*invDet;
	if(t<=RAY_EPSILON) return false; //make sure in front of you
	i.t = t;
	i.setUVCoordinates(Vec2d(u,v));
	//i.setBary(1-u-v,u,v);
	i.obj = this;
	if(!parent->materials.empty()){
		const Material* mat_a = parent->materials[ids[0]];
		const Material* mat_b = parent->materials[ids[1]];
		const Material* mat_c = parent->materials[ids[2]];
		Material itpl_mat;
		itpl_mat += (1-u-v)*(*mat_a);
		itpl_mat += u*(*mat_b);
		itpl_mat += v*(*mat_c);
		i.setMaterial(itpl_mat);
		//i.setMaterial(this->getMaterial());//not sure if material per object or per vertices
	}
	else
		i.setMaterial(this->getMaterial());//not sure if material per object or per vertices
	if((parent->normals.size()==parent->vertices.size())&&traceUI->smShadSw()){//interpolate normal
//		cout << "interpolate norm" << endl;
		const Vec3d & na = parent->normals[ids[0]];
		const Vec3d & nb = parent->normals[ids[1]];
		const Vec3d & nc = parent->normals[ids[2]];
		i.N = na*(1-u-v)+nb*u+nc*v;
		i.N.normalize();
	}
	else{	// use obj normal if parser didn't call
//		cout << "generate norm" << endl;
		i.N = ab^ac;
		i.N.normalize();
	}

	return true;	
}

void Trimesh::generateNormals()
// Once you've loaded all the verts and faces, we can generate per
// vertex normals by averaging the normals of the neighboring faces.
{
    //cout << "has the norm being generated default " << endl;
    int cnt = vertices.size();
    normals.resize( cnt );
    int *numFaces = new int[ cnt ]; // the number of faces assoc. with each vertex
    memset( numFaces, 0, sizeof(int)*cnt );
    
    for( Faces::iterator fi = faces.begin(); fi != faces.end(); ++fi )
    {
		Vec3d faceNormal = (**fi).getNormal();
        
        for( int i = 0; i < 3; ++i )
        {
            normals[(**fi)[i]] += faceNormal;
            ++numFaces[(**fi)[i]];
        }
    }

    for( int i = 0; i < cnt; ++i )
    {
        if( numFaces[i] )
            normals[i]  /= numFaces[i];
    }

    delete [] numFaces;
    vertNorms = true;
}

