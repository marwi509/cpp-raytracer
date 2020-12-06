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
#include "Window.h"

#include "include/SDL.h"

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
	//theBox.material -> noise = boxNoise;
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
	theBox.material -> noiseStrength = 0.5f;
	
	// Front
	theBox.addQuad(1,2,5,6);
	
	theBox.material = Material::DIFFUSE(Vector3(.75,.75,.75));
	theBox.material -> check = true;
	// Floor
	theBox.addQuad(0,1,2,3);
	
	//theBox.material = Material::DIFFUSE(Vector3(.75,.25,.25));
	theBox.material = Material::DIFFUSE(Vector3(.75, .25, .25));
	theBox.material -> brick = true;
	
	// Left
	theBox.addQuad(2,3,4,5);
	
	
	//theBox.material = Material::DIFFUSE(Vector3(.25,.75,.25));
	theBox.material = Material::DIFFUSE(Vector3(.25, .75, .15));
	theBox.material -> noise = boxNoise;
	theBox.material -> noiseStrength = 0.5f;
	//theBox.material -> brick = true;
	//theBox.material -> check = true;
	
	// Höger
	theBox.addQuad(0,1,6,7);
	
	theBox.material = Material::DIFFUSE(Vector3(.75,.75,.75));
	theBox.material->noise = boxNoise;
	theBox.material->noiseStrength = 0.5f;
	//top
	theBox.addQuad(4,5,6,7);
	
	theBox.scale(scales);
	theBox.getAAB(0);
	theBox.SMOOTH = false;
	theBox.USE_AAB = false;
	return theBox;
}

Object createCheckeredFloor() {
	Object theBox;
	Noise3* boxNoise = new Noise3(5, 5, 5, 5);
	float x = 1, y = 1, z = 1;
	Vector3 scales = Vector3(1300, 10, 2600);

	/* Add the vertices for the box */
	theBox.addVertex(Vector3(x, -y, -z));
	theBox.addVertex(Vector3(x, -y, z));
	theBox.addVertex(Vector3(-x, -y, z));
	theBox.addVertex(Vector3(-x, -y, -z));



	theBox.material = Material::DIFFUSE(Vector3(.75, .75, .75));
	theBox.material->check = true;
	// Floor
	theBox.addQuad(0, 1, 2, 3);


	theBox.scale(scales);
	theBox.getAAB(0);
	theBox.SMOOTH = false;
	theBox.USE_AAB = false;
	return theBox;
}

