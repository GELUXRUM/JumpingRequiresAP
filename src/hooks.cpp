#include "hooks.h"

#define A_THIRD 1.0f/3.0f

namespace Utils
{
	float JumpCostCalculator(RE::Actor* a_actor)
	{
		float calculatedJumpCost{ 0.0f };

		float maxAP = a_actor->GetBaseActorValue(*avSingleton->actionPoints);

		if (modConfigs.jumpCost <= 1.0f && modConfigs.jumpCost > 0.0f) {
			calculatedJumpCost += modConfigs.jumpCost * maxAP;
		} else {
			calculatedJumpCost += modConfigs.jumpCost;
		}

		int myLegOwMyLeg{ 0 };

		if (a_actor->GetActorValue(*avSingleton->leftMobiltyCondition) == 0.0f || a_actor->GetActorValue(*avSingleton->rightMobilityCondition) == 0.0f) {
			myLegOwMyLeg = 1;
		}

		calculatedJumpCost += calculatedJumpCost * (myLegOwMyLeg * modConfigs.multPerLegCrippled);

		return calculatedJumpCost;
	}
}

namespace Hooks
{
	typedef uint8_t(Actor_GetMobilityCrippledSig)(RE::Actor*);
	REL::Relocation<Actor_GetMobilityCrippledSig> CrippleFunction;

	typedef void(bhkCharacterController_JumpSig)(RE::bhkCharacterController*, float);
	REL::Relocation<bhkCharacterController_JumpSig> JumpFunction;
	
	uint8_t HookedActor_GetMobilityCrippled(RE::Actor* a_actor)
	{
		switch (modConfigs.jumpStyle) {
		case 1:
			{
				if (a_actor->GetActorValue(*avSingleton->actionPoints) < Utils::JumpCostCalculator(a_actor)) {
					return 2;  // hopefully the user doesn't have some weird-ass mod to make the player have more than 2 legs WeirdChamp
				}
				break;
			}
		case 2:
			{
				if (a_actor->GetActorValue(*avSingleton->actionPoints) < (A_THIRD * Utils::JumpCostCalculator(a_actor))) {
					return 2;
				}
				break;
			}
		}

		return CrippleFunction(a_actor);
	}

	// technically the a_jumpHeight is multiplied by another value, but if we apply a mult to it
	// then it should behave fine
	void HookedbhkCharacterController_Jump(RE::bhkCharacterController* a_controller, float a_jumpHeight)
	{
		static RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		float jumpCost = Utils::JumpCostCalculator(player);
		float currentAP = player->GetActorValue(*avSingleton->actionPoints);

		if (modConfigs.jumpStyle == 2) {
			if (currentAP < jumpCost) {
				float jumpDebuff = currentAP / jumpCost;
				// we don't want our jump height to be lower than 33%
				if (jumpDebuff < A_THIRD) {
					jumpDebuff = A_THIRD;
				}

				a_jumpHeight = a_jumpHeight * jumpDebuff;
			}
		}

		player->ModActorValue(RE::ACTOR_VALUE_MODIFIER::Damage, *avSingleton->actionPoints, -1.0f * jumpCost);
		
		return JumpFunction(a_controller, a_jumpHeight * modConfigs.jumpHeightMult);
	}

	void Install(F4SE::Trampoline& trampoline) {
		REL::Relocation<Actor_GetMobilityCrippledSig> crippleLocation{ REL::ID(1394576), 0x17 };
		CrippleFunction = trampoline.write_call<5>(crippleLocation.address(), &HookedActor_GetMobilityCrippled);

		REL::Relocation<bhkCharacterController_JumpSig> jumpLocation{ REL::ID(1394576), 0x1DE };
		JumpFunction = trampoline.write_call<5>(jumpLocation.address(), &HookedbhkCharacterController_Jump);
	}
}
