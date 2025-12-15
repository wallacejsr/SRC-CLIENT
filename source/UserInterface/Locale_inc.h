#pragma once

//////////////////////////////////////////////////////////////////////////
// ### Default Ymir Macros ###
#define LOCALE_SERVICE_EUROPE
#define ENABLE_COSTUME_SYSTEM
// #define ENABLE_ENERGY_SYSTEM
// #define ENABLE_DRAGON_SOUL_SYSTEM
#define ENABLE_NEW_EQUIPMENT_SYSTEM
// ### Default Ymir Macros ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### New From LocaleInc ###
#define ENABLE_PACK_GET_CHECK
#define ENABLE_CANSEEHIDDENTHING_FOR_GM
#define ENABLE_PROTOSTRUCT_AUTODETECT
#define DISABLE_FONT_ITALIC_GM

// #define ENABLE_PLAYER_PER_ACCOUNT5
#define ENABLE_LEVEL_IN_TRADE
// #define ENABLE_DICE_SYSTEM
#define ENABLE_EXTEND_INVEN_SYSTEM
#define ENABLE_ARMOR_EFFECT					// Efeitos nas Armaduras
#define USE_BODY_COSTUME_WITH_EFFECT		// Efeitos dos Trajes
#define BODY_COSTUME_WITH_EFFECT_IMPERIAL	// Efeito no Traje Imperial
#define BODY_COSTUME_WITH_EFFECT_GVG		// Efeito no Traje GvG
#define WEAPON_COSTUME_CHRISTMAS_EFFECT		// Efeito Skin de Armas PvP
#define ENABLE_SLOT_WINDOW_EX
#define ENABLE_TEXT_LEVEL_REFRESH
#define ENABLE_USE_COSTUME_ATTR

#define WJ_SHOW_MOB_INFO
#ifdef WJ_SHOW_MOB_INFO
#define ENABLE_SHOW_MOBAIFLAG
#define ENABLE_SHOW_MOBLEVEL
#endif
// ### New From LocaleInc ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### From GameLib ###
// #define ENABLE_WOLFMAN_CHARACTER

// #define ENABLE_MAGIC_REDUCTION_SYSTEM
#define ENABLE_MOUNT_COSTUME_SYSTEM
#define ENABLE_WEAPON_COSTUME_SYSTEM
// #define ENABLE_ACCE_COSTUME_SYSTEM
// ### From GameLib ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### From EterLib ###
#define ENABLE_SEQUENCE_SYSTEM
// ### From EterLib ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### New System ###
#define ENABLE_SHOW_CHEST_DROP				// Sistema de Informações dos Cofres
#define ENABLE_BLACK_SCREEN_FIX				// Correção da Tela Preta
#define ENABLE_NEW_ATTRBONUS				// Sistema Novos Bônus Boss e Metins
#define ENABLE_EXTEND_SHOP_SYSTEM			// Sistema Expansão das Lojas
#define ENABLE_AUTO_PICKUP_SYSTEM			// Sistema Pega Item Automatico
#define ENABLE_SEND_TARGET_INFO				// Sistema de Informações dos Monstros
#define ENABLE_VIEW_STONE_IN_MAP			// Sistema Mostra as Metins no Mini Mapa
#define ENABLE_DETAILS_UI					// Sistema de Tabela de Bônus
#define ENABLE_NEW_LOGIN_INTERFACE			// Nova Tela de Login
#define ENABLE_INDEX_AT_SOURCE				// Ativa Index na Fonte
#define ENABLE_EMPIRE_FLAG					// Sistema de Bandeira do Reino
#define ENABLE_FOG_FIX						// Sistema de Neblina
#define NEW_SELECT_CHARACTER				// Nova Intro Select
#define ENABLE_EMOJI_SYSTEM					// Sistema de Icone Texto
#define ENABLE_CHEQUE_SYSTEM				// Sistema de Won
#define ENABLE_ACHIEVEMENT_SYSTEM			// Sistema de Conquista
#define ENABLE_MESSENGER_TEAM				// Staff Online
#define ENABLE_REFINE_RENEWAL				// Sistema Refinação Automatico
#define ENABLE_HIDE_COSTUME_SYSTEM			// Sistema de Ocultar Trajes
// #define ENABLE_HIDE_COSTUME_SYSTEM_ACCE	// Sistema de Ocultar Faixas
#define ENABLE_HIDE_COSTUME_SYSTEM_WEAPON	// Sistema de Ocultar Skins
#define ENABLE_IMMUNE_BOSS					// Faz com que os Boss não escorregue
#define ENABLE_WHISPER_FLASHING				// A janela pisca quando recebe uma mensagem
#define ENABLE_SELECT_COLOR_TARGET			// Sistema de Cor dos Mobs/NPC e Jogadores
#define ENABLE_NEW_AFFECT_POTION			// Sistema do Efeito das Poções
#define ENABLE_ERROR_PROPERTY_FILENAME		// Show folder if we have error propperty!
#define ENABLE_FIX_BUG_MINIMIZE				// Erro da Camera ao Minimizar o Client
#define ENABLE_REGISTER_EFFECT_LOG			// Register effect to log.txt
#define FIX_CHECK_EMPTY_TEXTURESET			// Fix textureset is empty!
#define ENABLE_CHEQUE_TO_GOLD				// Sistema de Trocar Won por Gold
#define ENABLE_NEW_EXCHANGE_WINDOW			// Sistema de Nova Negociação
// #define ENABLE_MAX_FPS						// Aumenta o Máximo do FPS
#define ENABLE_HWID_BAN_SYSTEM				// Banimento por HWID
#define ENABLE_SYSTEM_MT2				// Sistemas Darkyn2
#define DISABLE_TASKBAR_GROUPING			// Janelas separadas
#define ENABLE_FIX_SKYBOX					// Não lembro

