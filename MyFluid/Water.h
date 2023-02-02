#include "Particle.h"
#include <vector>
#include <math.h>
#include <iostream>
#define PI acos(-1)

struct Space
{
	float left;
	float right;
	float bottom;
	float top;

	std::vector<Particle*> p;
};
class Water
{
public:
	int width, height;
	int nodesPerRow, nodesPerColumn;
	int density;

	std::vector<Particle*> Particles;
	std::vector<Vertex> vertices;
	std::vector<Space> s;
	std::vector<int> tags;
	glm::vec2 gravity = glm::vec2(0, -5.98);

	float h = 2;
	float h_surface = 1.3;
	float h_press = 2.4;
	float h_vis = 2;
	float k1 = 4.3;
	float u = 0.62;
	float miga = 0.6;

	float W_first = 315.0f / (64.0f * PI * pow(h, 9));
	float W_spiky = 15 / (PI * pow(h_press, 6));
	float W_viscosity = 15.0f /2.0f / (PI * pow(h_vis, 3));
	float W_secondDirect_surface = 945.0f / 8.0f / PI / pow(h, 9);

private:
	void CreateParticles(int rowIndex, int ColumnIndex);

public:
	Water(int width, int height,int density);

	// simulation functions
	inline void SimulateParticles(float timeStamp) { for (Particle* p : Particles) p->Simulate(timeStamp); }

	void init(){ for (Particle* p : Particles) vertices.push_back(p->vertex); }
	void SimulateGravity(float timestep) { for (Particle* p : Particles) p->force += (gravity * p->mass); }

	void collisionTest(glm::vec2 bottom, glm::vec2 normal);

	void BonderyTest(glm::vec2 bottom, glm::vec2 normal);

	void SPHsimulation();

	void caculateSpace(float left, float right, float bottom, float top, float density)
	{
		float row = std::abs(right - left);
		float column = std::abs(top - bottom);

		float lengthPerRow = row / density;
		float lengthPerColumn = column / density;
		int totalSpace = density * density;

		for (int i = 0; i < density; i++)
		{
			float tempRow = left, tempColumn = bottom;
			for (int j = 0; j < density; j++)
			{
				Space temp_s;
				temp_s.left = tempRow + j * lengthPerRow;
				temp_s.right = tempRow + (j + 1) * lengthPerRow;
				temp_s.bottom = tempColumn + i * lengthPerColumn;
				temp_s.top = tempColumn + (i + 1) * lengthPerColumn;

				s.push_back(temp_s);
			}
		}

	}

	void perSpace() 
	{
		for (int i = 0; i < s.size(); i++)
		{
			s[i].p.clear();
			for (Particle* p : Particles)
			{
				if ((p->vertex.Position.x<s[i].right && p->vertex.Position.x > s[i].left)
					&& (p->vertex.Position.y<s[i].top && p->vertex.Position.y > s[i].bottom))
				{
					s[i].p.push_back(p);
				}
			}
		}
	}

	void caculateDensity() 
	{
		float W_des = 0;
		for (Particle* p1 : Particles)
		{
			float length = 0;
			for (Particle* p2 : Particles)
			{
				length = glm::length(p1->vertex.Position - p2->vertex.Position);

				float density_p1 = 0;
				if (p1 != p2)
				{
					float W_end = 0;
					//density
					if (length <= 0.25 && length >= 0)
					{
						W_end = pow(h * h - length * length, 3);
						W_des = W_first * W_end;
						p1->density += W_des;
					}
				}
			}
		}
	}

