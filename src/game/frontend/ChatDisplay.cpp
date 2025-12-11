#include "ChatDisplay.hpp"

#include "Menu.hpp"
#include "core/logger/LogHelper.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/pointers/Pointers.hpp"
#include "core/frontend/widgets/imgui_colors.h"
#include "fonts/Fonts.hpp"


namespace YimMenu
{
	void ChatDisplay::ShowImpl(std::string sender, std::string message, ImColor color)
	{
		if (sender.empty() || message.empty())
			return;

		Message notification{};
		notification.m_Sender = sender;
		notification.m_Message = message;
		notification.m_Color = color;

		static const bool isBigScreen = *Pointers.ScreenResX > 1600 && *Pointers.ScreenResY > 900;
		static const int maxMessages = isBigScreen ? 17 : 7;

		std::lock_guard<std::mutex> lock(m_Mutex);

		if (m_Messages.size() >= maxMessages)
		{
			m_Messages.erase(m_Messages.begin());
		}

		m_Messages.push_back(notification);
		MarkAccessed();
	}

	const ImWchar* ChatDisplay::GetGlyphRangesCyrillicOnly()
	{
		static const ImWchar ranges[] =
			{
			0x0400,
			0x052F, // Cyrillic + Cyrillic Supplement
			0x2DE0,
			0x2DFF, // Cyrillic Extended-A
			0xA640,
			0xA69F, // Cyrillic Extended-B
			0,
		};
		return &ranges[0];
	}

