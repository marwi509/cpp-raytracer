#include <vector>
#include <iostream>
#include <string>

#include "Vector3.hpp"
#include "Ray.hpp"

#include "Image.hpp"

#include "Material.hpp"
#include "Polygon.hpp"
#include "Vertex.hpp"
#include "Cell.hpp"
#include "Object.hpp"
#include "Light.hpp"
#include "RenderSettings.hpp"
#include "Scene.hpp"
#include "Check.hpp"
#include "Noise3.hpp"

using namespace std;

Object createBox() {
	Object theBox;
	Noise3* boxNoise = new Noise3(5,5,5,5);
	float x = 1, y = 1, z = 1;
	Vector3 scales = Vector3(13,10,26);
	
	/* Add the vertices for the box */
	theBox.addVertex(Vector3(x,-y,-z));
	theBox.addVertex(Vector3(x,-y,z));
	theBox.addVertex(Vector3(-x,-y,z));
	theBox.addVertex(Vector3(-x,-y,-z));
	
	theBox.addVertex(Vector3(-x,y,-z));
	theBox.addVertex(Vector3(-x,y,z));
	theBox.addVertex(Vector3(x,y,z));
	theBox.addVertex(Vector3(x,y,-z));
	
	theBox.addVertex(Vector3(-x * 0.9f,-y*0.4,z));
	theBox.addVertex(Vector3(x * 0.50f, -y*0.4,z));
	theBox.addVertex(Vector3(x * 0.50f,0,-z));
	theBox.addVertex(Vector3(-x * 0.9f,-y*0.4,-z));

	theBox.material = Material::DIFFUSE(Vector3(.75,.75,.75));
	theBox.material -> noise = boxNoise;
	theBox.material -> noiseStrength = 0.4f;
	
	/*theBox.material = new Material(Vector3(.75,.75,.75));
	theBox.material -> specularColor = Vector3(1,1,1);
	theBox.material -> reflective = 1.0f; */
	
	//theBox.material -> noise = boxNoise;
	//theBox.material -> noiseStrength = 0.1f;
	// Back
	theBox.addQuad(0,3,4,7);
	// Top
	
	theBox.material = Material::DIFFUSE(Vector3(.75,.75,.75));
	theBox.material -> noise = boxNoise;
	theBox.material -> noiseStrength = 0.4f;
	
	// Front
	theBox.addQuad(1,2,5,6);
	
	theBox.material = Material::DIFFUSE(Vector3(.75,.75,.75));
	theBox.material -> check = true;
	// Floor
	theBox.addQuad(0,1,2,3);
	
	theBox.material = Material::DIFFUSE(Vector3(.75,.25,.25));
	theBox.material -> brick = true;
	
	// Left
	theBox.addQuad(2,3,4,5);
	
	
	theBox.material = Material::DIFFUSE(Vector3(.25,.75,.25));
	theBox.material -> noise = boxNoise;
	theBox.material -> noiseStrength = 0.4f;
	//theBox.material -> brick = true;
	//theBox.material -> check = true;
	
	// Höger
	theBox.addQuad(0,1,6,7);
	
	theBox.material = Material::DIFFUSE(Vector3(.75,.75,.75));
	//top
	theBox.addQuad(4,5,6,7);
	
	theBox.scale(scales);
	theBox.getAAB(0);
	theBox.SMOOTH = false;
	theBox.USE_AAB = false;
	return theBox;
}

Object getStatue() {
	Object reflectiveStatue = Object("statue.obj", Material::DIFFUSE(Vector3(0.75, 0.75, 0.75)));
	
	/* Translate, scale and place on the floor */
	reflectiveStatue.rotate(1, M_PI / 2.0);
	reflectiveStatue.rotate(2, 0.3);
	//reflectiveStatue.rotate(3, 0.3);
	reflectiveStatue.translate(Vector3(-5.0,0.0,14.5));
	reflectiveStatue.scale(Vector3(0.045, 0.045, 0.045));
	reflectiveStatue.placeOnFloor(-10.0f);
	//O44.translate(Vector3(0.0,3.0,0.0));
	
	/* Create the kd-tree */
	reflectiveStatue.getAAB(24);
	reflectiveStatue.SMOOTH = false;
	reflectiveStatue.USE_AAB = true;
	//*/
	return reflectiveStatue;
}