	void caculateForce_Poly() 
	{
		float W_des = 0;
		for (Particle* p1 : Particles)
		{
			float length = 0;
			for (Particle* p2 : Particles)
			{
				length = glm::length(p1->vertex.Position - p2->vertex.Position);
				float density_p1 = 0;
				if (p1 != p2)
				{
					float W_end = 0;
					//density
					if (length <= 0.16 && length>=0)
					{
						W_end = pow(h * h - length * length, 3);
						W_des = W_first * W_end;
						p1->density += W_des;
					}
				}
			}
		}
		for (Particle* p1 : Particles)
		{
			float W_total = 0,W_vis = 0;
			glm::vec2 Force_pressure = glm::vec2(0), Force_viscosity = glm::vec2(0);
			for (Particle* p2 : Particles)
			{
				float length = glm::length(p1->vertex.Position - p2->vertex.Position);
				//pressure
				if (p1 != p2)
				{
					if (length <= 0.16 && length >= 0)
					{
						float pressure_01 = k1 * (p1->density - 0.5);
						float pressure_02 = k1 * (p2->density - 0.5);

						Force_pressure = p2->mass * ((pressure_01 + pressure_02) / 2.0f / p2->density) * W_des * glm::normalize(p1->vertex.Position - p2->vertex.Position);
						Force_viscosity = -1 * u * p2->mass * ((p2->velocity - p1->velocity) / p2->density) * W_des * 0.1f;
						p1->force += Force_pressure + Force_viscosity;

					}

				}
			}
		}

	}

	void caculateForce_Spiky()
	{
		float W_total = 0;
		for (Particle* p1 : Particles)
		{
			float length = 0;
			for (Particle* p2 : Particles)
			{
				length = glm::length(p1->vertex.Position - p2->vertex.Position);
				float density_p1 = 0;
				if (p1 != p2)
				{
					float W_end = 0;
					//density
					if (length <= 0.16 && length >= 0)
					{
						W_total = W_spiky * pow((h_press - length), 3);
						p1->density += W_total;
					}
				}
			}
		}
		for (Particle* p1 : Particles)
		{
			float W_total = 0, W_vis = 0;
			glm::vec2 Force_pressure = glm::vec2(0), Force_viscosity = glm::vec2(0);
			for (Particle* p2 : Particles)
			{
				float length = glm::length(p1->vertex.Position - p2->vertex.Position);
				//pressure
				if (p1 != p2)
				{
					if (length <= 0.16 && length >= 0)
					{
						float W_s = W_spiky * pow((h_press - length), 3);

						float pressure_01 = k1 * (p1->density - 0.5);
						float pressure_02 = k1 * (p2->density - 0.5);

						Force_pressure = p2->mass * ((pressure_01 + pressure_02) / 2.0f / p2->density) * W_s * glm::normalize(p1->vertex.Position - p2->vertex.Position) * 0.5f;
						Force_viscosity = -1.0f * u * p2->mass * ((p2->velocity - p1->velocity) / p2->density) * W_s * 0.1f;
						p1->force += Force_pressure + Force_viscosity;

					}

				}
			}
		}

	}

	void caculateForce_Vis()
	{
		float W_total = 0;
		for (Particle* p1 : Particles)
		{
			float length = 0;
			for (Particle* p2 : Particles)
			{
				length = glm::length(p1->vertex.Position - p2->vertex.Position);
				float density_p1 = 0;
				if (p1 != p2)
				{
					float W_end = 0;
					//density
					if (length <= 0.16 && length >= 0)
					{
						float a = W_viscosity*pow((h_press - length), 3);
						W_total = a ;
						p1->density += W_total;
					}
				}
			}
		}
		for (Particle* p1 : Particles)
		{
			float W_total = 0, W_vis = 0;
			glm::vec2 Force_pressure = glm::vec2(0), Force_viscosity = glm::vec2(0);
			for (Particle* p2 : Particles)
			{
				float length = glm::length(p1->vertex.Position - p2->vertex.Position);
				//pressure
				if (p1 != p2)
				{
					if (length <= 0.16 && length >= 0)
					{
						float second_deritive = 45.0f / (PI * pow(h_vis, 6));
						float b = second_deritive * (h_vis - length);
						W_vis = b ;

						float pressure_01 = k1 * (p1->density - 0.5);
						float pressure_02 = k1 * (p2->density - 0.5);

						Force_pressure = p2->mass * ((pressure_01 + pressure_02) / 2.0f / p2->density) * W_vis * glm::normalize(p1->vertex.Position - p2->vertex.Position) * 0.3f;
						Force_viscosity = -1.0f * u * p2->mass * ((p2->velocity - p1->velocity) / p2->density) * W_vis;
						p1->force += Force_pressure + Force_viscosity;

					}

				}
			}
		}

	}