	ImFont* ChatDisplay::CreateFontWithCyrillicSupport(ImGuiIO& io, float size)
	{
		ImFontConfig FontCfg{};
		FontCfg.FontDataOwnedByAtlas = false;

		auto font = io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Fonts::MainFont), sizeof(Fonts::MainFont), size, &FontCfg, io.Fonts->GetGlyphRangesDefault());

		// just use Arial for Cyrillic

		FontCfg.MergeMode = true;
		io.Fonts->AddFontFromFileTTF((std::filesystem::path(std::getenv("SYSTEMROOT")) / "Fonts" / "arial.ttf").string().c_str(), size, &FontCfg, GetGlyphRangesCyrillicOnly());

		io.Fonts->Build();

		return font;
	}

	static FloatCommand _ChatFontSize{"chatfontsize", "Chat Font Size", "Sets the chat font size scale", 0.0f, 150.0f, 24.0f};

	void ChatDisplay::DrawImpl()
	{
	    std::lock_guard<std::mutex> lock(m_Mutex);
	    int position = 0;

	    static const float y_pos = position * 100 + 200;
	    static const float x_pos = *Pointers.ScreenResX - 470;
	    auto& IO = ImGui::GetIO();

	    if (m_Messages.size() && m_Opacity >= 0.011f)
	    {
	    	// float size = _ChatFontSize.GetState();
	    	// if (std::abs(size - m_Size) > 0.1f)
	    	// {
	    		// LOG(VERBOSE) << "Updating chat size: " << size;

	    		// auto& io = ImGui::GetIO();

	    		// m_CurrentFont = CreateFontWithCyrillicSupport(io, size);

	    		// io.Fonts->Build();

	    		// m_Size = size;
	    	// }


	        float width = *Pointers.ScreenResX - x_pos - 10;
	        float height = 250.f; // previously full screen; set to a fixed region height

	        ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
	        ImGui::SetNextWindowPos(ImVec2(x_pos, y_pos), ImGuiCond_Always);

	        ImGuiWindowFlags flags =
	            ImGuiWindowFlags_NoTitleBar |
	            ImGuiWindowFlags_NoResize |
	            ImGuiWindowFlags_NoMove |
	            ImGuiWindowFlags_NoCollapse |
	            ImGuiWindowFlags_NoSavedSettings |
	            ImGuiWindowFlags_NoScrollbar |
	            ImGuiWindowFlags_NoScrollWithMouse |
	            ImGuiWindowFlags_NoInputs;

	        ImVec4 bg = ImVec4(0.0f, 0.0f, 0.0f, m_Opacity * 0.6f);
	        ImGui::PushStyleColor(ImGuiCol_WindowBg, bg);

	        bool opened = ImGui::Begin("##chatwin", nullptr, flags);

	        if (opened)
	        {
	            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
	        	if (m_CurrentFont)
	        		ImGui::PushFont(m_CurrentFont);
	        	else
	        		ImGui::PushFont(Menu::Font::g_ChatFont);

	            for (auto &message : m_Messages)
	            {
	                auto color = message.m_Color;
	                color.Value.w = m_Opacity;
	                ImGui::PushStyleColor(ImGuiCol_Text, color.Value);
	                ImGui::TextWrapped("%s:", message.m_Sender.c_str());
	                ImGui::PopStyleColor();
	                ImGui::SameLine();
	                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, m_Opacity));
	                ImGui::TextWrapped("%s", message.m_Message.c_str());
	                ImGui::PopStyleColor();
	            }

	            ImGui::PopFont();
	            ImGui::PopStyleColor();
	        }

	        ImGui::End();
	        ImGui::PopStyleColor();
	    }

		// TODO: unlink fade speed with FPS
	    if (std::chrono::system_clock::now() - m_LastAccessTime > 15s && m_Opacity != 0.0f)
	    {
	        m_Opacity -= 0.01f;
	        if (m_Opacity <= 0.011f)
	            m_Opacity = 0.0f;
	    }
	}

	void ChatDisplay::ClearImpl()
	{
		m_Messages.clear();
	}

	void ChatDisplay::MarkAccessedImpl()
	{
		m_Opacity = 1.0f;
		m_LastAccessTime = std::chrono::system_clock::now();
	}


	// class UpdateFontSize : public LoopedCommand
	// {
		// using LoopedCommand::LoopedCommand;
		// float m_size = 0;

		// static const ImWchar* GetGlyphRangesCyrillicOnly()
		// {
		// 	static const ImWchar ranges[] =
		// 		{
		// 		0x0400,
		// 		0x052F, // Cyrillic + Cyrillic Supplement
		// 		0x2DE0,
		// 		0x2DFF, // Cyrillic Extended-A
		// 		0xA640,
		// 		0xA69F, // Cyrillic Extended-B
		// 		0,
		// 	};
		// 	return &ranges[0];
		// }
		//
		// static ImFont* CreateFontWithCyrillicSupport(ImGuiIO& io, float size)
		// {
		// 	ImFontConfig FontCfg{};
		// 	FontCfg.FontDataOwnedByAtlas = false;
		//
		// 	auto font = io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Fonts::MainFont), sizeof(Fonts::MainFont), size, &FontCfg, io.Fonts->GetGlyphRangesDefault());
		//
		// 	// just use Arial for Cyrillic
		//
		// 	FontCfg.MergeMode = true;
		// 	io.Fonts->AddFontFromFileTTF((std::filesystem::path(std::getenv("SYSTEMROOT")) / "Fonts" / "arial.ttf").string().c_str(), size, &FontCfg, GetGlyphRangesCyrillicOnly());
		//
		// 	io.Fonts->Build();
		//
		// 	return font;
		// }

	// 	void OnTick() override
	// 	{
	// 		float scale = _ChatFontSize.GetState();
	// 		if (scale != m_size)
	// 		{
	// 			m_size = scale;
	// 			auto& IO = ImGui::GetIO();
	// 			Menu::Font::g_ChatFont = CreateFontWithCyrillicSupport(IO, scale);
	// 		}
	// 	}
	// };
	//
	// static UpdateFontSize _UpdateFontSize{
	// 	"updatefontsizeloop",
	// 	"[internal] Update font size loop",
	// 	"Updates global font"};
}