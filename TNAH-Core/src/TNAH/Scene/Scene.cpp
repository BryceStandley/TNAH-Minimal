#include <tnahpch.h>
#include "Scene.h"
#include "GameObject.h"
#include "Components/Components.h"
#include "TNAH/Core/Application.h"
#include "TNAH/Core/Input.h"
#include "TNAH/Renderer/Renderer.h"
#include <glm/gtx/string_cast.hpp>


namespace tnah{

#pragma region SceneSetups
	
	Scene::ActiveScene Scene::s_ActiveScene = Scene::ActiveScene();

	struct SceneComponent
	{
		UUID SceneID;
	};
	
	std::map<UUID, GameObject>& Scene::GetGameObjectsInScene()
	{
		return m_GameObjectsInScene;
	}

	GameObject* Scene::GetRefGameObject(const UUID& id)
	{
		return &m_GameObjectsInScene[id];
	}

	Scene::Scene(bool editor)
	{
		m_SceneEntity = m_Registry.create();
		m_Registry.emplace<SceneComponent>(m_SceneEntity, m_SceneID);
		if(editor)
		{
			m_EditorCamera = CreateEditorCamera().GetUUID();
			FramebufferSpecification fbspec = {1280, 720};
			m_EditorSceneFramebuffer = Framebuffer::Create(fbspec);
			m_EditorGameFramebuffer = Framebuffer::Create(fbspec);
			m_IsEditorScene = true;
			m_RenderPasses = 2;
		}

		auto& cam = CreateGameObject("Main Camera");
		m_ActiveCamera = cam.GetUUID();
		auto& camera = cam.AddComponent<CameraComponent>();
		Camera::SetMainCamera(camera.Camera);
		cam.AddComponent<SkyboxComponent>();
		
		
		auto& l = CreateGameObject("Main Light");
		m_SceneLight = l.GetUUID();
		auto& light = l.AddComponent<LightComponent>();
		light.Light = Light::CreateDirectional();
		light.Light->m_IsSceneLight = true;
		
		s_ActiveScene.Scene.Reset(this);
	}

	Scene::~Scene()
	{
		m_Registry.clear();
		m_GameObjectsInScene.clear();
		//s_ActiveScenes.erase(m_SceneID);
		m_EditorGameFramebuffer.Reset();
		m_EditorSceneFramebuffer.Reset();

	}

	void Scene::ClearActiveScene()
	{
		s_ActiveScene.Scene.Reset();
	}

	Ref<Scene> Scene::CreateNewEditorScene()
	{
		return Ref<Scene>::Create(true);
	}

	GameObject& Scene::GetEditorCamera()
	{
		return m_GameObjectsInScene[m_EditorCamera];
	}

	Ref<Scene> Scene::CreateEmptyScene()
	{
		return Ref<Scene>::Create();
	}

#pragma endregion SceneSetups

#pragma region SceneUpdate
	
