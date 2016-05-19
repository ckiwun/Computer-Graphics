#include <cmath>
#include <assert.h>

#include "scene.h"
#include "light.h"
#include "../ui/TraceUI.h"
#include "../SceneObjects/trimesh.h"

using namespace std;

bool Geometry::intersect(ray& r, isect& i) const {
	double tmin, tmax;
	if (hasBoundingBoxCapability() && !(bounds.intersect(r, tmin, tmax))) return false;
	// Transform the ray into the object's local coordinate space
	Vec3d pos = transform->globalToLocalCoords(r.p);
	Vec3d dir = transform->globalToLocalCoords(r.p + r.d) - pos;
	double length = dir.length();
	dir /= length;
	Vec3d Wpos = r.p;
	Vec3d Wdir = r.d;
	r.p = pos;
	r.d = dir;
	bool rtrn = false;
	if (intersectLocal(r, i))
	{
		// Transform the intersection point & normal returned back into global space.
		i.N = transform->localToGlobalCoordsNormal(i.N);
		i.t /= length;
		rtrn = true;
	}
	r.p = Wpos;
	r.d = Wdir;
	return rtrn;
}

bool Geometry::hasBoundingBoxCapability() const {
	// by default, primitives do not have to specify a bounding box.
	// If this method returns true for a primitive, then either the ComputeBoundingBox() or
    // the ComputeLocalBoundingBox() method must be implemented.

	// If no bounding box capability is supported for an object, that object will
	// be checked against every single ray drawn.  This should be avoided whenever possible,
	// but this possibility exists so that new primitives will not have to have bounding
	// boxes implemented for them.
	return false;
}

Scene::~Scene() {
    giter g;
    liter l;
    tmap::iterator t;
    for( g = objects.begin(); g != objects.end(); ++g ) delete (*g);
    for( l = lights.begin(); l != lights.end(); ++l ) delete (*l);
    for( t = textureCache.begin(); t != textureCache.end(); t++ ) delete (*t).second;
}

// Get any intersection with an object.  Return information about the 
// intersection through the reference parameter.
bool Scene::intersect(ray& r, isect& i) const {
	double tmin = 0.0;
	double tmax = 0.0;
	bool have_one = false;
	typedef vector<Geometry*>::const_iterator iter;
	for(iter j = objects.begin(); j != objects.end(); ++j) {
		isect cur;
		if( (*j)->intersect(r, cur) ) {
			if(!have_one || (cur.t < i.t)) {
				i = cur;
				have_one = true;
			}
		}
	}
	if(!have_one) i.setT(1000.0);
	// if debugging,
	if (TraceUI::m_debug) intersectCache.push_back(std::make_pair(new ray(r), new isect(i)));
	return have_one;
}

TextureMap* Scene::getTexture(string name) {
	tmap::const_iterator itr = textureCache.find(name);
	if(itr == textureCache.end()) {
		textureCache[name] = new TextureMap(name);
		return textureCache[name];
	} else return (*itr).second;
}


void Scene::buildKdTree(unsigned int d, unsigned int lsize) {
	kdtree = new KdNode<Geometry>(sceneBounds,d-1,lsize);
	vector<Geometry*> deconstr_boundedobjects;
	for(int i=0;i<boundedobjects.size();i++){
		if(dynamic_cast<Trimesh*>(boundedobjects[i])){
			//cout<<"this is trimesh\n";
			auto trimesh_ptr = dynamic_cast<Trimesh*>(boundedobjects[i]);
			for(int j=0;j<trimesh_ptr->faces.size();j++){
				deconstr_boundedobjects.push_back(trimesh_ptr->faces[j]);
			}
		}
		else{
			//cout<<"this is not trimesh\n";
			deconstr_boundedobjects.push_back(boundedobjects[i]);
		}
		//cout << "type of obj no."<< i <<" is "<< typeid(boundedobjects[i]).name() <<endl;
	}
	for(int i=0;i<deconstr_boundedobjects.size();i++)
		deconstr_boundedobjects[i]->ComputeBoundingBox();
	
	//cout << "objects size is:" << deconstr_boundedobjects.size() << endl;
	kdtree->build(deconstr_boundedobjects);
}

