#include "Particle.h"
#include <iostream>
#include <cstdlib>


Particle::Particle(glm::vec2 position) : mass(1.0), velocity(0.0),force(0),density(0)
{
    vertex.Position = position;
    oldPos = position;
    //mass = 1 + ((rand() % 10)/10);
}

void Particle::Simulate(float timeStamp)
{
    velocity += force / mass * timeStamp;

    glm::vec2 temp = vertex.Position;
    vertex.Position += 0.5f * (force / mass) * timeStamp * timeStamp + velocity * timeStamp;
    //0.5f * (force / mass) * timeStamp * timeStamp
    oldPos = temp;
    density = 0;
    force = glm::vec2(0);
}