Object getDragon() {
	//Object reflectiveStatue = Object("mirror.obj", Material::GLASS(Vector3(0.55, 0.95, 0.95)));
	Object reflectiveStatue = Object("mirror.obj", Material::PORCELAIN(Vector3(0.85, 0.55, 0.55)));
	
	reflectiveStatue.translate(Vector3(5.0,0.0,14.5));
	reflectiveStatue.scale(Vector3(15, 15, 15));
	reflectiveStatue.placeOnFloor(-10.0f);
	//O44.translate(Vector3(0.0,3.0,0.0));
	reflectiveStatue.rotate(2, -0.6);
	/* Create the kd-tree */
	//reflectiveStatue.getAAB(24);
	reflectiveStatue.SMOOTH = false;
	reflectiveStatue.USE_AAB = true;
	//*/
	return reflectiveStatue;
}

int main(int argc, char** argv)
{
	
	
	
	//Object reflectiveStatue = getStatue();
	
	Light L;//("box.obj", Vector3(10.0f, 10.0f, 10.0f));
	L.makeUnitBox();
	L.material -> diffuseColor = Vector3(1,1,1) * 150.0f;
	//L.translate(Vector3(9.5,-5.0,14.5));
	L.translate(Vector3(7.9f,9.97,10.0));
	L.scale(Vector3(1,0.01,15.97));
	L.normal = Vector3(0,1,0);
	
	L.getAAB(0);
	L.SMOOTH = false;
	L.USE_AAB = true;
	
	Light L2;//("box.obj", Vector3(10.0f, 10.0f, 10.0f));
	L2.makeUnitBox();
	L2.material -> diffuseColor = Vector3(1,1,1) * 150.0f;
	//L.translate(Vector3(9.5,-5.0,14.5));
	L2.translate(Vector3(-7.9f,9.97,10.0));
	L2.scale(Vector3(1,0.01,15.97));
	L2.normal = Vector3(0,1,0);
	
	L2.getAAB(0);
	L2.SMOOTH = false;
	L2.USE_AAB = true;
	
	/*Light L3;//("box.obj", Vector3(10.0f, 10.0f, 10.0f));
	L3.makeUnitBox();
	L3.material -> diffuseColor = Vector3(1,1,1) * 150.0f;
	//L.translate(Vector3(9.5,-5.0,14.5));
	L3.translate(Vector3(0.0f,0.0,-10.5));
	// 3
	L3.scale(Vector3(9,9,0.01));
	//L3.rotate(3, 3.14 / 2.0);
	L3.normal = Vector3(0,0,1);
	
	L3.getAAB(0);
	L3.SMOOTH = false;
	L3.USE_AAB = true;*/
	
	Object theBox = createBox();
	
	//Object theDragon2 = getDragon();
	//theDragon2.translate(Vector3(-10.0,0.0,-5.0));
	//Object reflectiveStatue = getStatue();
	/* Create a scene and add the objects */
	Scene test;
	//test.vObj.push_back(&O33);
	//test.vObj.push_back(&O44);
	//test.vObj.push_back(&reflectiveStatue);
//	Object theDragon = getStatue();
	//theDragon.getAAB(24);
//	test.vObj.push_back(&theDragon);
	
	Object theDragon2 = getDragon();
	theDragon2.getAAB(24);
	test.vObj.push_back(&theDragon2);
	//theDragon2.getAAB(24);
	
	
	//test.vObj.push_back(&theDragon2);
	test.vObj.push_back(&theBox);
	test.vLight.push_back(&L);
	test.vLight.push_back(&L2);
	
	string filename("statue-small-largelightsmirror2.ppm");
	//Vector3 pos, rad;
	//O33.findPosRadius(pos, rad);
	
	/* Render the scene with 2x2 multisampling, focus point at 11.0f units of length from the camera */
	//std::cout << reflectiveStatue.findClosestPosition() << std::endl;
	test.MSAA = 2;
	//float dof = 5.0;//reflectiveStatue.findClosestPosition();
	//test.renderScenePath(1024, 600, filename.c_str(), 14.0, 0.6f);
	test.renderScenePath(RenderSettings(1024, 600, 14.0, 0.15f), filename.c_str());
	
	
	return 0;
}