//template <typename T>
//void KdNode<T>::build(std::vector<T*>& objs){
//	//cout << "build KdTree\n";
//	//cout << "objects size is:" << objs.size() << endl;
//	left = right = nullptr;
//	if(objs.size()<target_lsize) return;
//	int sample=1;
//	//while(objs.size()/sample>10) sample++;//sample 10 objects
//	int rand_start = rand()%sample;
//	vector<T*> left_objs,right_objs;
//	double volume = bbox.volume();
//	Vec3d bbox_min = bbox.getMin();
//	Vec3d bbox_max = bbox.getMax();
//	objects = objs;
//	//cout << "depth: " << max_depth << "bbox min " << bbox_min << "bbox max " << bbox_max << endl;
//	//cout << "objects size is:" << objs.size() << endl;
//	BoundingBox bbox_a,bbox_b;
//	double cost=-1;
//	for(int i=0;i<objs.size();i++){
//		//cout << "object number " << i << endl;
//		Vec3d bmin = objs[i]->getBoundingBox().getMin();//localbox min/max
//		Vec3d bmax = objs[i]->getBoundingBox().getMax();
//		double local_cost,objvol_a,objvol_b,num_a,num_b;
//		for(int j=0;j<3;j++){
//			if(bmin[j]<bbox_min[j]||bmin[j]>bbox_max[j]) continue;
//			objvol_a = objvol_b = num_a = num_b = 0;
//			Vec3d psplit = bbox_max;
//			psplit[j] = bmin[j];
//			box_a.setMin(bbox_min);
//			box_a.setMax(psplit);
//			if(box_a.volume()==0) continue;
//			psplit = bbox_min;
//			psplit[j] = bmin[j];
//			box_b.setMin(psplit);
//			box_b.setMax(bbox_max);
//			if(box_b.volume()==0) continue;
//			//need sorting
//			for(int k=0;k<objs.size();k++){
//				if(objs[k]->getBoundingBox().intersects(box_a)){
//					num_a++;
//					objvol_a += objs[k]->getBoundingBox().volume();//assume not overlap severely now
//				}
//				if(objs[k]->getBoundingBox().intersects(box_b)){
//					num_a++;
//					objvol_b += objs[k]->getBoundingBox().volume();
//				}
//			}
//			local_cost = 1+((objvol_a/box_a.volume())*num_a+(objvol_b/box_b.volume())*num_b)*80;
//			if(cost!=-1){
//				if(local_cost<cost){
//					cost = local_cost;
//					bbox_a = box_a;
//					bbox_b = box_b;
//				}
//			}
//			else{
//				cost = local_cost;
//				bbox_a = box_a;
//				bbox_b = box_b;
//			}
//		}
//		for(int j=0;j<3;j++){
//			if(bmin[j]<bbox_min[j]||bmin[j]>bbox_max[j]) continue;
//			objvol_a = objvol_b = num_a = num_b = 0;
//			Vec3d psplit = bbox_max;
//			psplit[j] = bmax[j];
//			box_a.setMin(bbox_min);
//			box_a.setMax(psplit);
//			if(box_a.volume()==0) continue;
//			psplit = bbox_min;
//			psplit[j] = bmax[j];
//			box_b.setMin(psplit);
//			box_b.setMax(bbox_max);
//			if(box_b.volume()==0) continue;
//			for(int k=0;k<objs.size();k++){
//				if(objs[k]->getBoundingBox().intersects(box_a)){
//					num_a++;
//					objvol_a += objs[k]->getBoundingBox().volume();
//				}
//				if(objs[k]->getBoundingBox().intersects(box_b)){
//					num_b++;
//					objvol_b += objs[k]->getBoundingBox().volume();
//				}
//			}
//			local_cost = 1+((objvol_a/box_a.volume())*num_a+(objvol_b/box_b.volume())*num_b)*80;
//			if(local_cost<cost){
//				cost = local_cost;
//				bbox_a = box_a;
//				bbox_b = box_b;
//			}
//		}
//	}
//	for(int i=0;i<objs.size();i++){
//		if(objs[i]->getBoundingBox().intersects(bbox_a))
//			left_objs.push_back(objs[i]);
//		if(objs[i]->getBoundingBox().intersects(bbox_b))
//			right_objs.push_back(objs[i]);
//	}
//	//if(left_objs.empty() && !right_objs.empty()) {left_objs = right_objs; bbox_a = bbox_b; cout<< "split on boundary" << endl; return;}
//	//if(right_objs.empty() && !left_objs.empty()) {right_objs = left_objs; bbox_b = bbox_a; cout<< "split on boundary" << endl; return;}
//	//cout << "depth: " << max_depth << " final cost is " << cost << endl;
//	//cout << "build left node with box min " << bbox_a.getMin() << " max " << bbox_a.getMax() << endl;
//	//cout << "build right node with box min " << bbox_b.getMin() << " max " << bbox_b.getMax() << endl;
//	int matches = left_objs.size()+right_objs.size()-objs.size();
//	//cout << left_objs.size() << endl;
//	//cout << right_objs.size() << endl;
//	//cout << objs.size() << endl;
//	//if((double)left_objs.size()/(double)objs.size()>0.9||(double)left_objs.size()/(double)objs.size()<0.1) return;//if parent has almost every objs, just return
//	//if((double)right_objs.size()/(double)objs.size()>0.9||(double)right_objs.size()/(double)objs.size()<0.1) return;
//	if(left_objs.size()==objs.size()) return;
//	if(right_objs.size()==objs.size()) return;
//	if((double)matches/(double)objs.size()>0.5) return;
//	if(max_depth>0){
//		cout << "depth: "<<max_depth << " build left node with objs " << left_objs.size() << endl;
//		left = new KdNode<T>(bbox_a,max_depth-1,target_lsize);
//		left->build(left_objs);
//		
//		cout << "depth: "<<max_depth << " build right node with objs " << right_objs.size() << endl;
//		right = new KdNode<T>(bbox_b,max_depth-1,target_lsize);
//		right->build(right_objs);
//	}
//}

