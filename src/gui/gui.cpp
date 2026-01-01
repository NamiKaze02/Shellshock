#include "gui.h"

#include "ImGuiFileDialog.h"
#include <d3d11.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <log.h>

constexpr auto CHOOSE_MODEL_DIALOG_KEY = "ChooseModelKey";
constexpr auto LOG_WHITE = ImVec4(1, 1, 1, 1);

static ImVec4 LogTypeToColor(TGW::LogType type);

TGW::GUI::Base::~Base()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void TGW::GUI::Init(HWND hwnd, ID3D11Device *device, ID3D11DeviceContext *context)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, context);
}

void TGW::GUI::Base::Update()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void TGW::GUI::Base::Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void TGW::GUI::EditorMain::Update()
{
	Base::Update();

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open Model...")) {
				IGFD::FileDialogConfig config;
				config.path = ".";
				config.flags = ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_ShowDevicesButton;
				ImGuiFileDialog::Instance()->OpenDialog(CHOOSE_MODEL_DIALOG_KEY, "Select Model", ".obj,.fbx,.gltf", config);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (ImGuiFileDialog::Instance()->Display(CHOOSE_MODEL_DIALOG_KEY)) {
		if (ImGuiFileDialog::Instance()->IsOk()) {
			std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
			_queue.push_back(LoadModelCommand{path});
			TGW::Logger::AddLog("Loading model: " + path, TGW::LogType::INFO);
		}
		ImGuiFileDialog::Instance()->Close();
	}

	UpdateLogs();
	ImGui::End();
}

void TGW::GUI::EditorMain::UpdateLogs()
{
	const ImGuiViewport *viewport = ImGui::GetMainViewport();
	float logWindowHeight = viewport->WorkSize.y * 0.20f;
	ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + viewport->WorkSize.y - logWindowHeight));
	ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, logWindowHeight));

	ImGuiWindowFlags logFlags =
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

	if (ImGui::Begin("LogSession", nullptr, logFlags)) {
		if (ImGui::Button("Clear")) {
			TGW::Logger::Clear();
		}
		ImGui::SameLine();
		ImGui::TextUnformatted("Application Logs");
		ImGui::Separator();

		const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);

		for (const auto &log : TGW::Logger::GetAll()) {
			ImGui::TextColored(LogTypeToColor(log.type) , log.AsString().c_str());
		}

		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
			ImGui::SetScrollHereY(1.0f);
		}

		ImGui::EndChild();
	}
}

static ImVec4 LogTypeToColor(TGW::LogType type)
{
	switch (type) {
	case TGW::LogType::INFO:
		return LOG_WHITE;
	}
	return LOG_WHITE;
}