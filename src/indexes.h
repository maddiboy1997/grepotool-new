#ifndef ___DATAINDEXES_H
#define ___DATAINDEXES_H
/* dataindexes.h  - enumerations for data

Copyright (c) 2013 P. Vorpahl

This file is part of Grepotool, a free utility for the browsergame 
Grepolis which is the intellectual property of InnoGames GmbH.

Grepotool is free Software: you can redistribute it and/or modify 
it under the terms of the GNU Lesser General Public License as 
published by the Free Software Foundation, either version 3 of the 
License, or any later version. Alternatively, you may use it under 
the terms of the GNU General Public License as published by the Free 
Software Foundation, either version 3 of the License, or any later 
version.

Grepotool is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License 
along with 'Grepotool; see the files 'GPL.txt' and 'LGPL.txt' in 
the base directory of your installation. If not, see 
'http://www.gnu.org/licenses/'.
*/
enum UnitIndex {
	Militia,
	Sword,
	Slinger,
	Archer,
	Hoplite,
	Rider,
	Chariot,
	Kata,
	Transporter,
	Bireme,
	Attackship,
	Brander,
	FastTransporter,
	Trireme,
	Kolo,
	Minotaur,
	Manti,
	Zyklop,
	Hydra,
	Harpy,
	Medusa,
	Centaur,
	Pegasus,
	Zerberus,
	Eriny,
	Griffin,
	Kalydonian,
	Godsent,
	UnitMax
};

enum TownTypeIndex {
	TownTypeUndefined, // circle
	TownTypeAtt,       // rect  
	TownTypeDeff,      // Triangle
	TownTypeAttShip,   // raute
	TownTypeAttKolo,   // rect+hat
	TownTypeBireme,    // Tri down
	TownTypeManti,     // star
	TownTypePega,      // Rect with slash
	TownTypeHarpy,    //  rect with backslash
	TownTypeMax        // 
};

enum AttackTypeIndex {
	Hack,
	Pierce,
	Distance,
	Naval
};

enum GodIndex {
	AnyGod,
	Zeus,
	Poseidon,
	Hera,
	Athene,
	Hades,
	Artemis
};

enum RessiIndex {
	Wood,
	Stone,
	Silver
};

enum IconIndex {
	IconPlayer,
	IconIsland,
	IconTown,
	IconAlly,
	IconPoints,
	IconDeffHelmet,
	IconDeffArmour,
	IconDeffShield,
	IconOffSpike,
	IconOffSword,
	IconOffArrow,
	IconRessis,
	IconOffSea,
	IconDeffSea,
	IconDivine,  // Points
	IconDeffBash,
	IconOffBash,
	IconTotalBash,
	IconMax
};


enum PremiumIndex {
	Counselor = 1,
	Trader=2,
	HighPriest=4,
	Captain=8,
	Commander=16
};

enum WonderIndex {
	ZeusStatue = 1,
	ArtemisTemple = 2,
	Mausoleum   = 4,
	KolossOfRhodos = 8,
	AlexandrianLighthouse = 16,
	Pyramid = 32,
	Gardens = 64
};

enum BuildingIndex {
	
	Ironer,
	Lumber,
	Stoner,
	Senat,
	Barracks,
	Wall,
	Market,
	Temple,
	Academy,
	Storage,
	Hide,
	Farm,
	Agora,
	Docks,
	Theater,
	Therme,
	Library,
	Lighthouse,
	Tower,
	Oracle,
	TradeOffice,
	Statue,
	BuildingMax
};

enum ResearchIndex {
	ResearchSlinger,
	ResearchArcher,
	ResearchHoplite,
	ResearchCitiGuard,
	ResearchDiplo,
	ResearchSpy,
	ResearchBounty,
	ResearchCeramics,
	ResearchRider,
	ResearchArchitecture,
	ResearchTrainer,
	ResearchBireme,
	ResearchCrane,
	ResearchMeteorology,
	ResearchChariot,
	ResearchAttackship,
	ResearchConscription,
	ResearchBoatBuilder,
	ResearchBrander,
	ResearchCatapult,
	ResearchCryptografy,
	ResearchColoship,
	ResearchFastTrans,
	ResearchPlough,
	ResearchBunks,
	ResearchTrireme,
	ResearchPhalanx,
	ResearchBreakthrough,
	ResearchMathematics,
	ResearchRam,
	ResearchCartography,
	ResearchConquering,

	ResearchStoneHail,
	ResearchTempleLooting,
	ResearchGodlySelection,
	ResearchExperience,
	ResearchStrongWine,
	ResearchSail,

	ResearchMax
};

enum SpellIndex {
	// Zeus
	GodsSign,
	Lightning,
	GoodWind,
	ZeusRage,
	// Poseidon
	GiftOfSea,
	CallofSea,
	Earthquake,
	SeeStorm,
	// Hera
	Marriage,
	Happyness,
	Babyboom,
	Whishfullness,
	// Athene
	GuardianGoddess,
	Wisdom,
	Protection,
	HeroPower,
	// Hades
	Pest,
	ReturnFromUnderworld,
	UnderworldTreasure,
	HelmetOfInvisibility,
	// Artemis
	GiftOfNature,
	HountressDesire,
	Illusion,
	Purification,

	SpellMax
};

#endif