//use midpoint
template <typename T>
void KdNode<T>::build(std::vector<T*>& objs){
	//cout << "build KdTree\n";
	//cout << "objects size is:" << objs.size() << endl;
	left = right = nullptr;
	objects = objs;
	if(objs.size()<=target_lsize) return;
	vector<T*> left_objs,right_objs;
	//cout << "depth: " << max_depth << "bbox min " << bbox.getMin() << "bbox max " << bbox.getMax() << endl;
	BoundingBox bbox_a,bbox_b;
	Vec3d midpoint;
	int main_axis=0;
	double wx = bbox.getMax()[0]-bbox.getMin()[0];
	double wy = bbox.getMax()[1]-bbox.getMin()[1];
	double wz = bbox.getMax()[2]-bbox.getMin()[2];
	if(wy>wz&&wy>wx) main_axis=1;
	if(wz>wy&&wz>wx) main_axis=2;
	int objsize= objs.size();
	for(int i=0;i<objsize;i++){
		midpoint += (objs[i]->getBoundingBox().getMin()+objs[i]->getBoundingBox().getMax())/(2*(double)objsize);
	}
	Vec3d psplit;
	psplit = bbox.getMax();
	psplit[main_axis]=midpoint[main_axis];
	bbox_a.setMin(bbox.getMin());
	bbox_a.setMax(psplit);
	psplit = bbox.getMin();
	psplit[main_axis]=midpoint[main_axis];
	bbox_b.setMin(psplit);
	bbox_b.setMax(bbox.getMax());
	if(bbox_a.volume()==bbox.volume()) return;
	if(bbox_b.volume()==bbox.volume()) return;
	//cout << "box a " << bbox_a.getMin() << "; " <<bbox_a.getMax() << endl;
	//cout << "box b " << bbox_b.getMin() << "; " <<bbox_b.getMax() << endl;
	for(int i=0;i<objs.size();i++){
		//Vec3d objmid = (objs[i]->getBoundingBox().getMin()+objs[i]->getBoundingBox().getMax())/2.0;
		//if(objmid[main_axis]>=midpoint[main_axis])
		//	left_objs.push_back(objs[i]);
		//else
		//	right_objs.push_back(objs[i]);
		if(objs[i]->getBoundingBox().intersects(bbox_a))
			left_objs.push_back(objs[i]);
		if(objs[i]->getBoundingBox().intersects(bbox_b))
			right_objs.push_back(objs[i]);
	}
	//for(int i=0;i<left_objs.size();i++)
	//	bbox_a.merge(left_objs[i]->getBoundingBox());
	//for(int i=0;i<right_objs.size();i++)
	//	bbox_b.merge(right_objs[i]->getBoundingBox());
	int matches = left_objs.size()+right_objs.size()-objs.size();
	if(left_objs.size()==objs.size()) return;
	if(right_objs.size()==objs.size()) return;
	if((double)matches/(double)objs.size()>0.6) return;
	if(max_depth>0){
		objects.clear();
		//cout << "depth: "<<max_depth << " build left node with objs " << left_objs.size() << endl;
		left = new KdNode<T>(bbox_a,max_depth-1,target_lsize);
		left->build(left_objs);
		
		//cout << "depth: "<<max_depth << " build right node with objs " << right_objs.size() << endl;
		right = new KdNode<T>(bbox_b,max_depth-1,target_lsize);
		right->build(right_objs);
	}
}

