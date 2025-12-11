#pragma once
#include "game/pointers/Pointers.hpp" // game import in core

#include <mutex>


namespace YimMenu
{
	struct Message
	{
		std::string m_Sender;
		std::string m_Message;
		ImColor m_Color;
	};

	class ChatDisplay
	{
	private:
		std::chrono::system_clock::time_point m_LastAccessTime;
		float m_Opacity = 1.0f;
		std::vector<Message> m_Messages = {};
		std::mutex m_Mutex;
		float m_Size = 0;
		ImFont* m_CurrentFont = nullptr;

		void ShowImpl(std::string sender, std::string message, ImColor color);
		const ImWchar* GetGlyphRangesCyrillicOnly();
		ImFont* CreateFontWithCyrillicSupport(ImGuiIO& io, float size);
		void DrawImpl();
		void ClearImpl();
		void MarkAccessedImpl();

		static ChatDisplay& GetInstance()
		{
			static ChatDisplay instance;
			return instance;
		}

	public:
		static void Show(std::string sender, std::string message, ImColor color)
		{
			return GetInstance().ShowImpl(sender, message, color);
		}

		static void Draw()
		{
			GetInstance().DrawImpl();
		}

		static void Clear()
		{
			GetInstance().ClearImpl();
		}

		static void MarkAccessed()
		{
			GetInstance().MarkAccessedImpl();
		}
	};

	static bool g_ChatInputActive = false;
	static char g_ChatBuffer[256] = {};
}