	void Scene::OnUpdate(Timestep deltaTime)
	{
		
		
#pragma region PlayerControllerUpdate
		//TODO: Actually test the player controller component
		// Process any PlayerControllers before updating anything else in the scene
		{
			auto e = EditorComponent();
			
			auto view = m_Registry.view<PlayerControllerComponent, TransformComponent>();
			for(auto obj : view)
			{
				auto& editor = m_GameObjectsInScene[m_EditorCamera].GetComponent<EditorComponent>();
				//Only run this update for a player controller if the editor isnt empty, were in play mode to test the scene
				// or this scene isnt being ran in a editor ie in the runtime
				if((editor.m_EditorMode == EditorComponent::EditorMode::Play) || (!m_IsEditorScene && this->FindGameObjectByID(obj).IsActive()))
				{
					auto& transform = view.get<TransformComponent>(obj);
					auto& player = view.get<PlayerControllerComponent>(obj);
					// Only continue if the component is active/enabled
					if(player.IsActive())
					{
						if(!player.m_MouseDisabled) Application::Get().GetWindow().SetCursorDisabled(true); player.m_MouseDisabled = true;
						float s = 0.0f;
						// if were sprinting, use sprint speed else if were crouched use crouched speed else use the normal movement speed
						auto speed = (player.IsSprinting()) ? player.SprintSpeed : (s = (player.IsCrouched()) ? player.CrouchSpeed : player.MovementSpeed);
				
						if(Input::IsKeyPressed(player.Forward)) transform.Position += transform.Forward * speed * deltaTime.GetSeconds();
						if(Input::IsKeyPressed(player.Backward)) transform.Position -= transform.Forward * speed * deltaTime.GetSeconds();
						if(Input::IsKeyPressed(player.Left)) transform.Position -= transform.Right * speed * deltaTime.GetSeconds();
						if(Input::IsKeyPressed(player.Right)) transform.Position += transform.Right * speed * deltaTime.GetSeconds();
						player.ProcessMouseRotation(transform);
					}
				}
			}
			
		}
#pragma endregion

#pragma region TransformComponentUpdate
		
		//Update all transform components and their forward, right and up vectors
		{
			auto view = m_Registry.view<TransformComponent>();
			for (auto obj : view)
			{
				auto& transform = view.get<TransformComponent>(obj);

				glm::vec3 forward = glm::vec3(0.0f);
				glm::vec3 right = glm::vec3(0.0f);
				glm::vec3 up = glm::vec3(0.0f);
				forward.x = cos(glm::radians(transform.Rotation.x)) * cos(glm::radians(transform.Rotation.y));
				forward.y = sin(glm::radians(transform.Rotation.y));
				forward.z = sin(glm::radians(transform.Rotation.x)) * cos(glm::radians(transform.Rotation.y));
				forward = glm::normalize(forward);
				right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
				up = glm::normalize(glm::cross(right, forward));
				transform.Forward = forward;
				transform.Right = right;
				transform.Up = up;
			}
		}
		
#pragma endregion	
		
#pragma region FramebufferBindings
		uint32_t passes = 0;
		do
		{
			if(m_IsEditorScene)
			{
				if(passes == 0)
				{
					m_EditorSceneFramebuffer->Bind(0);
				}
				
				if(passes == 1)
				{
					
					m_EditorGameFramebuffer->Bind(0);
				}
			}
		
			//after the transform is updated, update the camera matrix etc 
			{
				if(m_IsEditorScene && passes == 0)
				{
					auto& camera = GetEditorCamera().GetComponent<EditorCameraComponent>().EditorCamera;
					auto& transform = GetEditorCamera().Transform();
					camera.OnUpdate(transform);
				}
				else
				{
					auto view = m_Registry.view<TransformComponent, CameraComponent>();
					for(auto entity : view)
					{ 
						auto& camera = view.get<CameraComponent>(entity);
						auto& transform = view.get<TransformComponent>(entity);
						camera.Camera.OnUpdate(transform);
					}
				}
			}
#pragma endregion 			

#pragma region Rendering
			{
#pragma region ClearColorAndSkybox
				glm::vec3 cameraPosition;
				bool usingSkybox = false;
				{
					//Check if its were in the editor, if so render from the perspective of the editor camera
					if(m_IsEditorScene && passes == 0)
					{
						auto& camera = GetEditorCamera().GetComponent<EditorCameraComponent>();
						auto& transform = GetEditorCamera().Transform();
						cameraPosition = transform.Position;
						if(camera.ClearMode == CameraClearMode::Color)
						{
							RenderCommand::SetClearColor(camera.ClearColor);
							RenderCommand::Clear();
						
						}
						else if(camera.ClearMode == CameraClearMode::Skybox)
						{
							RenderCommand::SetClearColor({0.0f, 0.0f, 0.0f, 1.0f});
							RenderCommand::Clear();
							usingSkybox = true;
						}
					
						Renderer::BeginScene(camera, transform);
					
					}
					else
					{
						auto view = m_Registry.view<TransformComponent, CameraComponent>();
						for(auto entity : view)
						{ 
							auto& camera = view.get<CameraComponent>(entity);
							auto& transform = view.get<TransformComponent>(entity);
							cameraPosition = transform.Position;
							if(camera.ClearMode == CameraClearMode::Color)
							{
								RenderCommand::SetClearColor(camera.ClearColor);
								RenderCommand::Clear();
							}
							else if(camera.ClearMode == CameraClearMode::Skybox)
							{
								RenderCommand::SetClearColor({0.0f, 0.0f, 0.0f, 1.0f});
								RenderCommand::Clear();
								usingSkybox = true;
							}
						
							Renderer::BeginScene(camera, transform);
							break;
							TNAH_CORE_ASSERT(false, "The TNAH-Engine only supports rendering from a single camera!")
						}
					}
				}

				// Skybox
				if(usingSkybox)
				{
					auto view = m_Registry.view<SkyboxComponent>();
					for(auto obj : view)
					{
						auto& skybox = view.get<SkyboxComponent>(obj);
						// Bind the skybox shader
						// Do any skybox render setup
						Renderer::SubmitSkybox(skybox.SceneSkybox->GetVertexArray(), skybox.SceneSkybox->GetMaterial());
					}
				}
#pragma endregion 
				
#pragma region Lighting
				std::vector<Ref<Light>> sceneLights;
				{
					auto view = m_Registry.view<LightComponent, TransformComponent>();
					for(auto entity : view)
					{
						auto& light = view.get<LightComponent>(entity);
						auto& transform = view.get<TransformComponent>(entity);
						if(light.Light == nullptr) continue; 
						light.Light->SetPosition(transform.Position);
						light.Light->UpdateShaderLightInfo(cameraPosition);
						sceneLights.push_back(light.Light);
					}
				}
#pragma endregion

#pragma region TerrainRender
				//Render any terrain objects
				{
					auto view = m_Registry.view<TransformComponent, TerrainComponent>();
					for(auto entity : view)
					{
						auto& terrain = view.get<TerrainComponent>(entity);
						auto& transform = view.get<TransformComponent>(entity);

						Renderer::SubmitTerrain(terrain.SceneTerrain->GetVertexArray(), terrain.SceneTerrain->GetMaterial(), sceneLights, transform.GetTransform());
					}
				}
#pragma endregion

#pragma region MeshRender
				//Render all mesh components
				{
					auto view = m_Registry.view<TransformComponent, MeshComponent>();
					for(auto entity : view)
					{
						auto& model = view.get<MeshComponent>(entity);
						auto& transform = view.get<TransformComponent>(entity);
						glm::mat4 matrix = transform.GetTransform();
						auto& go = FindGameObjectByID(entity);
						
						if(model.Model)
						{
							for (auto& mesh : model.Model->GetMeshes())
							{
								if(mesh.GetMeshMaterial()->GetTextures().size() == 1)
								{
									//theres no specular texture on the mesh. assign the default black to the specular.
									auto mat = mesh.GetMeshMaterial();
									auto t = Renderer::GetBlackTexture(); // we want to copy not directly use to be able to set a custom uniform name
									t->m_UniformName = "texture_specular1";
									mat->AddTexture(t);
									
								}
								Renderer::SubmitMesh(mesh.GetMeshVertexArray(), mesh.GetMeshMaterial(), sceneLights, matrix);
							}
						}
					}
				}
#pragma endregion

				Renderer::EndScene();
			}
#pragma endregion

#pragma region FramebufferEndings
			passes++;
		}while(passes < m_RenderPasses);

		//Only need to call this on one buffer as its always going to bind FBO 0 ie no framebuffer
		if(m_IsEditorScene) m_EditorSceneFramebuffer->Unbind();
#pragma endregion
		
	}
#pragma endregion SceneUpdate

#pragma region SceneHelpers
	glm::mat4 Scene::GetTransformRelativeToParent(GameObject gameObject)
	{
		glm::mat4 transform(1.0f);

		GameObject parent = FindEntityByUUID(gameObject.GetParentUUID());
		if (parent)
			transform = GetTransformRelativeToParent(parent);

		return transform * gameObject.Transform().GetTransform();
	}

