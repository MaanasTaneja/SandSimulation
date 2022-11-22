#pragma once
#include <iostream>
#include <glad/glad.h>
#include <vector>

struct particle_t
{
	uint8_t m_type;
	float position[3];
	float velocity[3];
};

class ParticleBuffer
{
//All points are loaded into one vertex buffer and rendered.
//This wont work well, i just realied becuase to add new points ill have to recreate the entire buffer
//so thats like a lot of cycles and power needed and stuttering etc,
//Best way i think has to be the texture method
private:
	std::vector<float> m_rawposition;

	void order_memory();
	void load_to_memory();

public:
	std::vector<particle_t> m_particles;
	unsigned int m_vbo;
	unsigned int m_vao;
	std::vector<float> m_packed_data;

	ParticleBuffer() : m_vbo{ 0 }, m_vao{ 0 }, m_packed_data{}, m_rawposition{}, m_particles{} {}

	void send_to_buffer(std::vector<particle_t> particles);

	~ParticleBuffer() = default;
};

