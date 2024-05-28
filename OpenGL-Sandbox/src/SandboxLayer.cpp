#include "SandboxLayer.h"

using namespace GLCore;
using namespace GLCore::Utils;

SandboxLayer::SandboxLayer()
	: m_CameraController(16.0f / 9.0f)
{
}

SandboxLayer::~SandboxLayer()
{
}

void SandboxLayer::OnAttach()
{
	EnableGLDebugging();

	// Init here

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_Shader = Shader::FromGLSLTextFiles(
		"assets/shaders/test.glsl.vert",
		"assets/shaders/test.glsl.frag"
	);

	

	float vertices[] = {
		-1.5f, -0.5f, 0.0f, 0.24f, 0.16f, 0.26f, 1.0f,
		-0.5f, -0.5f, 0.0f,	0.4f, 0.26f, 0.46f, 1.0f,
		-0.5f,  0.5f, 0.0f,	0.84f, 0.36f, 0.56f, 1.0f,
		-1.5f,  0.5f, 0.0f,	0.94f, 0.46f, 0.76f, 1.0f,

		 0.5f, -0.5f, 0.0f,	0.1f, 0.23f, 0.1f, 1.0f,
		 1.5f, -0.5f, 0.0f,	0.2f, 0.53f, 0.3f, 1.0f,
		 1.5f,  0.5f, 0.0f,	0.6f, 0.73f, 0.5f, 1.0f,
		 0.5f,  0.5f, 0.0f, 0.9f, 0.83f, 0.6f, 1.0f
	};

	glCreateVertexArrays(1, &m_QuadVA);
	glBindVertexArray(m_QuadVA);

	glCreateBuffers(1, &m_QuadVB);
	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexArrayAttrib(m_QuadVB, 0); // 0 -> position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, 0);//first attrib position, second - number of 
									//floats per postion, fourth - stride - size of 1 vertex, fifth offset of vertex pos 
	
	glEnableVertexArrayAttrib(m_QuadVB, 1); // 1 -> color
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void*)12);//first attrib color, second - number of 
									//floats per vertex color, fourth - stride - size of 1 vertex, fifth offset of vertex color,
							//since it starts after 3 floats of vertex positon, therefore, 3 x 4 = 12

	uint32_t indices[] = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	};

	glCreateBuffers(1, &m_QuadIB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_QuadIB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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

void SandboxLayer::OnUpdate(Timestep ts)
{
	// Render here

	m_CameraController.OnUpdate(ts);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_Shader->GetRendererID());

	auto vp = m_CameraController.GetCamera().GetViewProjectionMatrix();
	SetUniformMat4(m_Shader->GetRendererID(), "u_ViewProj", vp);

	
	
	glBindVertexArray(m_QuadVA);
	SetUniformMat4(m_Shader->GetRendererID(), "u_Transform", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
	


}

void SandboxLayer::OnImGuiRender()
{
	// ImGui here
}
