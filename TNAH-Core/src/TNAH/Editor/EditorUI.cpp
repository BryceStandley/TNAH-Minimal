#include "tnahpch.h"
#include "EditorUI.h"

#include "TNAH/Core/Application.h"
#include "TNAH/Scene/Components/Components.h"



#include "TNAH/Core/FileManager.h"

#pragma warning(push, 0)
#include <imgui.h>
#include "imgui_internal.h"
#pragma warning(pop)

namespace tnah {

	static std::string search = "";
	static ComponentVariations selectedComponent = ComponentVariations::None;
	void EditorUI::DrawComponentProperties(GameObject& object, const bool& addComponents)
	{
		
		if(object.HasComponent<TagComponent>())
		{
			auto& tag = object.GetComponent<TagComponent>();
			DrawTextControl("Name", tag.Tag);
			ImGui::Separator();
		}
		
	   
		if(object.HasComponent<TransformComponent>())
		{
			if(ImGui::TreeNode("Transform"))
			{
				auto& t = object.GetComponent<TransformComponent>();
				if(DrawVec3Control("Position", t.Position))
				{
				}
				glm::vec3 rotation = glm::degrees(t.Rotation);
				if(DrawVec3Control("Rotation", rotation))
				{
					t.Rotation = glm::radians(rotation);
				}
				
				if(DrawVec3Control("Scale", t.Scale, false, 1))
				{
				}
				ImGui::TreePop();
				ImGui::Separator();
			}

		}

		if(object.HasComponent<CameraComponent>())
		{
			if(ImGui::TreeNode("Camera"))
			{
				auto& c = object.GetComponent<CameraComponent>();
				static int selectedType = 1;
				static const char* CameraTypes[]
				{
					"Orthographic", "Perspective"	
				};

				static int selectedClear = 1;
				static const char* CameraClear[] {"Skybox", "Color"};
				bool modified = false;
				ImGui::Combo("##T", &selectedType, CameraTypes, IM_ARRAYSIZE(CameraTypes));
				ImGui::Combo("##C", &selectedClear, CameraClear, IM_ARRAYSIZE(CameraClear));
				if(selectedClear == 0)
				{
					ImGui::Text("Display some text here for setting a skybox or use default");
				
				}
				else if(selectedClear == 1 && c.ClearMode == CameraClearMode::Color)
				{
					Draw4ColorControl("Clear Color", c.ClearColor);
				}

				if(c.ClearMode == CameraClearMode::Skybox && selectedClear == 1)
				{
					if(ImGui::Button("Save"))
					{
						c.SetClearMode(CameraClearMode::Color);
					}
				}

				if(c.ClearMode == CameraClearMode::Color && selectedClear == 0)
				{
					if(ImGui::Button("Save"))
					{
						c.SetClearMode(CameraClearMode::Skybox);
					}
				}

				if(c.ClearMode == CameraClearMode::None)
				{
					if(ImGui::Button("Reset"))
					{
						c.SetClearMode(CameraClearMode::Skybox);
					}
				}

			
				if(selectedType == 0)
				{
					//Ortho
					if(c.Camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
					{
						c.Camera.SetOrthographic(10);
					}
					auto s = c.Camera.m_OrthographicSize;
					auto n = c.Camera.m_OrthographicNear;
					auto f = c.Camera.m_OrthographicFar;
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
					ImGui::Text("Orthographic Cameras currently not supported");
					ImGui::PopStyleColor();
					modified |= DrawFloatControl("Size", s, 0, 50);
					modified |= DrawFloatControl("Near Plane", n, -10, 10);
					modified |= DrawFloatControl("Far Plane", f, -10, 10);

					if(modified)
					{
						c.Camera.SetOrthographicSize(s);
						c.Camera.SetOrthographicNearClip(n);
						c.Camera.SetOrthographicNearClip(f);
					}
				}
				else
				{
					if(c.Camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
					{
						c.Camera.SetPerspective(60);
					}
					auto fov = c.Camera.GetPerspectiveVerticalFOV();
					auto nearc = c.Camera.m_PerspectiveNear;
					auto farc = c.Camera.m_PerspectiveFar;
				
					modified |= DrawFloatControl("Field of View", fov, 60, 120);
					modified |=DrawFloatControl("Near Plane", nearc,  0.01f, 1.0f);
					modified |=DrawFloatControl("Far Plane", farc,  100.0f, 10000.0f);
				
					if(modified)
					{
						c.Camera.SetPerspectiveVerticalFOV(fov);
						c.Camera.SetPerspectiveNearClip(nearc);
						c.Camera.SetPerspectiveFarClip(farc);
					}
				}

				if(Camera::Main != &c.Camera && addComponents) // only allow the camera to be removed if its not the main camera
					{
					if(DrawRemoveComponentButton("camera"))
					{
						object.RemoveComponent<CameraComponent>();
					}
					}
				ImGui::TreePop();
				ImGui::Separator();
				
			}
		}
		

		if(object.HasComponent<EditorCameraComponent>())
		{
			if(ImGui::TreeNode("Editor Camera"))
			{
				
				auto& c = object.GetComponent<EditorCameraComponent>();
				auto fov = glm::degrees(c.EditorCamera.m_PerspectiveFOV);
				auto nearc = c.EditorCamera.m_PerspectiveNear;
				auto farc = c.EditorCamera.m_PerspectiveFar;
				float w = static_cast<float>(c.EditorCamera.m_ViewportWidth);
				float h = static_cast<float>(c.EditorCamera.m_ViewportHeight);
				bool modified = false;
				static int selectedClear = 1;
				static const char* CameraClear[] {"Skybox", "Color"};
				ImGui::Combo("##C", &selectedClear, CameraClear, IM_ARRAYSIZE(CameraClear));
				if(selectedClear == 0)
				{
					ImGui::Text("Display some text here for setting a skybox or use default");
					
				}
				else if(selectedClear == 1 && c.ClearMode == CameraClearMode::Color)
				{
					Draw4ColorControl("Clear Color", c.ClearColor);
				}

				if(c.ClearMode == CameraClearMode::Skybox && selectedClear == 1)
				{
					if(ImGui::Button("Save"))
					{
						c.SetClearMode(CameraClearMode::Color);
					}
				}

				if(c.ClearMode == CameraClearMode::Color && selectedClear == 0)
				{
					if(ImGui::Button("Save"))
					{
						c.SetClearMode(CameraClearMode::Skybox);
					}
				}

				if(c.ClearMode == CameraClearMode::None)
				{
					if(ImGui::Button("Reset"))
					{
						c.SetClearMode(CameraClearMode::Skybox);
					}
				}

				DrawFloatControl("Viewport Width", w, 0,0, true);
				DrawFloatControl("Viewport Height", h, 0,0, true);
				modified |= DrawFloatControl("Field of View", fov, 60, 120);
				modified |=DrawFloatControl("Near Plane", nearc,  0.01f, 1.0f);
				modified |=DrawFloatControl("Far Plane", farc,  100.0f, 10000.0f);
				if(modified)
				{
					c.EditorCamera.SetPerspectiveVerticalFOV(fov);
					c.EditorCamera.SetPerspectiveNearClip(nearc);
					c.EditorCamera.SetPerspectiveFarClip(farc);
				}
				
				ImGui::TreePop();
				ImGui::Separator();
			}
		}
		
		if(object.HasComponent<TerrainComponent>())
		{
			
			if(ImGui::TreeNode("Terrain"))
			{
				auto& t = object.GetComponent<TerrainComponent>().SceneTerrain;
				DrawVec2Control("Size", t->m_Size, true);
				ImGui::BulletText("Maybe have more options here to set the terrain textures?");

				if(addComponents)
				{
					if(DrawRemoveComponentButton("terrain"))
					{
						object.RemoveComponent<TerrainComponent>();
					}
				}
				ImGui::TreePop();
				ImGui::Separator();
				
			}
		}
		
		if(object.HasComponent<MeshComponent>())
		{
			
			if(ImGui::TreeNode("Mesh"))
			{
				auto m = object.GetComponent<MeshComponent>().Model;
				if(m)
				{
					
					DrawTextControl("Model File", m->m_Resource.FileName.FullFile, false, true);
					if(ImGui::Button("Change Mesh"))
					{
						if (FileManager::OpenMesh())
						{
							auto file = FileManager::GetActiveFile();
							if (file->FileOpenError == FileError::PathInvalid)
							{
								TNAH_WARN("The path or file was invalid!");
							}
							else
							{
								object.GetComponent<MeshComponent>().LoadMesh(file->FilePath);
							}
						}
					}
					ImGui::Separator();
					ImGui::Text("Sub Meshes");
					int count = 0;
					for(auto& mesh : m->m_Meshes)
					{
						std::string label = "SubMesh " + std::to_string(count);
						if(ImGui::CollapsingHeader(label.c_str()))
						{
							DrawMaterialProperties(false, mesh.m_Material);
						}
						count++;
					}
				}
				else
				{
					std::string error = "Empty";
					DrawTextControl("Model File", error, true);
					if(ImGui::Button("Add Mesh"))
					{
						if (FileManager::OpenMesh())
						{
							auto file = FileManager::GetActiveFile();
							if (file->FileOpenError == FileError::PathInvalid)
							{
								TNAH_WARN("The path or file was invalid!");
							}
							else
							{
								object.GetComponent<MeshComponent>().LoadMesh(file->FilePath);
							}
						}
					}
					ImGui::Separator();
					ImGui::Text("Sub Meshes");
					if(ImGui::CollapsingHeader("Empty"))
					{
						DrawMaterialProperties(true);
					}
				}

				if(addComponents)
				{
					if(DrawRemoveComponentButton("Mesh"))
					{
						object.RemoveComponent<MeshComponent>();
					}
				}
			
				ImGui::Separator();
				ImGui::TreePop();
			}
		}

		if(object.HasComponent<LightComponent>())
		{
			auto& l = object.GetComponent<LightComponent>().Light;
			auto name = l->GetTypeAsString() + " Light";
			ImGui::Text(name.c_str());
				if (l->GetType() == Light::LightType::Directional)
				{
					DrawVec3Control("Direction", l->GetDirection());
					DrawFloatControl("Intensity", l->GetIntensity(), 0, 10);
					Draw4ColorControl("Color", l->GetColor());
					DrawVec3Control("Ambient", l->GetAmbient());
					DrawVec3Control("Diffuse", l->GetDiffuse());
					DrawVec3Control("Specular", l->GetSpecular());
				}
				else if (l->GetType() == Light::LightType::Point)
				{
					DrawFloatControl("Intensity", l->GetIntensity(), 0, 10);
					Draw4ColorControl("Color", l->GetColor());
					DrawVec3Control("Ambient", l->GetAmbient());
					DrawVec3Control("Diffuse", l->GetDiffuse());
					DrawVec3Control("Specular", l->GetSpecular());
				}
				else if (l->GetType() == Light::LightType::Spot)
				{
					DrawFloatControl("Intensity", l->GetIntensity(), 0, 10);
					Draw4ColorControl("Color", l->GetColor());
					DrawVec3Control("Ambient", l->GetAmbient());
					DrawVec3Control("Diffuse", l->GetDiffuse());
					DrawVec3Control("Specular", l->GetSpecular());
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
					ImGui::Text("ERROR: Unknown light type");
					ImGui::PopStyleColor();
				}
			if (!l->m_IsSceneLight && addComponents)
			{
				if (DrawRemoveComponentButton("Light"))
				{
					object.RemoveComponent<LightComponent>();
				}
			}
			ImGui::TreePop();
		}

		ImGui::Separator();
		
		//Only add components to scene objects, the editor camera cant have components added to it
		if(!object.HasComponent<EditorCameraComponent>() && addComponents)
		{
			ImGui::NewLine();
			auto width = ImGui::GetWindowWidth();
			static bool addComponentPressed = false;
			if(!addComponentPressed)
			{
				if (ImGui::Button("Add Component", {width, 0}))
				{
					
					addComponentPressed = true;
				}
			}
			else
			{
				//draw add component section
				
				DrawTextControl("Search", search);
				std::transform(search.begin(), search.end(), search.begin(), ::tolower);
				ImGui::NewLine();
				if(DrawAddComponent(object, search)) { addComponentPressed = false; search = ""; selectedComponent = ComponentVariations::None; }
				if(!ImGui::IsWindowFocused()) addComponentPressed = false;
			}
		}

		ImGui::Separator();
	}

#pragma region DrawFunctions
	
	bool EditorUI::Draw4ColorControl(const std::string& label, glm::vec4& value, bool readOnly, const glm::vec4& resetValue)
	{
		bool modified = false;
		ImGui::PushID(label.c_str());
		ImGui::Text(label.c_str());
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		
		if(!readOnly)
		{
			if(ImGui::ColorEdit4("##C", glm::value_ptr(value)))
				modified = true;
		}
		else
		{
			ImGui::InputFloat4("##C", glm::value_ptr(value), "%.3f", ImGuiInputTextFlags_ReadOnly);
		}
		ImGui::PopItemWidth();
		ImGui::NextColumn();
		ImGui::PopID();
		
		return modified;
	}

	bool EditorUI::DrawMaterialProperties(bool empty, Ref<Material> material)
	{
		if(!empty)
		{
			bool singleFile = false;
			if(std::strcmp(material->m_Shader->m_FilePaths.second.c_str(), "") == 0) singleFile = true;
			ImGui::Text("Material");
			ImGui::Separator();
			ImGui::Text("Shader");
			DrawTextControl("Name", material->m_Shader->m_ShaderName);
			if(singleFile)
				DrawTextControl("Source", material->m_Shader->m_FilePaths.first);
			else
			{
				DrawTextControl("Vertex Source", material->m_Shader->m_FilePaths.first);
				DrawTextControl("Fragment Source", material->m_Shader->m_FilePaths.second);
			}
			ImGui::BulletText("Maybe make the shader option a file input to select a new shader file");
			ImGui::Separator();
			ImGui::Text("Properties");
			DrawFloatControl("Shininess", material->m_Properties.Shininess);
			DrawFloatControl("Metalness", material->m_Properties.Metalness);
			ImGui::Separator();
			ImGui::Text("Texture");

			auto size = ImGui::GetWindowWidth();
			size = (size /  4.0f) * 2;
		
			for(auto& t : material->m_Textures)
			{
				if(ImGui::CollapsingHeader(t->m_TextureResource.CustomName.c_str()))
				{
					glm::vec2 s = {t->m_Width, t->m_Height};
					DrawTextControl("Path", t->m_TextureResource.RelativeDirectory);
					DrawVec2Control("Size", s, true);
					ImGui::Separator();
					if(ImGui::CollapsingHeader("Preview"))
					{
						ImGui::Image((void*)static_cast<intptr_t>(t->m_RendererID), ImVec2(size, size));
					}	
				}
			}

			ImGui::Text("Maybe Add a 3D Material preview on a sphere for PBR materials?");
			return true;
		}
		
		ImGui::Text("No material to display");
		return false;
		
	}

	bool EditorUI::DrawRemoveComponentButton(const std::string& id)
	{
		std::string label = "Remove Component##" + id;
		return DrawRedButton(label);
	}

	bool EditorUI::DrawResetButton(const std::string& id)
	{
		std::string label = "Reset##" + id;
		return DrawRedButton(label);
	}

	bool EditorUI::DrawFloatControl(const std::string& label, float& value, float min, float max,
	                                bool readOnly)
	{
		bool modified = false;
		ImGui::PushID(label.c_str());
		ImGui::Text(label.c_str());
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		if (!readOnly)
		{
			if (ImGui::SliderFloat("##V", &value, min, max))
				modified = true;
		}
		else
		{
			ImGui::InputFloat("##V", &value, 0.0F, 0.0F, "%.3f", ImGuiInputTextFlags_ReadOnly);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();
		ImGui::PopID();

		return modified;
	}

	bool EditorUI::DrawVec4Control(const std::string& label, glm::vec4& values, bool readOnly, float resetValue,
		float columnWidth)
	{
		bool modified = false;

		const ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (!readOnly && ImGui::Button("X", buttonSize))
		{
			values.x = resetValue;
			modified = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if(!readOnly) modified |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		else ImGui::Text("%.3f", values.x);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (!readOnly && ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue;
			modified = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if(!readOnly) modified |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		else ImGui::Text("%.3f", values.y);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (!readOnly && ImGui::Button("Z", buttonSize))
		{
			values.z = resetValue;
			modified = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if(!readOnly) modified |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		else ImGui::Text("%.3f", values.z);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.8f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (!readOnly && ImGui::Button("W", buttonSize))
		{
			values.w = resetValue;
			modified = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if(!readOnly) modified |= ImGui::DragFloat("##W", &values.w, 0.1f, 0.0f, 0.0f, "%.2f");
		else ImGui::Text("%.3f", values.w);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return modified;
	}

	bool EditorUI::DrawVec2Control(const std::string& label, glm::vec2& values, bool readOnly, float resetValue,
		float columnWidth)
	{
		bool modified = false;

		const ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (!readOnly && ImGui::Button("X", buttonSize))
		{
			values.x = resetValue;
			modified = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if(!readOnly) modified |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		else ImGui::Text("%.3f", values.x);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (!readOnly && ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue;
			modified = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if(!readOnly) modified |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		else ImGui::Text("%.3f", values.y);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return modified;
	}
	
	bool EditorUI::DrawVec3Control(const std::string& label, glm::vec3& values, bool readOnly, float resetValue, float columnWidth)
	{
		bool modified = false;

		const ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (!readOnly && ImGui::Button("X", buttonSize))
		{
			values.x = resetValue;
			modified = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if(!readOnly) modified |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		else ImGui::Text("%.3f", values.x);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (!readOnly && ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue;
			modified = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if(!readOnly) modified |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		else ImGui::Text("%.3f", values.y);
		
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (!readOnly && ImGui::Button("Z", buttonSize))
		{
			values.z = resetValue;
			modified = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if(!readOnly) modified |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		else ImGui::Text("%.3f", values.z);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
		ImGui::NewLine();

		return modified;
	}

	static uint32_t s_Counter = 0;
	static char s_IDBuffer[16];
	bool EditorUI::DrawTextControl(const char* label, std::string& text, bool error, bool readOnly)
	{
		bool modified = false;
		int flags = 0;
		ImGui::Text(label);
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		char buffer[256];
		strcpy_s<256>(buffer, text.c_str());

		s_IDBuffer[0] = '#';
		s_IDBuffer[1] = '#';
		memset(s_IDBuffer + 2, 0, 14);
		sprintf_s(s_IDBuffer + 2, 14, "%o", s_Counter++);
		std::string name = "##";
		name += label;
		
		flags = readOnly ? ImGuiInputTextFlags_ReadOnly : 0; 
		
		if (error)
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
		if (ImGui::InputText(name.c_str(), buffer, 256, flags))
		{
			text = buffer;
			modified = true;
		}
		if (error)
			ImGui::PopStyleColor();
		ImGui::PopItemWidth();
		ImGui::NextColumn();

		return modified;
	}

	bool EditorUI::DrawRedButton(const std::string& label)
	{
		bool pressed = false;
		ImGui::PushItemWidth(ImGui::GetWindowWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		auto buttonSize = ImGui::GetWindowWidth();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		
		if(ImGui::Button(label.c_str(), {buttonSize, 0}))
		{
			pressed = true;
		}

		ImGui::PopStyleColor(3);
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();

		return pressed;
	}

	std::list<ComponentVariations> EditorUI::GetPossibleComponentTypes(std::vector<ComponentVariations> typesHeld)
	{
		std::list allTypes =
			{
				ComponentVariations::ID, ComponentVariations::Tag, ComponentVariations::Relationship, ComponentVariations::Transform,
				ComponentVariations::Camera, ComponentVariations::EditorCamera, ComponentVariations::Editor, ComponentVariations::Skybox,
				ComponentVariations::Light, ComponentVariations::Terrain, ComponentVariations::Mesh, ComponentVariations::PlayerController,
				ComponentVariations::AudioListener, ComponentVariations::AudioSource, ComponentVariations::Rigidbody, ComponentVariations::AStar,
			ComponentVariations::AiCharacter, ComponentVariations::Affordance, ComponentVariations::AStarObstacle
				
			};

		std::list<ComponentVariations> allTypesNotHeld = allTypes;

		for(auto type: typesHeld)
		{
			//remove any items from the list that are held by the game object
			allTypesNotHeld.remove(type);
			if(type == ComponentVariations::Terrain)
			{
				//Add a heightfield collider for the terrain
			}
		}
		allTypesNotHeld.remove(ComponentVariations::EditorCamera);
		allTypesNotHeld.remove(ComponentVariations::Editor);

		return allTypesNotHeld;
		
	}

	bool EditorUI::DrawAddComponent(GameObject& object, const std::string& searchTerm)
	{
		auto componentsCanShow = GetPossibleComponentTypes(object.GetComponentList());

		bool emptySearch = searchTerm.compare("") == 0 ? true : false;
		

		if(selectedComponent == ComponentVariations::None)
		{
			if(!emptySearch)
			{
				selectedComponent = FindAndDrawComponentSearchTerm(componentsCanShow, searchTerm);
			}
			else
			{
				selectedComponent = DrawComponentList(componentsCanShow);
			}
		}

		if(selectedComponent != ComponentVariations::None)
		{
			ImGui::Text("Selected:");
			std::string text = FindStringFromComponentType(selectedComponent);
			ImGui::Text(text.c_str());
			if(DrawResetButton("AddComponent"))
			{
				selectedComponent = ComponentVariations::None;
			}
			ImGui::NewLine();
			if(selectedComponent != ComponentVariations::None)
			{
				auto width = ImGui::GetWindowWidth();
				if(ImGui::Button("Add", {width, 0}))
				{
			
					return AddComponentFromType(object, selectedComponent);
					
				}
			}
		}

		return false;
		
	}

	ComponentVariations EditorUI::FindAndDrawComponentSearchTerm(std::list<ComponentVariations> typesToSearch, const std::string& searchTerm)
	{
		auto types = FindAllComponentsContaining(typesToSearch, searchTerm);
		
		return  DrawComponentList(types);
	}


	ComponentVariations EditorUI::DrawComponentList(std::list<ComponentVariations> componentsToDisplay)
	{
		auto width = ImGui::GetWindowWidth();
		for(auto t : componentsToDisplay)
		{
			if(ImGui::Button(FindStringFromComponentType(t).c_str(), {width, 0}))
			{
				return t;
			}
		}
		return ComponentVariations::None;
	}

		
	std::list<ComponentVariations> EditorUI::FindAllSubtypesFromBaseType(std::list<ComponentVariations> componentsToSearch, const ComponentCategory& category)
	{
		std::list<ComponentVariations> foundComponents;

		for(auto v : componentsToSearch)
		{
			if(v == ComponentVariations::ID && Utility::Contains<ComponentCategory>(IDComponent::s_Types.Categories, category))
				foundComponents.emplace_back(ComponentVariations::ID);

			if(v == ComponentVariations::Tag && Utility::Contains<ComponentCategory>(TagComponent::s_Types.Categories, category))
				foundComponents.emplace_back(ComponentVariations::Tag);

			if(v == ComponentVariations::Relationship && Utility::Contains<ComponentCategory>(RelationshipComponent::s_Types.Categories, category))
				foundComponents.emplace_back(ComponentVariations::Relationship);
			
			if(v == ComponentVariations::Transform && Utility::Contains<ComponentCategory>(TransformComponent::s_Types.Categories, category))
				foundComponents.emplace_back(ComponentVariations::Transform);
			
			if(v == ComponentVariations::Camera && Utility::Contains<ComponentCategory>(CameraComponent::s_Types.Categories, category))
				foundComponents.emplace_back(ComponentVariations::Camera);
			
			if(v == ComponentVariations::EditorCamera && Utility::Contains<ComponentCategory>(EditorCameraComponent::s_Types.Categories, category))
				foundComponents.emplace_back(ComponentVariations::EditorCamera);

			if(v == ComponentVariations::Editor && Utility::Contains<ComponentCategory>(EditorComponent::s_Types.Categories, category))
				foundComponents.emplace_back(ComponentVariations::Editor);

			if(v == ComponentVariations::Skybox && Utility::Contains<ComponentCategory>(SkyboxComponent::s_Types.Categories, category))
				foundComponents.emplace_back(ComponentVariations::Skybox);

			if(v == ComponentVariations::Light && Utility::Contains<ComponentCategory>(LightComponent::s_Types.Categories, category))
				foundComponents.emplace_back(ComponentVariations::Light);

			if(v == ComponentVariations::Terrain && Utility::Contains<ComponentCategory>(TerrainComponent::s_Types.Categories, category))
				foundComponents.emplace_back(ComponentVariations::Terrain);

			if(v == ComponentVariations::Mesh && Utility::Contains<ComponentCategory>(MeshComponent::s_Types.Categories, category))
				foundComponents.emplace_back(ComponentVariations::Mesh);

			if(v == ComponentVariations::PlayerController && Utility::Contains<ComponentCategory>(PlayerControllerComponent::s_Types.Categories, category))
				foundComponents.emplace_back(ComponentVariations::PlayerController);

		}

		return foundComponents;
	}

	std::list<ComponentVariations> EditorUI::FindAllComponentsContaining(std::list<ComponentVariations> componentsToSearch, const std::string& term)
	{
		std::list<ComponentVariations> foundComponents;
		
		if(IDComponent::s_SearchString.find(term) != std::string::npos && Utility::Contains<ComponentVariations>(componentsToSearch, ComponentVariations::ID))
			foundComponents.emplace_back(ComponentVariations::ID);

		if(TagComponent::s_SearchString.find(term) != std::string::npos && Utility::Contains<ComponentVariations>(componentsToSearch, ComponentVariations::Tag))
			foundComponents.emplace_back(ComponentVariations::Tag);

		if(RelationshipComponent::s_SearchString.find(term) != std::string::npos && Utility::Contains<ComponentVariations>(componentsToSearch, ComponentVariations::Relationship))
			foundComponents.emplace_back(ComponentVariations::Relationship);

		if(TransformComponent::s_SearchString.find(term) != std::string::npos && Utility::Contains<ComponentVariations>(componentsToSearch, ComponentVariations::Transform))
			foundComponents.emplace_back(ComponentVariations::Transform);

		if(CameraComponent::s_SearchString.find(term) != std::string::npos && Utility::Contains<ComponentVariations>(componentsToSearch, ComponentVariations::Camera))
			foundComponents.emplace_back(ComponentVariations::Camera);

		if(EditorCameraComponent::s_SearchString.find(term) != std::string::npos && Utility::Contains<ComponentVariations>(componentsToSearch, ComponentVariations::EditorCamera))
			foundComponents.emplace_back(ComponentVariations::EditorCamera);

		if(EditorComponent::s_SearchString.find(term) != std::string::npos && Utility::Contains<ComponentVariations>(componentsToSearch, ComponentVariations::Editor))
			foundComponents.emplace_back(ComponentVariations::Editor);

		if(SkyboxComponent::s_SearchString.find(term) != std::string::npos && Utility::Contains<ComponentVariations>(componentsToSearch, ComponentVariations::Skybox))
			foundComponents.emplace_back(ComponentVariations::Skybox);

		if(LightComponent::s_SearchString.find(term) != std::string::npos && Utility::Contains<ComponentVariations>(componentsToSearch, ComponentVariations::Light))
			foundComponents.emplace_back(ComponentVariations::Light);

		if(TerrainComponent::s_SearchString.find(term) != std::string::npos && Utility::Contains<ComponentVariations>(componentsToSearch, ComponentVariations::Terrain))
			foundComponents.emplace_back(ComponentVariations::Terrain);

		if(MeshComponent::s_SearchString.find(term) != std::string::npos && Utility::Contains<ComponentVariations>(componentsToSearch, ComponentVariations::Mesh))
			foundComponents.emplace_back(ComponentVariations::Mesh);

		if(PlayerControllerComponent::s_SearchString.find(term) != std::string::npos && Utility::Contains<ComponentVariations>(componentsToSearch, ComponentVariations::PlayerController))
			foundComponents.emplace_back(ComponentVariations::PlayerController);

		return foundComponents;
	}

	std::string EditorUI::FindStringFromComponentType(ComponentVariations type)
    {
        switch (type)
        {
        case ComponentVariations::None:
            return "No Component";
        case ComponentVariations::ID:
            return "ID";
        case ComponentVariations::Tag:
            return "Tag";
        case ComponentVariations::Relationship:
            return "Relationship";
        case ComponentVariations::Transform:
            return "Transform";
        case ComponentVariations::Camera:
            return "Camera";
        case ComponentVariations::EditorCamera:
            return "Editor Camera";
        case ComponentVariations::Editor:
            return "Editor";
        case ComponentVariations::Skybox:
            return "Skybox";
        case ComponentVariations::Light:
            return "Light";
        case ComponentVariations::Mesh:
        	return "Mesh";
        case ComponentVariations::Terrain:
            return "Terrain";
        case ComponentVariations::PlayerController:
            return "Player Controller";
        case ComponentVariations::AudioSource:
            return "Audio Source";
        case ComponentVariations::AudioListener:
            return "Audio Listener";
        case ComponentVariations::AStar:
        	return "AStar Component";
        case ComponentVariations::AiCharacter:
        	return "AiCharacter Component";
        case ComponentVariations::Rigidbody:
			return "Rigid Body";
        case ComponentVariations::Affordance:
        	return "Affordance";
        case ComponentVariations::AStarObstacle:
        	return "AStar Obstacle Component";
        default: return "";
        }
    }

	std::string EditorUI::FindComponentTypeCategory(ComponentVariations type)
	{
		switch (type)
		{
		case ComponentVariations::None:
			break;
		case ComponentVariations::ID:
			break;
		case ComponentVariations::Tag:
			break;
		case ComponentVariations::Relationship:
			break;
		case ComponentVariations::Transform:
			break;
		case ComponentVariations::Camera:
			return "Camera";
		case ComponentVariations::EditorCamera:
			break;
		case ComponentVariations::Editor:
			break;
		case ComponentVariations::Skybox:
			return "Camera";
		case ComponentVariations::Light:
			return "Light";
		case ComponentVariations::Terrain:
			return "Mesh";
		case ComponentVariations::Mesh:
			return "Mesh";
		case ComponentVariations::AStar:
			return "AStar";
		case ComponentVariations::AiCharacter:
			return "AiCharacter";
		case ComponentVariations::PlayerController:
			return "Control";
		case ComponentVariations::AudioSource:
			return "Physics";
		case ComponentVariations::AudioListener:
			return "Physics";
		case ComponentVariations::Rigidbody:
			return "Physics";
		case ComponentVariations::Affordance:
			return "Affordance";
		default: return "";
		}
		return "";
	}


	bool EditorUI::AddComponentFromType(GameObject& object, ComponentVariations type)
    {
        switch (type)
        {
        case ComponentVariations::None:
            return false;
        case ComponentVariations::ID:
            return false;
        case ComponentVariations::Tag:
            return false;
        case ComponentVariations::Relationship:
            return false;
        case ComponentVariations::Transform:
            return false;
        case ComponentVariations::Camera:
            object.AddComponent<CameraComponent>();
            return true;
        case ComponentVariations::EditorCamera:
            return false;
        case ComponentVariations::Editor:
            return false;
        case ComponentVariations::Skybox:
            object.AddComponent<SkyboxComponent>();
            return true;
        case ComponentVariations::Light:
            object.AddComponent<LightComponent>();
            return true;
        case ComponentVariations::Terrain:
            object.AddComponent<TerrainComponent>();
            return true;
        case ComponentVariations::Mesh:
            object.AddComponent<MeshComponent>();
            return true;
        case ComponentVariations::PlayerController:
            object.AddComponent<PlayerControllerComponent>();
            return true;

        default: return false;
        }
    }
#pragma endregion DrawFunctions

}
