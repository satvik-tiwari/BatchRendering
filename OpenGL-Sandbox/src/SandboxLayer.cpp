#include "SandboxLayer.h"
#include "stb_image/stb_image.h"

using namespace GLCore;
using namespace GLCore::Utils;

struct Vertex
{
	float Position[3];
	float Color[4];
	float TexCoords[2];
	float TexID;
};

SandboxLayer::SandboxLayer()
	: m_CameraController(16.0f / 9.0f)
{

}

SandboxLayer::~SandboxLayer()
{
}

static GLuint LoadTexture(const std::string& path)
{
	int w, h, bits;

	stbi_set_flip_vertically_on_load(1);
	auto* pixels = stbi_load(path.c_str(), &w, &h, &bits, STBI_rgb);
	GLuint textureID;

	glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	stbi_image_free(pixels);

	return textureID;
}

void SandboxLayer::OnAttach()
{
	EnableGLDebugging();

	// Init here

	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_Shader = Shader::FromGLSLTextFiles(
		"assets/shaders/test.glsl.vert",
		"assets/shaders/test.glsl.frag"
	);

	glUseProgram(m_Shader->GetRendererID());
	auto loc = glGetUniformLocation(m_Shader->GetRendererID(), "u_Textures");
	int samplers[2] = { 0, 1 };   //ascending list from 0 to whatever your number of texture slots is
	glUniform1iv(loc, 2, samplers);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	const size_t MaxQuadCount = 1000;
	const size_t MaxVertexCount = MaxQuadCount * 4;
	const size_t MaxIndexCount = MaxQuadCount * 6;
 
	glCreateVertexArrays(1, &m_QuadVA);
	glBindVertexArray(m_QuadVA);

	glCreateBuffers(1, &m_QuadVB);
	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
	glBufferData(GL_ARRAY_BUFFER, MaxVertexCount * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexArrayAttrib(m_QuadVB, 0); // 0 -> position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position));//first attrib position, second - number of 
									//floats per postion, fourth - stride - size of 1 vertex, fifth offset of vertex pos 
	
	glEnableVertexArrayAttrib(m_QuadVB, 1); // 1 -> color
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Color));//first attrib color, second - number of 
									//floats per vertex color, fourth - stride - size of 1 vertex, fifth offset of vertex color,
							//since it starts after 3 floats of vertex positon, therefore, 3 x 4 = 12

	glEnableVertexArrayAttrib(m_QuadVB, 2); //texture coordinates
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexCoords));
	
	glEnableVertexArrayAttrib(m_QuadVB, 3); //texture coordinates
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexID));

	/*uint32_t indices[] = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	};*/

	uint32_t indices[MaxIndexCount];
	uint32_t offset = 0;
	for (size_t i = 0; i < MaxIndexCount; i += 6)
	{
		indices[i]     = offset + 0;
		indices[i + 1] = offset + 1;
		indices[i + 2] = offset + 2;

		indices[i + 3] = offset + 2;
		indices[i + 4] = offset + 3;
		indices[i + 5] = offset + 0;

		offset += 4;
 	}

	glCreateBuffers(1, &m_QuadIB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_QuadIB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	m_Tex1 = LoadTexture("assets/textures/Gojo1.png");
	m_Tex2 = LoadTexture("assets/textures/Jogo2.png");
}

void SandboxLayer::OnDetach()
{
	// Shutdown here

	glDeleteVertexArrays(1, &m_QuadVA);
	glDeleteBuffers(1, &m_QuadVB);
	glDeleteBuffers(1, &m_QuadIB);
}

void SandboxLayer::OnEvent(Event& event)
{
	// Events here

	m_CameraController.OnEvent(event);
}

static void SetUniformMat4(uint32_t shader, const char* name, const glm::mat4& matrix)
{
	int loc = glGetUniformLocation(shader, name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
}

static std::array<Vertex, 4> CreateQuad(float x, float y, float textureID)
{
	float size = 1.0f;

	Vertex v0 = { { x, y, 0.0f },
				 { 0.24f, 0.16f, 0.26f, 1.0f },
				 { 0.0f, 0.0f },
				 textureID };

	Vertex v1 = { { x + size, y, 0.0f },
				  { 0.4f, 0.26f, 0.46f, 1.0f },
				  { 1.0f, 0.0f },
				  textureID };

	Vertex v2 = { { x + size, y + size, 0.0f },
				  { 0.84f, 0.36f, 0.56f, 1.0f },
				  { 1.0f, 1.0f },
				  textureID };

	Vertex v3 = { { x, y + size, 0.0f },
				  { 0.94f, 0.46f, 0.76f, 1.0f },
				  { 0.0f, 1.0f },
				  textureID };
	
	return { v0, v1, v2, v3 };

}

static int count = 0;

void SandboxLayer::OnUpdate(Timestep ts)
{
	// Render here

	//dynamic buffer

	/*float vertices[] = {
		-1.5f, -0.5f, 0.0f, 0.24f, 0.16f, 0.26f, 1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,	0.4f,  0.26f, 0.46f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,	0.84f, 0.36f, 0.56f, 1.0f, 1.0f, 1.0f, 0.0f,
		-1.5f,  0.5f, 0.0f,	0.94f, 0.46f, 0.76f, 1.0f, 0.0f, 1.0f, 0.0f,

		 0.5f, -0.5f, 0.0f,	0.2f,  0.53f, 0.3f,  1.0f, 0.0f, 0.0f, 1.0f,
		 1.5f, -0.5f, 0.0f,	0.6f,  0.73f, 0.5f,  1.0f, 1.0f, 0.0f, 1.0f,
		 1.5f,  0.5f, 0.0f,	0.1f,  0.23f, 0.1f,  1.0f, 1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f, 0.0f, 0.9f,  0.83f, 0.6f,  1.0f, 0.0f, 1.0f, 1.0f
	};*/

	indices[] = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	};

	auto q0 = CreateQuad(m_QuadPosition[0], m_QuadPosition[1], 0.0f);
	auto q1 = CreateQuad(0.5f, -0.5f, 1.0f);

	Vertex vertices[8];  //8 vertices
	memcpy(vertices, q0.data(), q0.size() * sizeof(Vertex)); // 4 vertex x size of Vertex
	memcpy(vertices + q0.size(), q1.data(), q1.size() * sizeof(Vertex));

	indices = 

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);


	m_CameraController.OnUpdate(ts);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);


	glUseProgram(m_Shader->GetRendererID());
    glBindTextureUnit(0, m_Tex1);
	glBindTextureUnit(1, m_Tex2);

	auto vp = m_CameraController.GetCamera().GetViewProjectionMatrix();
	SetUniformMat4(m_Shader->GetRendererID(), "u_ViewProj", vp);
	SetUniformMat4(m_Shader->GetRendererID(), "u_Transform", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));

	glBindVertexArray(m_QuadVA);
	
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
	
}

void SandboxLayer::OnImGuiRender()
{
	// ImGui here

	ImGui::Begin("Controls");
	ImGui::DragFloat2("Quad Vertex Position", m_QuadPosition, 0.1f);
	ImGui::End();
}
