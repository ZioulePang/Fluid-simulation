#include "Water.h"
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <iostream>
Water::Water( int width, int height, int density)
    : width(width), height(height), density(density)
{
    nodesPerRow = width * density;
    nodesPerColumn = height * density;

    for (int i = 0; i < nodesPerRow; i++)
        for (int j = 0; j < nodesPerColumn; j++)  CreateParticles(i, j);
}

void Water::CreateParticles(int rowIndex, int ColumnIndex)
{
    glm::vec2 position;
    position.x = (float)ColumnIndex / (float)density;
    position.y = (float)(-rowIndex) / (float)density;

    Particle* particles = new Particle(position);

    Particles.push_back(particles);
}

void Water::collisionTest(glm::vec2 bottom , glm::vec2 normal)
{
    for (Particle* p : Particles)
    {
        float distance =p->vertex.Position.y - bottom.y;
        glm::vec2 Vn = glm::dot(p->velocity, normal) * normal;
        glm::vec2 Vt = p->velocity - Vn;
        float phy = glm::dot(p->vertex.Position - bottom, normal);
        if (phy < 0)
        {
            p->vertex.Position += abs(phy) * normal *1.05f;
            if (glm::dot(p->velocity, normal) < 0)
            {
                float a = std::max(float(1 - 0.2* (1 + 0.25) * glm::length(Vn) / glm::length(Vt)), 0.0f);
                Vn = -1.0f * 0.25f * Vn;
                Vt = a * Vt * 0.25f;
                glm::vec2 Vnew = Vn + Vt;
                p->velocity = Vnew;

                if (glm::length(p->velocity) < 0.05f) p->velocity *= 4;
            }
        }
    }
}

void Water::BonderyTest(glm::vec2 bottom, glm::vec2 normal)
{
    for (Particle* p : Particles)
    {
        float distance = p->vertex.Position.x - bottom.x;
        glm::vec2 Vn = glm::dot(p->velocity, normal) * normal;
        glm::vec2 Vt = p->velocity - Vn;
        float phy = glm::dot(p->vertex.Position - bottom, normal);
        if (phy < 0)
        {
            p->vertex.Position += abs(phy) * normal;
            if (glm::dot(p->velocity, normal) < 0)
            {
                float a = std::max(float(1 - 0.2 * (1 + 0.2) * glm::length(Vn) / glm::length(Vt)), 0.0f);
                Vn = -1.0f * 0.25f * Vn;
                Vt = a * Vt * 0.2f;
                glm::vec2 Vnew = Vn + Vt;
                p->velocity = Vnew;
            }
        }
    }
}