Object getStatue() {
	Object reflectiveStatue = Object("statue.obj", Material::DIFFUSE(Vector3(0.85, 0.85, 0.85)));
	
	/* Translate, scale and place on the floor */
	reflectiveStatue.rotate(1, 3.14 / 2.0);
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

Object getDragon(Material* material) {
	//Object reflectiveStatue = Object("mirror.obj", Material::GLASS(Vector3(0.55, 0.95, 0.95)));
	Object reflectiveStatue = Object("mirror.obj", material);
	
	reflectiveStatue.translate(Vector3(-6.0,0.0,16.5));
	reflectiveStatue.scale(Vector3(10, 10, 10));
	reflectiveStatue.placeOnFloor(-10.0f);
	//O44.translate(Vector3(0.0,3.0,0.0));
	reflectiveStatue.rotate(2, 0.6);
	/* Create the kd-tree */
	//reflectiveStatue.getAAB(24);
	reflectiveStatue.SMOOTH = false;
	reflectiveStatue.USE_AAB = true;
	//*/
	return reflectiveStatue;
}

Object* getAjax() {
	
	//Object* ajax = new Object("ajax.obj", Material::PORCELAIN(Vector3(0.75f, 0.55f, 0.55f)));
	Object* ajax = new Object("ajax.obj", Material::GOLD());
	ajax->translate(Vector3(-7, 0, 20));
	ajax->rotate(2, -2.0 / 2);
	ajax->placeOnFloor(-10.0f);
	ajax->scale(Vector3(0.4f, 0.4f, 0.4f));
	ajax->USE_AAB = true;
	return ajax;
}


int main(int argc, char** argv)
{

	
	
	cout << "RUNNING" << endl;
	//Object reflectiveStatue = getStatue();
	
	Light L;//("box.obj", Vector3(10.0f, 10.0f, 10.0f));
	L.makeUnitBox();
	L.material -> diffuseColor = Vector3(1,1,1) * 450.0f * 1.0f;
	//L.translate(Vector3(9.5,-5.0,14.5));
	L.translate(Vector3(1.9f,9.97,10.0));
	L.scale(Vector3(1,0.01,1) * 2.0f);
	L.normal = Vector3(0,1,0);
	L.material->light = true;


	
	L.getAAB(0);
	L.SMOOTH = false;
	L.USE_AAB = true;
	
	
	cout << "LET THERE BE LIGHT" << endl;
	
	Object theBox = createBox();
	//Object theBox = createCheckeredFloor();
	
	cout << "LET THERE BE BOX" << endl;

	//Object theDragon2 = getDragon(Material::GLASS(Vector3(0.95f, .85f, .85f)));
	/*Object theDragon2 = getDragon(Material::GLASS(Vector3(0.85f, 0.85f, 0.95f)));

	theDragon2.translate(Vector3(0.0,0.0,-5.0));
	theDragon2.getAAB(24);

	Object theDragon3 = getDragon(Material::GOLD());

	theDragon3.translate(Vector3(13.0, 0.0, -5.0));
	theDragon3.rotate(2, 2.5);
	theDragon3.getAAB(24);
	*/

	//Object theDragon3 = getDragon(Material::GOLD());
	Object theDragon3 = getDragon(Material::GLASS(Vector3(0.85f, 0.95f, 0.85f)));
	
	theDragon3.translate(Vector3(13.0, 0.0, -10.0));
	theDragon3.rotate(2, 2.5);
	theDragon3.getAAB(24);

	Object* ajax = getAjax();
	ajax->getAAB(24);

	Scene test;
	

	
	Object redGlassBox;
	//redGlassBox.material = Material::DIFFUSE(Vector3(0.75, 0.75, 0.75));
	redGlassBox.material = Material::GLASS(Vector3(0.95f, 0.85f, 0.85f));
	//redGlassBox.material = Material::MIRROR();
	redGlassBox.makeUnitBox();
	redGlassBox.scale(Vector3(3.0f, 6.0f, 3.0f));
	redGlassBox.placeOnFloor(-10.0f);
	redGlassBox.translate(Vector3(5.0f, 0.0f, 20.0f));
	//redGlassBox.material = Material::MIRROR();
	redGlassBox.getAAB(0);
	
	

	Object top;
	top.material = Material::DIFFUSE(Vector3(0.75, 0.75, 0.5));
	//redGlassBox.material = Material::GLASS(Vector3(0.95f, 0.85f, 0.85f));
	top.makeUnitBox();
	top.rotate(2, 1.0);
	top.scale(Vector3(4.0f, 1.0f, 4.0f));
	top.placeOnFloor(2.0f);
	top.translate(Vector3(5.0f, 0.0f, 20.0f));
	//redGlassBox.material = Material::MIRROR();
	top.getAAB(0);

	

	Light floorTile;
	
	//redGlassBox.material = Material::GLASS(Vector3(0.95f, 0.85f, 0.85f));
	floorTile.makeUnitBox();
	floorTile.rotate(2, 0.7);
	floorTile.scale(Vector3(2.0f, 4.0f, 2.0f));
	floorTile.placeOnFloor(-10.0f);
	floorTile.translate(Vector3(0.0f, 0.0f, 0.0f));
	//redGlassBox.material = Material::MIRROR();
	floorTile.material = Material::DIFFUSE(Vector3(5.0f, 5.0f, 5.0f));
	floorTile.getAAB(0);

	

	Object greenGlassBox;
	redGlassBox.material = Material::MIRROR();
	//greenGlassBox.material = Material::DIFFUSE(Vector3(0.85f, 0.75f, 0.75f));
	greenGlassBox.makeUnitBox();
	//greenGlassBox.rotate(2, 1.0);
	greenGlassBox.scale(Vector3(3.0f, 6.0f, 3.0f));
	greenGlassBox.placeOnFloor(-10.0f);
	greenGlassBox.translate(Vector3(15.0f, 0.0f, 20.0f));

	greenGlassBox.getAAB(0);

	

	Object greenSolidBox;
	//redGlassBox.material = Material::MIRROR(Vector3(0.85f, 0.75f, 0.75f));
	greenSolidBox.material = Material::DIFFUSE(Vector3(0.85f, 0.95f, 0.85f));
	greenSolidBox.makeUnitBox();
	//greenGlassBox.rotate(2, 1.0);
	greenSolidBox.scale(Vector3(3.0f, 6.0f, 3.0f));
	greenSolidBox.placeOnFloor(-10.0f);
	greenSolidBox.translate(Vector3(-15.0f, 0.0f, 10.0f));

	greenSolidBox.getAAB(0);

	/*test.vObj.push_back(&redGlassBox);
	test.vObj.push_back(&top);
	test.vLight.push_back(&floorTile);
	test.vObj.push_back(&greenGlassBox);
	test.vObj.push_back(&greenSolidBox);*/


	
	test.vObj.push_back(&theBox);
	test.vObj.push_back(&theDragon3);
	test.vObj.push_back(ajax);
	test.vLight.push_back(&L);
	//test.vLight.push_back(&L2);
	
	string filename("statue-small-largelightsmirror2.ppm");
	//Vector3 pos, rad;
	//O33.findPosRadius(pos, rad);
	
	/* Render the scene with 2x2 multisampling, focus point at 11.0f units of length from the camera */
	//std::cout << reflectiveStatue.findClosestPosition() << std::endl;
	//float dof = 5.0;//reflectiveStatue.findClosestPosition();
	//test.renderScenePath(1024, 600, filename.c_str(), 14.0, 0.6f);
	int msaa = 8;
	//RenderSettings settings = RenderSettings(1024, 600, msaa, 10.0f, 0.15f, Vector3(0, -5, 5));
	RenderSettings settings = RenderSettings(1024, 600, msaa, 17.0f, 3.15f, Vector3(-5, 5, -5));
	settings.rotateCamera(1, 0.5);
	settings.rotateCamera(2, -0.5);
	test.renderSceneNew(settings, false);
	//test.renderSceneOpen(settings);
	//test.renderScenePath(RenderSettings(1024, 600, 14.0, 1.0f), "filename");

	globalWindow->destroy();

	return 0;
}


/*
//Window::initiate(1024, 600);
	/*SDL_Event event;
	SDL_Renderer *renderer;
	SDL_Window *window;
	int i;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(600, 600, 0, &window, &renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	for (i = 0; i < 600; ++i)
		SDL_RenderDrawPoint(renderer, i, i);
	SDL_RenderPresent(renderer);
	while (1) {
		if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
			break;
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();


cout << "RUNNING" << endl;
//Object reflectiveStatue = getStatue();

Light L;//("box.obj", Vector3(10.0f, 10.0f, 10.0f));
L.makeUnitBox();
L.material->diffuseColor = Vector3(1, 1, 1) * 450.0f * 1.0f;
//L.translate(Vector3(9.5,-5.0,14.5));
L.translate(Vector3(1.9f, 9.97, 10.0));
L.scale(Vector3(1, 0.01, 1) * 2.0f);
L.normal = Vector3(0, 1, 0);
L.material->light = true;



L.getAAB(0);
L.SMOOTH = false;
L.USE_AAB = true;

/*Light L2;//("box.obj", Vector3(10.0f, 10.0f, 10.0f));
L2.makeUnitBox();
L2.material -> diffuseColor = Vector3(1,1,1) * 150.0f * 8.0f * 1000.0f;
//L.translate(Vector3(9.5,-5.0,14.5));
L2.translate(Vector3(-7.9f,9.97,10.0));
L2.scale(Vector3(1,0.01,2.97) * 0.05f);
L2.normal = Vector3(0,1,0);

L2.getAAB(0);
L2.SMOOTH = false;
L2.USE_AAB = true;

cout << "LET THERE BE LIGHT" << endl;

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
L3.USE_AAB = true;

Object theBox = createBox();

cout << "LET THERE BE BOX" << endl;

//Object theDragon2 = getDragon(Material::GLASS(Vector3(0.95f, .85f, .85f)));
Object theDragon2 = getDragon(Material::GLASS(Vector3(0.85f, 0.85f, 0.95f)));

theDragon2.translate(Vector3(0.0, 0.0, -5.0));
theDragon2.getAAB(24);

Object theDragon3 = getDragon(Material::GOLD());

theDragon3.translate(Vector3(13.0, 0.0, -5.0));
theDragon3.rotate(2, 2.5);
//theDragon3.placeOnFloor(-10.0f);
theDragon3.getAAB(24);
//Object reflectiveStatue = getStatue();
/* Create a scene and add the objects 
Scene test;
//test.vObj.push_back(&O33);
//test.vObj.push_back(&O44);
//test.vObj.push_back(&reflectiveStatue);
//	Object theDragon = getStatue();
	//theDragon.getAAB(24);
//	test.vObj.push_back(&theDragon);

	//Object theDragon2 = getStatue();
cout << "LET THERE BE AABB" << endl;
//theDragon2.getAAB(24);
//test.vObj.push_back(&theDragon2);
//theDragon2.getAAB(24);


cout << "LET THERE BE DRAGON" << endl;

test.vObj.push_back(&theDragon2);
test.vObj.push_back(&theDragon3);
test.vObj.push_back(&theBox);
test.vLight.push_back(&L);
//test.vLight.push_back(&L2);

string filename("statue-small-largelightsmirror2.ppm");
//Vector3 pos, rad;
//O33.findPosRadius(pos, rad);

/* Render the scene with 2x2 multisampling, focus point at 11.0f units of length from the camera 
//std::cout << reflectiveStatue.findClosestPosition() << std::endl;
//float dof = 5.0;//reflectiveStatue.findClosestPosition();
//test.renderScenePath(1024, 600, filename.c_str(), 14.0, 0.6f);
int msaa = 16;
RenderSettings settings = RenderSettings(1024, 600, msaa, 6.0, 0.15f, Vector3(0, -9, 5));
settings.rotateCamera(1, -0.5);
test.renderSceneNew(settings);
//test.renderScenePath(RenderSettings(1024, 600, 14.0, 1.0f), "filename");

globalWindow->destroy();

return 0;
*/