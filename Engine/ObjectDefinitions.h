#ifndef __OBJECT_DEFINITIONS_H__
#define __OBJECT_DEFINITIONS_H__

/* ------------------ Object Class ID Definitions ----------------- */
#define	ITEM_GAP_ID							0
#define	ITEM_SOLID_TERRAIN_ID				1
#define	ITEM_ROCK_ID						2
#define	ITEM_DIGGER_ID						3
#define	ITEM_HOBBLIN_ID						4
#define	ITEM_NOBBLIN_ID						5
#define	ITEM_GOLD_ID						6
#define	ITEM_EMERALD_ID						7
#define	ITEM_RUBY_ID						8
#define	ITEM_SAPHIRE_ID						9
#define	ITEM_BONUS_LIFE_ID					10
#define	ITEM_BONUS_FATAL_ID					11
#define ITEM_ALPHA_MASK_ID					12
#define ITEM_GRAVE_ID						13
#define ITEM_LABEL_LEVEL_ID					14
#define ITEM_LABEL_SCORE_ID					15
#define ITEM_LABEL_NUMBER_ID				16
#define ITEM_LABEL_HEALTH_ID				17
#define ITEM_SOUND_DEATH_ID					18
#define ITEM_SOUND_SMASH_ID					19
#define ITEM_SOUND_PICK_GOLD_ID				20
#define ITEM_SOUND_PICK_EMERALD_ID			21
#define ITEM_GAME_OVER_ID					22
#define ITEM_VICTORY_ID						23
#define ITEM_PAUSED_ID						24
#define ITEM_INTRO_ID						25
#define ITEM_AUTHORS_ID						26
/* ---------------------------------------------------------------- */

#define ITEM_STR_DIGGER_PREFIX				"IconSet/Characters/Digger/D_"
#define ITEM_STR_HOBBLIN_PREFIX				"IconSet/Characters/Hobblin/H_"
#define ITEM_STR_NOBBLIN_PREFIX				"IconSet/Characters/Nobblin/N_"
#define ITEM_STR_SOLID_TERRAIN_PREFIX		"IconSet/Terrain/Ground/16x16/G_"
#define ITEM_STR_ROCK_PREFIX				"IconSet/Terrain/Obstacles/Solid/Rock_"
#define	ITEM_STR_EMERALD_PREFIX				"IconSet/TerrainObjects/64x64/EmeraldAnimation/E_"
#define	ITEM_STR_RUBY_PREFIX				"IconSet/TerrainObjects/64x64/RubyAnimation/R_"
#define	ITEM_STR_SAPHIRE_PREFIX				"IconSet/TerrainObjects/64x64/SaphireAnimation/S_"
#define	ITEM_STR_GOLD_PREFIX				"IconSet/TerrainObjects/64x64/Gold/Gold_"
#define	ITEM_STR_BONUS_LIFE_PREFIX			"IconSet/TerrainObjects/64x64/BI/oiltank"
#define	ITEM_STR_BONUS_FATAL_PREFIX			"IconSet/TerrainObjects/64x64/BI/shield"
#define	ITEM_STR_ALPHA_MASK_PREFIX			"IconSet/Terrain/Mask"
#define ITEM_STR_GRAVE_PREFIX				"IconSet/Characters/Graves/Grave_0"
#define ITEM_STR_CNTRL_PREFIX				"IconSet/Controls/"
#define ITEM_STR_LOGO_PREFIX				"IconSet/Logo/a_"
#define ITEM_STR_CNTRL_LEVEL_PREFIX			"level"
#define ITEM_STR_CNTRL_SCORE_PREFIX			"score"
#define ITEM_STR_CNTRL_HEALTH_PREFIX		"health"
#define ITEM_STR_GAME_OVER_PREFIX			"game_over"
#define ITEM_STR_VICTORY_PREFIX				"you_win"
#define ITEM_STR_PAUSED_PREFIX				"game_paused"
#define ITEM_STR_AUTHORS_PREFIX				"author_"
#define ITEM_STR_INTRO_PREFIX				"Countdown/"
#define ITEM_STR_SOUND_PREFIX				"Sound/"
#define ITEM_STR_SOUND_DEATH				"death"
#define ITEM_STR_SOUND_SMASH				"smash_chest"
#define ITEM_STR_SOUND_PICK_GOLD			"pick_gold"
#define ITEM_STR_SOUND_PICK_EMERALD			"pick_emerald"
#define ITEM_STR_POSTFIX					".tga"
#define ITEM_STR_SOUND_POSTFIX				".wav"

#define ITEM_DIGGER_NUM_MIN					0
#define ITEM_DIGGER_NUM_MAX					4
#define ITEM_HOBBLIN_NUM_MIN				0
#define ITEM_HOBBLIN_NUM_MAX				16
#define ITEM_NOBBLIN_NUM_MIN				0
#define ITEM_NOBBLIN_NUM_MAX				16
#define ITEM_ROCK_NUM_MIN					1
#define ITEM_ROCK_NUM_MAX					7
#define ITEM_EMERALD_NUM_MIN				0
#define ITEM_EMERALD_NUM_MAX				8
#define ITEM_RUBY_NUM_MIN					0
#define ITEM_RUBY_NUM_MAX					8
#define ITEM_SAPHIRE_NUM_MIN				0
#define ITEM_SAPHIRE_NUM_MAX				8
#define ITEM_GOLD_NUM_MIN					0
#define ITEM_GOLD_NUM_MAX					4
#define ITEM_GOLD_PACKED_STATE				0
#define ITEM_GOLD_FALLING_STATE				1
#define ITEM_GOLD_UNPACKED_STATE			2

#define GOLD_MONEY							100
#define EMERALD_MONEY						20
#define RUBY_MONEY							30
#define SAPHIRE_MONEY						50
#endif /* __OBJECT_DEFINITIONS_H__ */