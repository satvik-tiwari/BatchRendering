#include "SandboxLayer.h"
#include "stb_image/stb_image.h"
#include "Renderer.h"

using namespace GLCore;
using namespace GLCore::Utils;

/*
struct Vec2 {
	float x, y;
};

struct Vec3 {
	float x, y, z;
};

struct Vec4 {
	float x, y, z, w;
};

struct Vertex {
	Vec3 Position;
	Vec4 Color;
	Vec2 TexCoords;
	float TexID;
};*/

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
	auto* pixels = stbi_load(path.c_str(), &w, &h, &bits, 4);
	GLuint textureID;

	glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

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
	int samplers[32];   //ascending list from 0 to whatever your number of texture slots is
	for (int i = 0; i < 32; i++)
	{
		samplers[i] = i;
	}
	glUniform1iv(loc, 32, samplers);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	/*const size_t MaxQuadCount = 1000;
	const size_t MaxVertexCount = MaxQuadCount * 4;
	const size_t MaxIndexCount = MaxQuadCount * 6;
 */

	

	/*glCreateVertexArrays(1, &m_QuadVA);
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

	glCreateBuffers(1, &m_QuadIB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_QuadIB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	*/

	Renderer::Init();

	m_Tex1 = LoadTexture("assets/textures/Gojo.png");
	m_Tex2 = LoadTexture("assets/textures/Jogo2.png");
}

void SandboxLayer::OnDetach()
{
	// Shutdown here
	Renderer::ShutDown();
}

void SandboxLayer::OnEvent(Event& event)
{
	// Events here

	m_CameraController.OnEvent(event);

	if (event.GetEventType() == EventType::WindowResize)
	{
		WindowResizeEvent& e = (WindowResizeEvent&)event;
		glViewport(0, 0, e.GetWidth(), e.GetHeight());
	}
}

static void SetUniformMat4(uint32_t shader, const char* name, const glm::mat4& matrix)
{
	int loc = glGetUniformLocation(shader, name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
}

/*
static Vertex* CreateQuad(Vertex* target, float x, float y, float textureID)
{
	float size = 1.0f;

	target->Position = { x, y, 0.0f };
	target->Color = { 0.24f, 0.16f, 0.26f, 1.0f };
	target->TexCoords = { 0.0f, 0.0f };
	target->TexID = textureID;
	target++;

	target->Position = { x + size, y, 0.0f };
	target->Color = { 0.4f, 0.26f, 0.46f, 1.0f };
	target->TexCoords = { 1.0f, 0.0f };
	target->TexID = textureID;
	target++;

	target->Position = { x + size, y + size, 0.0f };
	target->Color = { 0.84f, 0.36f, 0.56f, 1.0f };
	target->TexCoords = { 1.0f, 1.0f };
	target->TexID = textureID;
	target++;

	target->Position = { x, y + size, 0.0f };
	target->Color = { 0.94f, 0.46f, 0.76f, 1.0f };
	target->TexCoords = { 0.0f, 1.0f };
    target->TexID = textureID;
	target++;
	
	return target;

}
*/

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


	/*auto q0 = CreateQuad(m_QuadPosition[0], m_QuadPosition[1], 0.0f);
	auto q1 = CreateQuad(0.5f, -0.5f, 1.0f);*/
    
	/*
	uint32_t indexCount = 0;

	std::array<Vertex, 1000> vertices;
	Vertex* buffer = vertices.data();
	for (int y = 0; y < 5; y++)
	{
		for (int x = 0; x < 5; x++)
		{
			buffer = CreateQuad(buffer, x, y, (x + y) % 2);
			indexCount += 6;
		}
	}

	buffer = CreateQuad(buffer, m_QuadPosition[0], m_QuadPosition[1], 0.0f);
	indexCount += 6;

	//std::cout << "Size : " << sizeof(buffer) / sizeof(Vertex) << std::endl;
	
	//Vertex vertices[8];  //8 vertices
	//memcpy(vertices, q0.data(), q0.size() * sizeof(Vertex)); // 4 vertex x size of Vertex
	//memcpy(vertices + q0.size(), q1.data(), q1.size() * sizeof(Vertex));

	//std::cout << "Size : " << vertices.size() << std::endl;
	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());//size of vertex buffer that we are sending in bytes
	*/

	m_CameraController.OnUpdate(ts);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(m_Shader->GetRendererID());

	auto vp = m_CameraController.GetCamera().GetViewProjectionMatrix();
	SetUniformMat4(m_Shader->GetRendererID(), "u_ViewProj", vp);
	SetUniformMat4(m_Shader->GetRendererID(), "u_Transform", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));

	Renderer::ResetStats();
	Renderer::BeginBatch();

	for (float y = -10.0f; y < 10.0f; y += 0.25)
	{
		for (float x = -10.0f; x < 10.0f; x += 0.25)
		{
			glm::vec4 color = { (x + 10) / 20.0f, 0.2f, (y + 10) / 20.0f, 1.0f };
			Renderer::DrawQuad({ x, y }, { 0.20f, 0.20f }, color);
		}
	}

	for (int y = 0; y < 5; y++)
	{
		for (int x = 0; x < 5; x++)
		{
			GLuint tex = (x + y) % 2 == 0 ? m_Tex1 : m_Tex2;
			Renderer::DrawQuad({ x, y }, { 1.0f, 1.0f }, tex);
		}
	}

	Renderer::DrawQuad(m_QuadPosition, { 1.0f, 1.0f }, m_Tex1);
	Renderer::EndBatch();

    /*glBindTextureUnit(0, m_Tex1);
	glBindTextureUnit(1, m_Tex2);*/

	

	Renderer::Flush();

	//glBindVertexArray(m_QuadVA);
	
	//glDrawElements(GL_TRIANGLES, , GL_UNSIGNED_INT, nullptr);

	/*GLint MaxTextureImageUnits;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MaxTextureImageUnits);   //to find number of texture slots	
	std::cout << MaxTextureImageUnits << std::endl;*/
}

void SandboxLayer::OnImGuiRender()
{
	// ImGui here

	ImGui::Begin("Controls");
	ImGui::DragFloat2("Quad Vertex Position", glm::value_ptr(m_QuadPosition), 0.1f);
	ImGui::End();
}