template <typename T>
bool KdNode<T>::hit(ray& r,isect& i,double& tmin,double& tmax){
	//cout << "traverse depth = " << max_depth << endl;
	//cout << "objects size is " << objects.size() << endl;
	if(left==nullptr&&right==nullptr){ //it's a leaf node
		//cout << "objects size is " << objects.size() << endl;
		//cout << "tmin/max is " << tmin << "," << tmax << endl;
		// check objects intersection in this node
		bool have_one = false;
		//typedef vector<Geometry*>::const_iterator iter;
		//for(iter j = objects.begin(); j != objects.end(); ++j) {
		for(int j=0;j<objects.size();j++){
			isect cur;
			if( objects[j]->intersect(r, cur) ) {
				if((!have_one || (cur.t < i.t))&&cur.t-RAY_EPSILON<=tmax&&cur.t+RAY_EPSILON>=tmin) {
					//cout << "hit objs, t = "<<cur.t << endl;
					i = cur;
					have_one = true;
				}
			}
		}
		//if(!have_one) i.setT(1000.0);//not hit anything
		//if(have_one) cout << i.material->kd(i) << endl;
		//cout << boolalpha << have_one << endl;
		return have_one;
	}
	//cout << "left box min/max is: "<<left->bbox.getMin() << "; " << left->bbox.getMax()<<endl;
	//cout << "right box min/max is: "<<right->bbox.getMin() << "; " << right->bbox.getMax()<<endl;
	double ta,tb,tc,td;
	bool hit_left = left->bbox.intersect(r,ta,tb);
	bool hit_right = right->bbox.intersect(r,tc,td);
	if(ta<0) ta=0;
	if(tc<0) tc=0;
	if(hit_left&&hit_right){
		//cout << "hit both node \n";
		if(fabs(tb-tc)<RAY_EPSILON){//left is near
			assert(td>tc&&td>tb&&tb>ta);
			if(left->hit(r,i,ta,tb)) return true;
			//cout << "didn't hit near" << endl;
			if(right->hit(r,i,tc,td)) return true;
			//cout << "didn't hit far" << endl;
		}
		else if(fabs(ta-td)<RAY_EPSILON){//right is near
			assert(td>tc&&tb>td&&tb>ta);
			if(right->hit(r,i,tc,td)) return true;
			//cout << "didn't hit near" << endl;
			if(left->hit(r,i,ta,tb)) return true;
			//cout << "didn't hit far" << endl;
		}
		else{
			//cout << "corner case\n" << ta << ", " << tb << endl;
			//cout << "ray dir is " << r.d << endl;
			r.d+=Vec3d(0.001,0.001,0.001);
			if(left->hit(r,i,ta,tb)) return true;
			if(right->hit(r,i,tc,td)) return true;
			//return false;
		}
	}
	else if(hit_left){
		//cout << "hit left node \n";
		if(left->hit(r,i,ta,tb)) return true;
	}
	else if(hit_right){
		//cout << "hit right node \n";
		if(right->hit(r,i,tc,td)) return true;
	}
	else {
		//cout<<"didn't hit anything\n";
		//return false;
	}
	
	return false;
}

bool Scene::kd_intersect(ray& r,isect& i) const{
	double tmin,tmax;
	kdtree->bbox.intersect(r,tmin,tmax);
	return kdtree->hit(r,i,tmin,tmax);
}
