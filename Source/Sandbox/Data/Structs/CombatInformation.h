#pragma once


#include "CoreMinimal.h"
#include "CombatInformation.generated.h"







/*


	Standard
	Slash
	Pierce
	Strike


	Magic
	Ice
	Fire
	Holy
	Lightning

	

	Ranged
		- Standard
		- Armor Piercing
		- Hollow Point
		- Incendiary
		- Cryo
		- Explosive
		- Frangible



	Weapon
		- Ranged Information / Damage
			- Weapon / Attachment config
			- Ranged damage information
			
		- Melee Information / Damage
			- Combo retrieval
			- Melee damage information

	->
	-> Damage information (varying attribute set modifiers)

	
	Weapon
		- Equip slot mapping
		- Anim montage mapping
		- Ability mapping
		- Character to Weapon information mapping
		- 







	Combat Component
		- Attribute damage calculation



	Combat
		- Weapon retrieves it's attack information
			- Branching logic is okay here, it ends up adjusting attributes which can easily be added to both
		- Weapon creates a damage calculation and sends it to the attribute logic
		- AttributeLogic handles adjusting attributes
		



 
*/

