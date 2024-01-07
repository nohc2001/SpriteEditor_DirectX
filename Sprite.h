#include "Utill_FreeMemory.h"
#include "ShapeObject.h"

class Object;
class InsideCode;
class ExecutableContext;

class Sprite {
public:
	union {
		fmvecarr<Object*>* objs;
		rbuffer* freepoly;
	};

	InsideCode* spric;

	Sprite() {}
	~Sprite() {}
};

class Object {
public:
	Sprite* Source;
	shp::vec3f pos;
	shp::vec3f rot;
	shp::vec3f sca;
	
	InsideCode* objic;
	fmvecarr<ExecutableContext*> ecs;

	Object(){}
	~Object(){}
};