#define ENABLE_GUILD_STATISTICS				// Sistema de Estatísticas de Guilda
#define ENABLE_WAR_KILL_COUNTER				// Sistema que Mostra os Kills na Guerra de Guilda
#define ENABLE_WAR_INSIGNIA					// Guerra das Insígnias
#define ENABLE_SOULBIND_SYSTEM				// Sistema de Bloquear Itens
#define ENABLE_KINGDOMS_WAR					// Guerra Imperial

// Protection
#define BLOCK_WRITE_PROCESS_MEMORY			// Proteção contra bloqueio de processo
// #define BLOCK_CHANGE_NAME_BIN				// Proteção contra alteração do nome do binario
// #define ENABLE_SVSIDE						// Proteção svside
// #define ENABLE_SVSIDE_PLUS					// Proteção svside PLUS

#define ENABLE_EXTRA_SECURITY	
#ifdef ENABLE_EXTRA_SECURITY
enum eExtraSecurity {
	DUMMY_LOGIN = 1,
	DUMMY_ATTACK = 1
};
#endif

// #define ENABLE_MELEY_LAIR_DUNGEON
#ifdef ENABLE_MELEY_LAIR_DUNGEON
	#define MELEY_LAIR_DUNGEON_STATUE 6118
#endif

#define ENABLE_OFFLINE_SHOP_SYSTEM
//#define ENABLE_OFFLINE_SHOP_DEBUG
#define ENABLE_OFFLINE_SHOP_IN_CITIES
#if defined(ENABLE_OFFLINE_SHOP_DEBUG) && defined(_DEBUG)
#define OFFSHOP_DEBUG(fmt , ...) Tracenf("%s:%d >> " fmt , __FUNCTION__ , __LINE__, __VA_ARGS__)
#else
#define OFFSHOP_DEBUG(...)   
#endif