	GameObject& Scene::CreateGameObject(const std::string& name)
	{
		GameObject go = { m_Registry.create(), this };
		auto& idComponent = go.AddComponent<IDComponent>();
		idComponent.ID = {};
		go.AddComponent<TransformComponent>();
		if (!name.empty())
		{
			go.AddComponent<TagComponent>(name);
		}

		go.AddComponent<RelationshipComponent>();

		
		m_GameObjectsInScene[idComponent.ID] = go;
		return m_GameObjectsInScene[idComponent.ID];
	}

	


	GameObject Scene::CreateGameObject()
	{
		GameObject go = { m_Registry.create(), this };
		auto& idComponent = go.AddComponent<IDComponent>();
		idComponent.ID = {};
		go.AddComponent<TransformComponent>();
		go.AddComponent<TagComponent>("Default");

		go.AddComponent<RelationshipComponent>();

		m_GameObjectsInScene[idComponent.ID] = go;
		return go;
	}

	GameObject Scene::CreateEditorCamera()
	{
		auto go = CreateGameObject("Editor Camera");
		auto& c = go.AddComponent<EditorCameraComponent>();
		auto& t = go.GetComponent<TransformComponent>();
		c.EditorCamera.SetViewportSize(1280, 720);
		t.Position = {0,0,0};
		Texture3DProperties properties = {{"Resources/textures/default/skybox/default_linear_skybox.ktx2"}};
		go.AddComponent<SkyboxComponent>(properties);
		m_EditorCamera = go.GetUUID();
		return go;
	}

	void Scene::Destroy()
	{
		m_EditorGameFramebuffer->~Framebuffer();
		m_EditorSceneFramebuffer->~Framebuffer();
		
	}

	GameObject Scene::FindEntityByTag(const std::string& tag)
	{
		auto view = m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			const auto& canditate = view.get<TagComponent>(entity).Tag;
			if (canditate == tag)
				return GameObject(entity, this);
		}

		return GameObject{};
	}

	GameObject Scene::FindEntityByUUID(UUID id)
	{
		auto view = m_Registry.view<IDComponent>();
		for (auto entity : view)
		{
			auto& idComponent = m_Registry.get<IDComponent>(entity);
			if (idComponent.ID == id)
				return GameObject(entity, this);
		}

		return GameObject{};
	}


	GameObject& Scene::FindGameObjectByID(const entt::entity& id)
	{
		try
		{
			for(auto& go : m_GameObjectsInScene)
			{
				if(go.second.GetID() == id)
				{
					return go.second;
				}
			}

			throw "GameObject not found!";
		}
		catch (const char* error)
		{
			TNAH_CORE_INFO("{0}", error);
		}
	}
	
	void Scene::DestroyGameObject(GameObject gameObject)
	{
		m_Registry.destroy(gameObject.GetID());
	}

	GameObject& Scene::GetSceneCamera()
	{
		return m_GameObjectsInScene[m_ActiveCamera];
	}

	GameObject& Scene::GetSceneLight()
	{
		return m_GameObjectsInScene[m_SceneLight];
	}
#pragma endregion 
	
}

