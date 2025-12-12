#include "core/commands/Command.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class RagdollSelf : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!Self::GetPed().GetHandle())
			{
				Notifications::Show("Ragdoll", "Player ped not found.");
				return;
			}
			Self::GetPed().SetRagdoll(true);
			auto ped = Self::GetPed().GetHandle();
			PED::SET_PED_TO_RAGDOLL(ped, 3000, 0, 0, true, true, false);
			ENTITY::APPLY_FORCE_TO_ENTITY(ped, 1, 10, 10, 10, 0, 0, 0, 0, false, true, true, false, true);
		}
	};

	static RagdollSelf _RagdollSelf{"ragdollself", "Ragdoll", "Ragdoll yourself"};
}