	void caculateForce_Summary()
	{
		float W_des = 0;
		for (Particle* p1 : Particles)
		{
			float W_total = 0, W_vis = 0;
			glm::vec2 Force_pressure = glm::vec2(0), Force_viscosity = glm::vec2(0);
			for (Particle* p2 : Particles)
			{
				float length = glm::length(p1->vertex.Position - p2->vertex.Position);
				//pressure
				if (p1 != p2)
				{
					if (length <= 0.16 && length >= 0)
					{
						W_total = W_spiky * pow((h_press - length), 3);
						float a = (45.0f / PI / pow(h_vis, 6)) * (h_vis - length);
						W_vis = a;

						float pressure_01 = k1 * (p1->density - 0.5);
						float pressure_02 = k1 * (p2->density - 0.5);

						Force_pressure = p2->mass * ((pressure_01 + pressure_02) / 2.0f / p2->density) * W_total * glm::normalize(p1->vertex.Position - p2->vertex.Position) ;
						Force_viscosity = -1.0f * u * p2->mass * ((p2->velocity - p1->velocity) / p2->density) * W_vis;
						p1->force += Force_pressure + Force_viscosity;

					}

				}
			}
		}

	}

	void caculateForce_SummaryWithSurface(glm::vec2 bottom)
	{

		for (Particle* p1 : Particles)
		{
			float W_total = 0, W_vis = 0,W_surface = 0;
			glm::vec2 Force_pressure = glm::vec2(0), Force_viscosity = glm::vec2(0), Force_surface = glm::vec2(0);

			float normal_length = glm::length(bottom - p1->vertex.Position);
			glm::vec2 normal = (bottom - p1->vertex.Position) / normal_length;

			for (Particle* p2 : Particles)
			{
				float length = glm::length(p1->vertex.Position - p2->vertex.Position);
				//pressure & viscosity
				if (p1 != p2)
				{
					if (length <= 0.2 && length >= 0)
					{
						W_total = W_spiky * pow((h_press - length), 3);
						float a = (45.0f / PI / pow(h_vis, 6)) * (h_vis - length);
						W_vis = a ;
						W_surface = miga * W_first * (6.0f * pow(h_surface * h_surface - length * length, 3) + 36.0f * h_surface * h_surface * (h_surface * h_surface - length * length));

						float pressure_01 = k1 * (p1->density - 0.5);
						float pressure_02 = k1 * (p2->density - 0.5);


						Force_pressure = p2->mass * ((pressure_01 + pressure_02) / 2.0f / p2->density) * W_total * glm::normalize(p1->vertex.Position - p2->vertex.Position) * 0.6f;
						Force_viscosity = -1.0f * u * p2->mass * ((p2->velocity - p1->velocity) / p2->density) * W_vis;

						p1->force += Force_pressure + Force_viscosity;

						float temp = pow(h * h - length * length, 3);
						glm::vec2 color = glm::normalize(p1->vertex.Position - p2->vertex.Position) * p2->mass * (1.0f / p2->density) * W_first * temp;
						float color_length = glm::length(color);
						glm::vec2 ll = -1.0f * miga * glm::normalize(color) * W_secondDirect_surface * p2->mass * (1.0f / p2->density) * (h * h - length * length) * (length * length - (3.0f * (h * h - length * length)) / 4.0f);
						if (glm::length(p1->vertex.Position - bottom) <= 0.33 && glm::length(p1->vertex.Position - bottom) >= 0.19f)
						{
							Force_surface = ll;
						}
						p1->force += Force_surface;

					}

				}
			}
		}

	}

};