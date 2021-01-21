#include "SimpleScene.h"

#include <vector>
#include <iostream>

#include "CameraInput.h"
#include "SceneInput.h"

#include <Core/Engine.h>
#include <Component/Transform/Transform.h>

using namespace std;
using namespace EngineComponents;

SimpleScene::SimpleScene()
{
	InitResources();
}

SimpleScene::~SimpleScene()
{
}

void SimpleScene::InitResources()
{
	// sets common GL states
	glClearColor(0, 0, 0, 1);

	drawGroundPlane = true;

	objectModel = new Transform();

	camera = new Camera();
	camera->SetPerspective(60, window->props.aspectRatio, 0.01f, 200);
	camera->transform->SetMoveSpeed(2);
	camera->transform->SetWorldPosition(glm::vec3(0, 1.6f, 2.5));
	camera->transform->SetWorldRotation(glm::vec3(-15, 0, 0));
	camera->Update();

	cameraInput = nullptr;
	window = Engine::GetWindow();

	SceneInput *SI = new SceneInput(this);

	// Default rendering mode will use depth buffer
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}

void SimpleScene::AddMeshToList(Mesh * mesh)
{
	if (mesh->GetMeshID())
	{
		meshes[mesh->GetMeshID()] = mesh;
	}
}

void SimpleScene::DrawCoordinatSystem()
{
	DrawCoordinatSystem(camera->GetViewMatrix(), camera->GetProjectionMatrix());
}

void SimpleScene::DrawCoordinatSystem(const glm::mat4 & viewMatrix, const glm::mat4 & projectionMaxtix)
{
}

void SimpleScene::RenderMesh(Mesh * mesh, Shader * shader, glm::vec3 position, glm::vec3 scale)
{
	if (!mesh || !shader || !shader->program)
		return;

	// render an object using the specified shader and the specified position
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

	glm::mat4 model(1);
	model = glm::translate(model, position);
	model = glm::scale(model, scale);
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(model));
	mesh->Render();
}

void SimpleScene::RenderMesh(Mesh * mesh, glm::vec3 position, glm::vec3 scale)
{
}

void SimpleScene::RenderMesh2D(Mesh * mesh, Shader * shader, const glm::mat3 &modelMatrix)
{
	if (!mesh || !shader || !shader->program)
		return;

	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
	
	glm::mat3 mm = modelMatrix;
	glm::mat4 model = glm::mat4(
		mm[0][0], mm[0][1], mm[0][2], 0.f,
		mm[1][0], mm[1][1], mm[1][2], 0.f,
		0.f, 0.f, mm[2][2], 0.f,
		mm[2][0], mm[2][1], 0.f, 1.f);

	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(model));
	mesh->Render();
}

void SimpleScene::RenderMesh2D(Mesh * mesh, const glm::mat3 & modelMatrix, const glm::vec3 & color) const
{
	
}

void SimpleScene::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
	if (!mesh || !shader || !shader->program)
		return;

	// render an object using the specified shader and the specified position
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	mesh->Render();
}

void SimpleScene::ReloadShaders() const
{
	cout << endl;
	cout << "=============================" << endl;
	cout << "Reloading Shaders" << endl;
	cout << "=============================" << endl;
	cout << endl;

	for (auto &shader : shaders)
	{
		shader.second->Reload();
	}
}

Camera * SimpleScene::GetSceneCamera() const
{
	return camera;
}

InputController * SimpleScene::GetCameraInput() const
{
	return cameraInput;
}