/*******************************************************************/
/*** sistemas Macros ***/
/*******************************************************************/
/*******************************************************************/
#define ENABLE_BONUS_SWITCHER
#ifdef ENABLE_BONUS_SWITCHER
#define ENABLE_SWITCH_ITEM_SELECT
#define ENABLE_SWITCH_BUY_ITEM_NPC 									// update 10-10-23
#endif
/*******************************************************************/
#define ENABLE_NOVA_ENCRIPTACION
#define __INDEX_FROM_SOURCE__
/*******************************************************************/
#define ENABLE_SPECIAL_INVENTORY									// Inventario especial. 			by: Sanii
#define ENABLE_ITEMSHOP
#define ENABLE_RANKING_SYSTEM
#define __AUTO_SKILL_READER__
#define ENABLE_SYSTEM_RUNE
#define ENABLE_HUNTING_SYSTEM
#define ENABLE_DYNAMIC_FONT
#define ENABLE_DAILY_GIFT_SYSTEM /// < Daily gifts
/*******************************************************************/
#define GUILD_WAR_COUNTER
#define __IMPROVED_GUILD_WAR__
#define __BL_KILL_BAR__
/*******************************************************************/
#define ENABLE_QUEST_RENEWAL // Quest page renewal
#define ENABLE_QUEST_WIDTH_EXPANSION
/*******************************************************************/
#define ENABLE_AUTO_REFINE
#define ENABLE_PVP_EFFECT_SYSTEM
#define ENABLE_SHINING_SYSTEM
#define __AUTO_HUNT__
#define UNBLOCK_SYSTEM_ENABLED
/*******************************************************************/
#define ENABLE_MOBS_WITHOUT_COLLISIONS
#define ENABLE_BADGE_NOTIFICATION_MANAGER
#define ENABLE_SOUL_SYSTEM
#define ENABLE_SOUL_SYSTEM_UPDATE
#define ENABLE_ANTI_EXP
/*******************************************************************/
#define ENABLE_SHOP_RENEWAL				// Sistema de Compra com Itens
/*******************************************************************/
#define ENABLE_EXTENDED_SOCKETS
#define ENABLE_ASLAN_BUFF_NPC_SYSTEM_STOP_COLLISION

#define ENABLE_ASLAN_BUFF_NPC_SYSTEM
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	#define ASLAN_BUFF_NPC_ENABLE_EXPORT
	#define ASLAN_BUFF_NPC_ENABLE_EXPORT_COST
	#define ASLAN_BUFF_NPC_USE_SKILL_TECH_LEVEL
#endif
/*******************************************************************/
#define _XTREME2_PROTECTION_
/*******************************************************************/
/*** 08-10-2023 ***/
#define ENABLE_MESSENGER_TEAM
#define ENABLE_GM_LIST
#define PLAYERS_ONLINE_LIST_TO_GM
/*******************************************************************/
/*** 10-10-2023 ***/
////////////////STANDART SHIT U KNOW///////////////////
//#define __ANTI_GM_DETECTOR__
#define __ANTI_FAKE_ITEM__ 			// não remover otario
//#define __ANTI__SPEEDHACK__
//#define __ANTI__M2BOB__
//#define ENABLE_BARABARABARA_BEREBEREBERE
// #define ENABLE_CUSTOM_SPECULAR_MAP
/*******************************************************************/
/*** 11-10-2023 ***/
#define __CAPTCHA__
/*******************************************************************/
/*** 12-10-2023 ***/
#define ENABLE_BOSS_EFFECT_SYSTEM /// < Boss Effects
/*******************************************************************/
/*** 13-10-2023 ***/
#define WORLD_BOSS_YUMA
#define __ENABLE_SHAMAN_ATTACK_SPEED_FIX_MAJESTE__

//New by ondry
#define ENABLE_BATTLE_ROYALE
/*******************************************************************/
/* Halloween */ 
// #define __ENABLE_VALENTINE_EVENT__
/* Halloween */ 
/*******************************************************************/
/*** 20-10-2023 ***/
#define ENABLE_EXCHANGE_LOG
#define ENABLE_SET_CUSTOM_ATTRIBUTE_SYSTEM
#define ENABLE_DUNGEON_BOSS_ICON_IN_MAP
/*******************************************************************/
/*** 20-10-2023 ***/
#define ENABLE_RESP_SYSTEM
#define __SPIN_WHEEL__
#define INSIDE_RENDER
/*******************************************************************/
#define ENABLE_CHECK_FILES_MD5				// Faz verificação dos arquivos em md5
/*******************************************************************/
/*******************************************************************/
//////////////////////////////////////////////////////////////////////////