#include "StatContainer.h"

bool UStatContainer::CheckIfCanUpgrade(float money)
{
	if (CurrentUpgradeCount + 1 >= Upgrades.Num())
		return false;

	return Upgrades[CurrentUpgradeCount].UpgradePrice <= money;
}

void UStatContainer::Upgrade()
{
	CurrentUpgradeCount = FMath::Clamp(CurrentUpgradeCount + 1, 0, Upgrades.Num()-1);
}

bool UStatContainer::TryUpgrade(float money)
{
	if (!CheckIfCanUpgrade(money))
		return false;

	Upgrade();
	return true;
}