#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
//#include "Camera.h"
#include "PhysicsWorld.h"

#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer()
		: m_Camera(45.0f, 0.1f, 100.0f)
	{
		Material& MetalSphere = m_Scene.Materials.emplace_back();
		MetalSphere.Albedo = { 1.0f, 0.0f, 1.0f };
		MetalSphere.Roughness = 0.0f;

		Material& LambertSphere = m_Scene.Materials.emplace_back();
		LambertSphere.Albedo = { 0.2f, 0.3f, 1.0f };
		LambertSphere.Roughness = 0.1f;

		Material& DielectricSphere = m_Scene.Materials.emplace_back();
		DielectricSphere.Albedo = { 0.2f, 0.3f, 1.0f };
		DielectricSphere.Roughness = 0.0f;
		DielectricSphere.type = Material::Type::Dielectric;

		{
			Sphere sphere;
			sphere.Position = { 0.8f, 0.0f, 0.0f };
			sphere.Radius = 1.0f;
			sphere.MaterialIndex = 0;
			m_Scene.AddSphere(sphere);
		}

		{
			Sphere sphere;
			sphere.Position = { 0.0f, -101.0f, 0.0f };
			sphere.Radius = 100.0f;
			sphere.MaterialIndex = 1;
			m_Scene.AddSphere(sphere);
		}
	}

	virtual void OnUpdate(float ts) override
	{
		if (m_Camera.OnUpdate(ts))
			m_Renderer.ResetFrameIndex();
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Setting");

		ImGui::Text("Last Render: %.3fms", m_LastRenderTime);
		if (ImGui::Button("Render"))
		{
			Render();
		}

		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);

		if (ImGui::Button("Reset"))
		{
			m_Renderer.ResetFrameIndex();
		}

		ImGui::End();

		ImGui::Begin("Scene");

		ImGui::Begin("Spheres");

		if (ImGui::Button("Add Sphere"))
		{
			{
				Sphere sphere;
				sphere.Dynamic = true;
				sphere.MaterialIndex = 2;
				sphere.Position = { 2.5, 5.0f, 0 };
				m_Scene.AddSphere(sphere);
			}
		}

		if (ImGui::Button("Delete Sphere") && m_Scene.Spheres.size() > 0)
		{
			//m_Scene.RemoveSphere(m_Scene.Spheres[0]);
			m_Scene.Spheres.pop_back();
		}

		for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
		{
			ImGui::PushID(i);

			Sphere& sphere = m_Scene.Spheres[i];
			ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
			ImGui::DragFloat("Radius", &sphere.Radius, 0.1f);
			ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0.0f, (int)m_Scene.Materials.size() - 1);
			

			ImGui::Separator();
			ImGui::PopID();
		}

		ImGui::End();
		ImGui::Begin("Material");
		if (ImGui::Button("Add Material"))
		{
			{
				Material material;
				m_Scene.Materials.push_back(material);
			}
		}

		//if (ImGui::Button("Delete Material") && m_Scene.Materials.size() > 0)
		//{
		//	m_Scene.Materials.pop_back();
		//}

		static const char* MaterialTypes[]{ "Lmbert", "Metal", "Dielectric" };
		static bool selected[3];
		static std::string preview = "";

		for (size_t i = 0; i < m_Scene.Materials.size(); i++)
		{
			ImGui::PushID(i);

			Material& material = m_Scene.Materials[i];
			ImGui::DragFloat("Metallic", &material.Metallic, 0.05f, 0.0f, 1.0f);
			ImGui::DragFloat("Roughness", &material.Roughness, 0.05f, 0.0f, 1.0f);
			ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));

			ImGui::DragInt("Type", &material.type, 0.1f, 0, 2);
			
			ImGui::Separator();
			ImGui::PopID();
		}

		ImGui::End();

		ImGui::End();

		ImGui::Begin("Viewport");

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = m_Renderer.GetFinalImage();
		if (image)
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() },
				ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();


		Render();
	}

	void Render()
	{
		Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);

		m_physics.simulate(m_Scene, 100.0f * timer.ElapsedMillis());

		m_Renderer.Render(m_Scene, m_Camera);
		
		m_LastRenderTime = timer.ElapsedMillis();
	}

private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;

	PhysicsWorld m_physics;

	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	float m_LastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing Demo";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}