#include "Renderer.h"

#include <GLCore.h>
#include <array>
#include <Glad\glad.h>

static const size_t MaxQuadCount = 1000;
static const size_t MaxVertexCount = MaxQuadCount * 4;
static const size_t MaxIndexCount = MaxQuadCount * 6;
static const size_t MaxTextures = 32;

struct Vertex
{
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec2 TexCoords;
	float TexIndex;
};

struct RendererData
{
	//ids for vertex array, vertex buffer and index buffer
	GLuint QuadVA = 0;
	GLuint QuadVB = 0;
	GLuint QuadIB = 0;

	GLuint WhiteTexture = 0;
	uint32_t WhiteTextureSlot = 0;

	uint32_t IndexCount = 0; //how many indices to be drawn when we flush

	Vertex* QuadBuffer = nullptr; //pointer to the beginning of buffer data on cpu
	Vertex* QuadBufferPtr = nullptr; //trackeing pointer as to where we are when copying data into buffer

	std::array<uint32_t, MaxTextures> TextureSlots;
	uint32_t TextureSlotIndex = 1;

	Renderer::Stats RendererStats;
};

static RendererData s_Data; //static data

void Renderer::Init()
{
	s_Data.QuadBuffer = new Vertex[MaxVertexCount];

	glCreateVertexArrays(1, &s_Data.QuadVA);
	glBindVertexArray(s_Data.QuadVA);

	glCreateBuffers(1, &s_Data.QuadVB);
	glBindBuffer(GL_ARRAY_BUFFER, s_Data.QuadVB);
	glBufferData(GL_ARRAY_BUFFER, MaxVertexCount, nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexArrayAttrib(s_Data.QuadVA, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position));

	glEnableVertexArrayAttrib(s_Data.QuadVA, 1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Color));

	glEnableVertexArrayAttrib(s_Data.QuadVA, 2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexCoords));

	glEnableVertexArrayAttrib(s_Data.QuadVA, 3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexIndex));

	uint32_t indices[MaxIndexCount];
	uint32_t offset = 0;
	for (size_t i = 0; i < MaxIndexCount; i += 6)
	{
		indices[i + 0] = offset + 0;
		indices[i + 1] = offset + 1;
		indices[i + 2] = offset + 2;

		indices[i + 3] = offset + 2;
		indices[i + 4] = offset + 3;
		indices[i + 5] = offset + 0;

		offset += 4;
	}

	glCreateBuffers(1, &s_Data.QuadIB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data.QuadIB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER)

}
void Renderer::ShutDown()
{
}

void Renderer::BeginBatch()
{
}

void Renderer::EndBatch()
{
}

void Renderer::Flush()
{
}

void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
{
}

void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, uint32_t textureID)
{
}

const Renderer::Stats& Renderer::GetStats()
{
	// TODO: insert return statement here
}

void Renderer::ResetStats()
{
}
