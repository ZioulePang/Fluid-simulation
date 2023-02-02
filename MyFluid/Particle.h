
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct Vertex
{
	glm::vec2 Position;
};
class Particle
{
public:
	// physics
	float mass;
	int index;
	glm::vec2 velocity;
	glm::vec2 force;

	glm::vec2 oldPos;
	Vertex vertex;
	float density;
	Particle(glm::vec2 position);
	void Simulate(float timeStamp);
};
