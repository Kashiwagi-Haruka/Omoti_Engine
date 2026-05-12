#pragma once
#include <memory>
#include "Mesh/Primitive/Primitive.h"
class Adhesion {

	std::unique_ptr<Primitive> preAttributePlane_;
	std::unique_ptr<Primitive> AttributePlane_;

	public: 

		Adhesion();
	    ~Adhesion();
	    void Initialize();
	    void Update();
	    void Draw();

};
