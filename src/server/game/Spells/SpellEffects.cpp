/*
* Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
* Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "DatabaseEnv.h"
#include "WorldPacket.h"
#include "Opcodes.h"
#include "Log.h"
#include "UpdateMask.h"
#include "World.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Player.h"
#include "SkillExtraItems.h"
#include "Unit.h"
#include "Spell.h"
#include "DynamicObject.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "Group.h"
#include "UpdateData.h"
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "SharedDefines.h"
#include "Pet.h"
#include "GameObject.h"
#include "GossipDef.h"
#include "Creature.h"
#include "Totem.h"
#include "CreatureAI.h"
#include "BattlegroundMgr.hpp"
#include "Battleground.h"
#include "BattlegroundEY.h"
#include "BattlegroundWS.h"
#include "BattlegroundTP.h"
#include "OutdoorPvPMgr.h"
#include "Language.h"
#include "SocialMgr.h"
#include "Util.h"
#include "VMapFactory.h"
#include "TemporarySummon.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "SkillDiscovery.h"
#include "Formulas.h"
#include "Vehicle.h"
#include "ScriptMgr.h"
#include "GameObjectAI.h"
#include "AccountMgr.h"
#include "InstanceScript.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "ArchaeologyMgr.h"
#include "GarrisonMgr.hpp"
#include "PetBattle.h"

pEffect SpellEffects[TOTAL_SPELL_EFFECTS] =
{
    &Spell::EffectNULL,                                     //  0
    &Spell::EffectInstaKill,                                //  1 SPELL_EFFECT_INSTAKILL
    &Spell::EffectSchoolDMG,                                //  2 SPELL_EFFECT_SCHOOL_DAMAGE
    &Spell::EffectDummy,                                    //  3 SPELL_EFFECT_DUMMY
    &Spell::EffectUnused,                                   //  4 SPELL_EFFECT_PORTAL_TELEPORT          unused
    &Spell::EffectTeleportUnits,                            //  5 SPELL_EFFECT_TELEPORT_UNITS
    &Spell::EffectApplyAura,                                //  6 SPELL_EFFECT_APPLY_AURA
    &Spell::EffectEnvironmentalDMG,                         //  7 SPELL_EFFECT_ENVIRONMENTAL_DAMAGE
    &Spell::EffectPowerDrain,                               //  8 SPELL_EFFECT_POWER_DRAIN
    &Spell::EffectHealthLeech,                              //  9 SPELL_EFFECT_HEALTH_LEECH
    &Spell::EffectHeal,                                     // 10 SPELL_EFFECT_HEAL
    &Spell::EffectBind,                                     // 11 SPELL_EFFECT_BIND
    &Spell::EffectNULL,                                     // 12 SPELL_EFFECT_PORTAL
    &Spell::EffectUnused,                                   // 13 SPELL_EFFECT_RITUAL_BASE              unused
    &Spell::EffectUnused,                                   // 14 SPELL_EFFECT_RITUAL_SPECIALIZE        unused
    &Spell::EffectUnused,                                   // 15 SPELL_EFFECT_RITUAL_ACTIVATE_PORTAL   unused
    &Spell::EffectQuestComplete,                            // 16 SPELL_EFFECT_QUEST_COMPLETE
    &Spell::EffectWeaponDmg,                                // 17 SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL
    &Spell::EffectResurrect,                                // 18 SPELL_EFFECT_RESURRECT
    &Spell::EffectAddExtraAttacks,                          // 19 SPELL_EFFECT_ADD_EXTRA_ATTACKS
    &Spell::EffectUnused,                                   // 20 SPELL_EFFECT_DODGE                    one spell: Dodge
    &Spell::EffectUnused,                                   // 21 SPELL_EFFECT_EVADE                    one spell: Evade (DND)
    &Spell::EffectParry,                                    // 22 SPELL_EFFECT_PARRY
    &Spell::EffectBlock,                                    // 23 SPELL_EFFECT_BLOCK                    one spell: Block
    &Spell::EffectCreateItem,                               // 24 SPELL_EFFECT_CREATE_ITEM
    &Spell::EffectUnused,                                   // 25 SPELL_EFFECT_WEAPON
    &Spell::EffectUnused,                                   // 26 SPELL_EFFECT_DEFENSE                  one spell: Defense
    &Spell::EffectPersistentAA,                             // 27 SPELL_EFFECT_PERSISTENT_AREA_AURA
    &Spell::EffectSummonType,                               // 28 SPELL_EFFECT_SUMMON
    &Spell::EffectLeap,                                     // 29 SPELL_EFFECT_LEAP
    &Spell::EffectEnergize,                                 // 30 SPELL_EFFECT_ENERGIZE
    &Spell::EffectWeaponDmg,                                // 31 SPELL_EFFECT_WEAPON_PERCENT_DAMAGE
    &Spell::EffectTriggerMissileSpell,                      // 32 SPELL_EFFECT_TRIGGER_MISSILE
    &Spell::EffectOpenLock,                                 // 33 SPELL_EFFECT_OPEN_LOCK
    &Spell::EffectSummonChangeItem,                         // 34 SPELL_EFFECT_SUMMON_CHANGE_ITEM
    &Spell::EffectApplyAreaAura,                            // 35 SPELL_EFFECT_APPLY_AREA_AURA_PARTY
    &Spell::EffectLearnSpell,                               // 36 SPELL_EFFECT_LEARN_SPELL
    &Spell::EffectUnused,                                   // 37 SPELL_EFFECT_SPELL_DEFENSE            one spell: SPELLDEFENSE (DND)
    &Spell::EffectDispel,                                   // 38 SPELL_EFFECT_DISPEL
    &Spell::EffectUnused,                                   // 39 SPELL_EFFECT_LANGUAGE
    &Spell::EffectDualWield,                                // 40 SPELL_EFFECT_DUAL_WIELD
    &Spell::EffectJump,                                     // 41 SPELL_EFFECT_JUMP
    &Spell::EffectJumpDest,                                 // 42 SPELL_EFFECT_JUMP_DEST
    &Spell::EffectTeleUnitsFaceCaster,                      // 43 SPELL_EFFECT_TELEPORT_UNITS_FACE_CASTER
    &Spell::EffectLearnSkill,                               // 44 SPELL_EFFECT_SKILL_STEP
    &Spell::EffectPlayMovie,                                // 45 SPELL_EFFECT_PLAY_MOVIE
    &Spell::EffectUnused,                                   // 46 SPELL_EFFECT_SPAWN clientside, unit appears as if it was just spawned
    &Spell::EffectTradeSkill,                               // 47 SPELL_EFFECT_TRADE_SKILL
    &Spell::EffectUnused,                                   // 48 SPELL_EFFECT_STEALTH                  one spell: Base Stealth
    &Spell::EffectUnused,                                   // 49 SPELL_EFFECT_DETECT                   one spell: Detect
    &Spell::EffectTransmitted,                              // 50 SPELL_EFFECT_TRANS_DOOR
    &Spell::EffectUnused,                                   // 51 SPELL_EFFECT_FORCE_CRITICAL_HIT       unused
    &Spell::EffectUnused,                                   // 52 SPELL_EFFECT_GUARANTEE_HIT            unused
    &Spell::EffectEnchantItemPerm,                          // 53 SPELL_EFFECT_ENCHANT_ITEM
    &Spell::EffectEnchantItemTmp,                           // 54 SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY
    &Spell::EffectTameCreature,                             // 55 SPELL_EFFECT_TAMECREATURE
    &Spell::EffectSummonPet,                                // 56 SPELL_EFFECT_SUMMON_PET
    &Spell::EffectLearnPetSpell,                            // 57 SPELL_EFFECT_LEARN_PET_SPELL
    &Spell::EffectWeaponDmg,                                // 58 SPELL_EFFECT_WEAPON_DAMAGE
    &Spell::EffectCreateRandomItem,                         // 59 SPELL_EFFECT_CREATE_RANDOM_ITEM       create item base at spell specific loot
    &Spell::EffectProficiency,                              // 60 SPELL_EFFECT_PROFICIENCY
    &Spell::EffectSendEvent,                                // 61 SPELL_EFFECT_SEND_EVENT
    &Spell::EffectPowerBurn,                                // 62 SPELL_EFFECT_POWER_BURN
    &Spell::EffectThreat,                                   // 63 SPELL_EFFECT_THREAT
    &Spell::EffectTriggerSpell,                             // 64 SPELL_EFFECT_TRIGGER_SPELL
    &Spell::EffectApplyAreaAura,                            // 65 SPELL_EFFECT_APPLY_AREA_AURA_RAID
    &Spell::EffectRechargeManaGem,                          // 66 SPELL_EFFECT_CREATE_MANA_GEM          (possibly recharge it, misc - is item ID)
    &Spell::EffectHealMaxHealth,                            // 67 SPELL_EFFECT_HEAL_MAX_HEALTH
    &Spell::EffectInterruptCast,                            // 68 SPELL_EFFECT_INTERRUPT_CAST
    &Spell::EffectDistract,                                 // 69 SPELL_EFFECT_DISTRACT
    &Spell::EffectPull,                                     // 70 SPELL_EFFECT_PULL                     one spell: Distract Move
    &Spell::EffectPickPocket,                               // 71 SPELL_EFFECT_PICKPOCKET
    &Spell::EffectAddFarsight,                              // 72 SPELL_EFFECT_ADD_FARSIGHT
    &Spell::EffectUntrainTalents,                           // 73 SPELL_EFFECT_UNTRAIN_TALENTS
    &Spell::EffectApplyGlyph,                               // 74 SPELL_EFFECT_APPLY_GLYPH
    &Spell::EffectHealMechanical,                           // 75 SPELL_EFFECT_HEAL_MECHANICAL          one spell: Mechanical Patch Kit
    &Spell::EffectSummonObjectWild,                         // 76 SPELL_EFFECT_SUMMON_OBJECT_WILD
    &Spell::EffectScriptEffect,                             // 77 SPELL_EFFECT_SCRIPT_EFFECT
    &Spell::EffectUnused,                                   // 78 SPELL_EFFECT_ATTACK
    &Spell::EffectSanctuary,                                // 79 SPELL_EFFECT_SANCTUARY
    &Spell::EffectAddComboPoints,                           // 80 SPELL_EFFECT_ADD_COMBO_POINTS
    &Spell::EffectUnused,                                   // 81 SPELL_EFFECT_CREATE_HOUSE             (unused)
    &Spell::EffectNULL,                                     // 82 SPELL_EFFECT_BIND_SIGHT
    &Spell::EffectDuel,                                     // 83 SPELL_EFFECT_DUEL
    &Spell::EffectStuck,                                    // 84 SPELL_EFFECT_STUCK
    &Spell::EffectSummonPlayer,                             // 85 SPELL_EFFECT_SUMMON_PLAYER
    &Spell::EffectActivateObject,                           // 86 SPELL_EFFECT_ACTIVATE_OBJECT
    &Spell::EffectGameObjectDamage,                         // 87 SPELL_EFFECT_GAMEOBJECT_DAMAGE
    &Spell::EffectGameObjectRepair,                         // 88 SPELL_EFFECT_GAMEOBJECT_REPAIR
    &Spell::EffectGameObjectSetDestructionState,            // 89 SPELL_EFFECT_GAMEOBJECT_SET_DESTRUCTION_STATE
    &Spell::EffectKillCreditPersonal,                       // 90 SPELL_EFFECT_KILL_CREDIT              Kill credit but only for single person
    &Spell::EffectThreatAll,                                // 91 SPELL_EFFECT_THREAT_ALL
    &Spell::EffectEnchantHeldItem,                          // 92 SPELL_EFFECT_ENCHANT_HELD_ITEM
    &Spell::EffectForceDeselect,                            // 93 SPELL_EFFECT_FORCE_DESELECT
    &Spell::EffectSelfResurrect,                            // 94 SPELL_EFFECT_SELF_RESURRECT
    &Spell::EffectSkinning,                                 // 95 SPELL_EFFECT_SKINNING
    &Spell::EffectCharge,                                   // 96 SPELL_EFFECT_CHARGE
    &Spell::EffectCastButtons,                              // 97 SPELL_EFFECT_CAST_BUTTON (totem bar since 3.2.2a)
    &Spell::EffectKnockBack,                                // 98 SPELL_EFFECT_KNOCK_BACK
    &Spell::EffectDisEnchant,                               // 99 SPELL_EFFECT_DISENCHANT
    &Spell::EffectInebriate,                                //100 SPELL_EFFECT_INEBRIATE
    &Spell::EffectFeedPet,                                  //101 SPELL_EFFECT_FEED_PET
    &Spell::EffectDismissPet,                               //102 SPELL_EFFECT_DISMISS_PET
    &Spell::EffectReputation,                               //103 SPELL_EFFECT_REPUTATION
    &Spell::EffectSummonObject,                             //104 SPELL_EFFECT_SUMMON_OBJECT_SLOT1
    &Spell::EffectSummonObject,                             //105 SPELL_EFFECT_SUMMON_OBJECT_SLOT2
    &Spell::EffectSummonObject,                             //106 SPELL_EFFECT_SUMMON_OBJECT_SLOT3
    &Spell::EffectSummonObject,                             //107 SPELL_EFFECT_SUMMON_OBJECT_SLOT4
    &Spell::EffectDispelMechanic,                           //108 SPELL_EFFECT_DISPEL_MECHANIC
    &Spell::EffectSummonDeadPet,                            //109 SPELL_EFFECT_SUMMON_DEAD_PET
    &Spell::EffectDestroyAllTotems,                         //110 SPELL_EFFECT_DESTROY_ALL_TOTEMS
    &Spell::EffectDurabilityDamage,                         //111 SPELL_EFFECT_DURABILITY_DAMAGE
    &Spell::EffectUnused,                                   //112 SPELL_EFFECT_FORCE_RIDE_ROCKET
    &Spell::EffectResurrectNew,                             //113 SPELL_EFFECT_RESURRECT_NEW
    &Spell::EffectTaunt,                                    //114 SPELL_EFFECT_ATTACK_ME
    &Spell::EffectDurabilityDamagePCT,                      //115 SPELL_EFFECT_DURABILITY_DAMAGE_PCT
    &Spell::EffectSkinPlayerCorpse,                         //116 SPELL_EFFECT_SKIN_PLAYER_CORPSE       one spell: Remove Insignia, bg usage, required special corpse flags...
    &Spell::EffectSpiritHeal,                               //117 SPELL_EFFECT_SPIRIT_HEAL              one spell: Spirit Heal
    &Spell::EffectSkill,                                    //118 SPELL_EFFECT_SKILL                    professions and more
    &Spell::EffectApplyAreaAura,                            //119 SPELL_EFFECT_APPLY_AREA_AURA_PET
    &Spell::EffectUnused,                                   //120 SPELL_EFFECT_TELEPORT_GRAVEYARD       one spell: Graveyard Teleport Test
    &Spell::EffectWeaponDmg,                                //121 SPELL_EFFECT_NORMALIZED_WEAPON_DMG
    &Spell::EffectUnused,                                   //122 SPELL_EFFECT_122                      unused
    &Spell::EffectSendTaxi,                                 //123 SPELL_EFFECT_SEND_TAXI                taxi/flight related (misc value is taxi path id)
    &Spell::EffectPullTowards,                              //124 SPELL_EFFECT_PULL_TOWARDS
    &Spell::EffectModifyThreatPercent,                      //125 SPELL_EFFECT_MODIFY_THREAT_PERCENT
    &Spell::EffectStealBeneficialBuff,                      //126 SPELL_EFFECT_STEAL_BENEFICIAL_BUFF    spell steal effect?
    &Spell::EffectProspecting,                              //127 SPELL_EFFECT_PROSPECTING              Prospecting spell
    &Spell::EffectApplyAreaAura,                            //128 SPELL_EFFECT_APPLY_AREA_AURA_FRIEND
    &Spell::EffectApplyAreaAura,                            //129 SPELL_EFFECT_APPLY_AREA_AURA_ENEMY
    &Spell::EffectRedirectThreat,                           //130 SPELL_EFFECT_REDIRECT_THREAT
    &Spell::EffectPlaySound,                                //131 SPELL_EFFECT_PLAY_SOUND               sound id in misc value (SoundEntries.dbc)
    &Spell::EffectPlayMusic,                                //132 SPELL_EFFECT_PLAY_MUSIC               sound id in misc value (SoundEntries.dbc)
    &Spell::EffectUnlearnSpecialization,                    //133 SPELL_EFFECT_UNLEARN_SPECIALIZATION   unlearn profession specialization
    &Spell::EffectKillCredit,                               //134 SPELL_EFFECT_KILL_CREDIT              misc value is creature entry
    &Spell::EffectNULL,                                     //135 SPELL_EFFECT_CALL_PET
    &Spell::EffectHealPct,                                  //136 SPELL_EFFECT_HEAL_PCT
    &Spell::EffectEnergizePct,                              //137 SPELL_EFFECT_ENERGIZE_PCT
    &Spell::EffectLeapBack,                                 //138 SPELL_EFFECT_LEAP_BACK                Leap back
    &Spell::EffectQuestClear,                               //139 SPELL_EFFECT_CLEAR_QUEST              Reset quest status (miscValue - quest ID)
    &Spell::EffectForceCast,                                //140 SPELL_EFFECT_FORCE_CAST
    &Spell::EffectForceCast,                                //141 SPELL_EFFECT_FORCE_CAST_WITH_VALUE
    &Spell::EffectTriggerSpell,                             //142 SPELL_EFFECT_TRIGGER_SPELL_WITH_VALUE
    &Spell::EffectApplyAreaAura,                            //143 SPELL_EFFECT_APPLY_AREA_AURA_OWNER
    &Spell::EffectKnockBack,                                //144 SPELL_EFFECT_KNOCK_BACK_DEST
    &Spell::EffectPullTowards,                              //145 SPELL_EFFECT_PULL_TOWARDS_DEST                      Black Hole Effect
    &Spell::EffectActivateRune,                             //146 SPELL_EFFECT_ACTIVATE_RUNE
    &Spell::EffectQuestFail,                                //147 SPELL_EFFECT_QUEST_FAIL               quest fail
    &Spell::EffectTriggerMissileSpell,                      //148 SPELL_EFFECT_TRIGGER_MISSILE_SPELL_WITH_VALUE
    &Spell::EffectChargeDest,                               //149 SPELL_EFFECT_CHARGE_DEST
    &Spell::EffectQuestStart,                               //150 SPELL_EFFECT_QUEST_START
    &Spell::EffectTriggerRitualOfSummoning,                 //151 SPELL_EFFECT_TRIGGER_SPELL_2
    &Spell::EffectSummonRaFFriend,                          //152 SPELL_EFFECT_SUMMON_RAF_FRIEND        summon Refer-a-Friend
    &Spell::EffectCreateTamedPet,                           //153 SPELL_EFFECT_CREATE_TAMED_PET         misc value is creature entry
    &Spell::EffectDiscoverTaxi,                             //154 SPELL_EFFECT_DISCOVER_TAXI
    &Spell::EffectTitanGrip,                                //155 SPELL_EFFECT_TITAN_GRIP Allows you to equip two-handed axes, maces and swords in one hand, but you attack $49152s1% slower than normal.
    &Spell::EffectEnchantItemPrismatic,                     //156 SPELL_EFFECT_ENCHANT_ITEM_PRISMATIC
    &Spell::EffectCreateItem2,                              //157 SPELL_EFFECT_CREATE_ITEM_2            create item or create item template and replace by some randon spell loot item
    &Spell::EffectMilling,                                  //158 SPELL_EFFECT_MILLING                  milling
    &Spell::EffectRenamePet,                                //159 SPELL_EFFECT_ALLOW_RENAME_PET         allow rename pet once again
    &Spell::EffectForcePlayerInteraction,                   //160 SPELL_EFFECT_FORCE_PLAYER_INTERACTION
    &Spell::EffectSpecCount,                                //161 SPELL_EFFECT_TALENT_SPEC_COUNT        second talent spec (learn/revert)
    &Spell::EffectActivateSpec,                             //162 SPELL_EFFECT_TALENT_SPEC_SELECT       activate primary/secondary spec
    &Spell::EffectUnused,                                   //163 SPELL_EFFECT_163                      (unused, 6.0.3)
    &Spell::EffectRemoveAura,                               //164 SPELL_EFFECT_REMOVE_AURA
    &Spell::EffectDamageFromMaxHealthPCT,                   //165 SPELL_EFFECT_DAMAGE_FROM_MAX_HEALTH_PCT
    &Spell::EffectGiveCurrency,                             //166 SPELL_EFFECT_GIVE_CURRENCY
    &Spell::EffectNULL,                                     //167 SPELL_EFFECT_UPDATE_PLAYER_PHASE
    &Spell::EffectNULL,                                     //168 SPELL_EFFECT_ALLOW_CONTROL_PET        @TODO: Implement it, it's needed to havn't action pet bar (hunter & warlocks) until level 10
    &Spell::EffectDestroyItem,                              //169 SPELL_EFFECT_DESTROY_ITEM
    &Spell::EffectNULL,                                     //170 SPELL_EFFECT_UPDATE_ZONE_AURAS_AND_PHASES
    &Spell::EffectSummonObject,                             //171 SPELL_EFFECT_SUMMON_OBJECT
    &Spell::EffectResurrectWithAura,                        //172 SPELL_EFFECT_RESURRECT_WITH_AURA      Aoe resurrection (guild perk)
    &Spell::EffectUnlockGuildVaultTab,                      //173 SPELL_EFFECT_UNLOCK_GUILD_VAULT_TAB
    &Spell::EffectApplyAura,                                //174 SPELL_EFFECT_APPLY_AURA_ON_PET
    &Spell::EffectUnused,                                   //175 SPELL_EFFECT_175                      Only one spell : Wild fixation (125570) 6.0.3
    &Spell::EffectBecomeUntargettable,                      //176 SPELL_EFFECT_BECOME_UNTARGETTABLE
    &Spell::EffectNULL,                                     //177 SPELL_EFFECT_DESPAWN_AREA_AURA
    &Spell::EffectUnused,                                   //178 SPELL_EFFECT_ABANDON_SCENARIO_QUEST
    &Spell::EffectCreateAreatrigger,                        //179 SPELL_EFFECT_CREATE_AREATRIGGER
    &Spell::EffectUnused,                                   //180 SPELL_EFFECT_UPDATE_AREATRIGGER      "Update Area Triggers" 106584 & 1333355
    &Spell::EffectUnlearnTalent,                            //181 SPELL_EFFECT_UNLEARN_TALENT
    &Spell::EffectDespawnAreaTrigger,                       //182 SPELL_EFFECT_DESPAWN_AREA_TRIGGER
    &Spell::EffectNULL,                                     //183 SPELL_EFFECT_183                     Unused 6.0.3
    &Spell::EffectReputation,                               //184 SPELL_EFFECT_REPUTATION_REWARD
    &Spell::EffectPlaySceneObject,                          //185 SPELL_EFFECT_PLAY_SCENEOBJECT
    &Spell::EffectPlaySceneObject,                          //186 SPELL_EFFECT_PLAY_SCENEOBJECT_2
    &Spell::EffectRandomizeArchaeologyDigsites,             //187 SPELL_EFFECT_RANDOMIZE_ARCHAEOLOGY_DIGSITES
    &Spell::EffectStampede,                                 //188 SPELL_EFFECT_STAMPEDE
    &Spell::EffectLootBonus,                                //189 SPELL_EFFECT_LOOT_BONUS              Boss loot bonus ?
    &Spell::EffectNULL,                                     //190 SPELL_EFFECT_190                     internal spell
    &Spell::EffectTeleportToDigsite,                        //191 SPELL_EFFECT_TELEPORT_TO_DIGSITE     Teleport player to an random digsite (Archaeology)
    &Spell::EffectUncagePetBattle,                          //192 SPELL_EFFECT_UNCAGE_BATTLE_PET       Battle pet exchange (123302)
    &Spell::EffectNULL,                                     //193 SPELL_EFFECT_193                     157417 Launch NPC Pet Battle
    &Spell::EffectNULL,                                     //194 SPELL_EFFECT_194                     Unused 6.0.3
    &Spell::EffectNULL,                                     //195 SPELL_EFFECT_195                     Scene related
    &Spell::EffectNULL,                                     //196 SPELL_EFFECT_196                     summon multi NPC
    &Spell::EffectNULL,                                     //197 SPELL_EFFECT_197                     Scene related
    &Spell::EffectNULL,                                     //198 SPELL_EFFECT_PLAY_CINEMATIC
    &Spell::EffectNULL,                                     //199 SPELL_EFFECT_199
    &Spell::EffectResurectPetBattles,                       //200 SPELL_EFFECT_RESURECT_BATTLE_PETS    Battle pet Healing  125439, 125801
    &Spell::EffectCanPetBattle,                             //201 SPELL_EFFECT_CAN_PETBATTLE           Battle pet first slot and track
    &Spell::EffectNULL,                                     //202 SPELL_EFFECT_202                     Unk 5.4.0
    &Spell::EffectNULL,                                     //203 SPELL_EFFECT_203                     Unk 5.4.0
    &Spell::EffectNULL,                                     //204 SPELL_EFFECT_204                     BATTLEPET System 5.4.0
    &Spell::EffectNULL,                                     //205 SPELL_EFFECT_205                     Garrison related 6.0.3
    &Spell::EffectNULL,                                     //206 SPELL_EFFECT_206                     used for TimelessIsle 5.4.0
    &Spell::EffectNULL,                                     //207 SPELL_EFFECT_207                     Quest Garrison related (value = questid)
    &Spell::EffectNULL,                                     //208 SPELL_EFFECT_208                     Unk 5.4.0
    &Spell::EffectNULL,                                     //209 SPELL_EFFECT_209                     test spell
    &Spell::EffectLearnBluePrint,                           //210 SPELL_EFFECT_LEARN_BLUE_PRINT
    &Spell::EffectNULL,                                     //211 SPELL_EFFECT_211                     Garrison learn (specialization)
    &Spell::EffectNULL,                                     //212 SPELL_EFFECT_212                     Unused 6.0.3
    &Spell::EffectDeathGrip,                                //213 SPELL_EFFECT_DEATH_GRIP
    &Spell::EffectNULL,                                     //214 SPELL_EFFECT_214                     Create Garrison
    &Spell::EffectNULL,                                     //215 SPELL_EFFECT_215                     Unk 6.0.1
    &Spell::EffectNULL,                                     //216 SPELL_EFFECT_CREATE_SHIPMENT
    &Spell::EffectNULL,                                     //217 SPELL_EFFECT_217                     Upgrade Garrison (171905)
    &Spell::EffectNULL,                                     //218 SPELL_EFFECT_218                     Unk 6.0.1
    &Spell::EffectNULL,                                     //219 SPELL_EFFECT_219                     Unk 6.0.1
    &Spell::EffectObtainFollower,                           //220 SPELL_EFFECT_OBTAIN_FOLLOWER         Obtain a garrison follower (contract item)
    &Spell::EffectNULL,                                     //221 SPELL_EFFECT_221                     Unk 6.0.1
    &Spell::EffectCreateHeirloom,                           //222 SPELL_EFFECT_CREATE_HEIRLOOM         Create Heirloom
    &Spell::EffectNULL,                                     //223 SPELL_EFFECT_223                     Unk 6.0.1
    &Spell::EffectGarrisonFinalize,                         //224 SPELL_EFFECT_GARRISON_FINALIZE_BUILDING
    &Spell::EffectNULL,                                     //225 SPELL_EFFECT_225                     Battle-Training Stone
    &Spell::EffectNULL,                                     //226 SPELL_EFFECT_226                     Unk 6.0.1
    &Spell::EffectNULL,                                     //227 SPELL_EFFECT_227                     Teleport Into Karabor
    &Spell::EffectNULL,                                     //228 SPELL_EFFECT_228                     Recruit A Friend Summon Effect
    &Spell::EffectNULL,                                     //229 SPELL_EFFECT_229                     Upgrade follower to epic
    &Spell::EffectUpgradeFolloweriLvl,                      //230 SPELL_EFFECT_UPGRADE_FOLLOWER_ILVL   Upgrade follower iLvL
    &Spell::EffectNULL,                                     //231 SPELL_EFFECT_231                     level up an follower (debug PTR spell)
    &Spell::EffectNULL,                                     //232 SPELL_EFFECT_232                     Phase related
    &Spell::EffectNULL,                                     //233 SPELL_EFFECT_233                     Retrain Follower (Reroll the abilities and traits on a follower.)
    &Spell::EffectNULL,                                     //234 SPELL_EFFECT_234                     Unused 6.0.3
    &Spell::EffectNULL,                                     //235 SPELL_EFFECT_235                     Unused 6.0.3
    &Spell::EffectNULL,                                     //236 SPELL_EFFECT_236                     Burst of Experience (follower)
    &Spell::EffectNULL,                                     //237 SPELL_EFFECT_237                     Unk 6.0.1
    &Spell::EffectNULL,                                     //238 SPELL_EFFECT_238                     Garrison restore item (give X in skill)
    &Spell::EffectNULL,                                     //239 SPELL_EFFECT_239                     Garrison skip contruction
    &Spell::EffectNULL,                                     //240 SPELL_EFFECT_240                     Unused 6.0.3
    &Spell::EffectNULL,                                     //241 SPELL_EFFECT_241                     Unused 6.0.3
    &Spell::EffectNULL,                                     //242 SPELL_EFFECT_242                     Unused 6.0.3
    &Spell::EffectNULL,                                     //243 SPELL_EFFECT_243                     Illusion spells (visual enchant)
    &Spell::EffectNULL,                                     //244 SPELL_EFFECT_244                     learn an follower ability NYI
    &Spell::EffectUpgradeHeirloom,                          //245 SPELL_EFFECT_UPGRADE_HEIRLOOM        Unused 6.0.3
    &Spell::EffectNULL,                                     //246 SPELL_EFFECT_246                     Unused 6.0.3
};

void Spell::EffectNULL(SpellEffIndex /*effIndex*/)
{
    sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "WORLD: Spell Effect DUMMY");
}

void Spell::EffectUnused(SpellEffIndex /*effIndex*/)
{
    // NOT USED BY ANY SPELL OR USELESS OR IMPLEMENTED IN DIFFERENT WAY IN TRINITY
}

void Spell::EffectResurrectNew(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->isAlive())
        return;

    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if (!unitTarget->IsInWorld())
        return;

    Player* target = unitTarget->ToPlayer();

    if (target->IsRessurectRequested())       // already have one active request
        return;

    uint32 health = damage;
    uint32 mana = m_spellInfo->Effects[effIndex].MiscValue;
    ExecuteLogEffectResurrect(effIndex, target);
    target->SetResurrectRequestData(m_caster, health, mana, 0);
    SendResurrectRequest(target);
}

void Spell::EffectInstaKill(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || (!unitTarget->isAlive() && m_spellInfo->Id != 108503))
        return;

    if (m_spellInfo->Id == 108503 && (!unitTarget->GetHealth() || !unitTarget->isAlive()))
    {
        unitTarget->ToPet()->Remove(PET_SLOT_ACTUAL_PET_SLOT, false, unitTarget->ToPet()->m_Stampeded);
        return;
    }

    if (unitTarget->GetTypeId() == TYPEID_PLAYER)
        if (unitTarget->ToPlayer()->GetCommandStatus(CHEAT_GOD))
            return;

    if (m_caster == unitTarget)                              // prevent interrupt message
        finish();

    WorldPacket data(SMSG_SPELL_INSTAKILL_LOG, 8 + 8 + 4);
    data << uint64(m_caster->GetGUID());
    data << uint64(unitTarget->GetGUID());
    data << uint32(m_spellInfo->Id);
    m_caster->SendMessageToSet(&data, true);

    m_caster->DealDamage(unitTarget, unitTarget->GetHealth(), NULL, NODAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
}

void Spell::EffectEnvironmentalDMG(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->isAlive())
        return;

    uint32 absorb = 0;
    uint32 resist = 0;

    m_caster->CalcAbsorbResist(unitTarget, m_spellInfo->GetSchoolMask(), SPELL_DIRECT_DAMAGE, damage, &absorb, &resist, m_spellInfo);

    m_caster->SendSpellNonMeleeDamageLog(unitTarget, m_spellInfo->Id, damage, m_spellInfo->GetSchoolMask(), absorb, resist, false, 0, false);
    if (unitTarget->GetTypeId() == TYPEID_PLAYER)
        unitTarget->ToPlayer()->EnvironmentalDamage(DAMAGE_FIRE, damage);
}

void Spell::EffectSchoolDMG(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH_TARGET)
        return;

    if (unitTarget && unitTarget->isAlive())
    {
        bool apply_direct_bonus = true;
        switch (m_spellInfo->SpellFamilyName)
        {
            case SPELLFAMILY_GENERIC:
            {
                // Meteor like spells (divided damage to targets)
                if (m_spellInfo->AttributesCu & SPELL_ATTR0_CU_SHARE_DAMAGE)
                {
                    uint32 count = 0;
                    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                    if (ihit->effectMask & (1 << effIndex))
                        ++count;

                    damage /= count;                    // divide to all targets
                }

                switch (m_spellInfo->Id)                     // better way to check unknown
                {
                    case 109721: // Lightning Strike, Vial of Shadows (lfr)
                        damage += int32(0.266f * m_caster->GetTotalAttackPowerValue(m_caster->getClass() == CLASS_HUNTER ? WeaponAttackType::RangedAttack : WeaponAttackType::BaseAttack));
                        break;
                    case 107994: // Lightning Strike, Vial of Shadows (normal)
                        damage += int32(0.3f * m_caster->GetTotalAttackPowerValue(m_caster->getClass() == CLASS_HUNTER ? WeaponAttackType::RangedAttack : WeaponAttackType::BaseAttack));
                        break;
                    case 109724: // Lightning Strike, Vial of Shadows (heroic)
                        damage += int32(0.339f * m_caster->GetTotalAttackPowerValue(m_caster->getClass() == CLASS_HUNTER ? WeaponAttackType::RangedAttack : WeaponAttackType::BaseAttack));
                        break;
                        // Detonate Mana, Tyrande's Favorite Doll
                    case 92601:
                        if (constAuraEffectPtr aurEff = m_caster->GetAuraEffect(92596, EFFECT_0))
                            damage = aurEff->GetAmount();
                        break;
                    // Consumption
                    case 28865:
                        damage = (((InstanceMap*)m_caster->GetMap())->GetDifficulty() == REGULAR_5_DIFFICULTY ? 2750 : 4250);
                        break;
                    // percent from health with min
                    case 25599: // Thundercrash
                    {
                        damage = unitTarget->GetHealth() / 2;
                        if (damage < 200)
                            damage = 200;
                        break;
                    }
                    // arcane charge. must only affect demons (also undead?)
                    case 45072:
                    {
                        if (unitTarget->GetCreatureType() != CREATURE_TYPE_DEMON
                            && unitTarget->GetCreatureType() != CREATURE_TYPE_UNDEAD)
                            return;
                        break;
                    }
                    // Gargoyle Strike
                    case 51963:
                    {
                        // about +4 base spell dmg per level
                        damage = (m_caster->getLevel() - 60) * 4 + 60;
                        break;
                    }
                    case 123199:// Toss Explosive Barrel
                        if (unitTarget->GetTypeId() == TYPEID_PLAYER ||
                            (unitTarget->GetTypeId() == TYPEID_UNIT && unitTarget->GetOwner() && unitTarget->GetOwner()->ToPlayer()))
                            return;
                        break;
                }
                break;
            }
            case SPELLFAMILY_WARRIOR:
            {
                switch (m_spellInfo->Id)
                {
                    case 34428: // Victory Rush
                    {
                        if (m_caster->ToPlayer()->GetSpecializationId(m_caster->ToPlayer()->GetActiveSpec()) == SPEC_WARRIOR_ARMS)
                            damage = CalculatePct(m_caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack), 67.2f);
                        else
                            damage = CalculatePct(m_caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack), 56.0f);

                        break;
                    }
                    case 46968: // Shockwave
                    {
                        if (m_caster->GetTypeId() != TYPEID_PLAYER)
                            break;

                        int32 pct = 0;

                        switch (m_caster->ToPlayer()->GetSpecializationId(m_caster->ToPlayer()->GetActiveSpec()))
                        {
                            case SPEC_WARRIOR_ARMS:
                                pct = 90;
                                break;
                            case SPEC_WARRIOR_FURY:
                            case SPEC_WARRIOR_PROTECTION:
                                pct = 75;
                                break;
                            default:
                                break;
                        }

                        damage = int32(CalculatePct(m_caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack), pct));

                        break;
                    }
                    case 103840:// Impending Victory
                    {
                        if (m_caster->ToPlayer()->GetSpecializationId(m_caster->ToPlayer()->GetActiveSpec()) == SPEC_WARRIOR_ARMS)
                            damage = CalculatePct(m_caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack), 70.0f);
                        else
                            damage = CalculatePct(m_caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack), 56.0f);

                        // Impending Victory heals you for 10% of your maximum health
                        m_caster->CastSpell(m_caster, 118340, true);

                        break;
                    }
                    case 118000:// Dragon Roar
                    {
                        if (m_caster->ToPlayer()->GetSpecializationId(m_caster->ToPlayer()->GetActiveSpec()) == SPEC_WARRIOR_ARMS)
                            damage += CalculatePct(m_caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack), 168);
                        else
                            damage += CalculatePct(m_caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack), 140);

                        break;
                    }
                }

                break;
            }
            case SPELLFAMILY_PALADIN:
            {
                switch (m_spellInfo->Id)
                {
                    // Ancient Fury
                    case 86704:
                    {
                        if (m_caster->GetTypeId() == TYPEID_PLAYER)
                        {
                            if (AuraPtr aura = m_caster->GetAura(86700))
                            {
                                uint8 stacks = aura->GetStackAmount();
                                damage = stacks * (damage + 0.1f * m_caster->SpellBaseDamageBonusDone(m_spellInfo->GetSchoolMask()));
                                damage = m_caster->SpellDamageBonusDone(unitTarget, m_spellInfo, damage, effIndex, SPELL_DIRECT_DAMAGE);
                                uint32 count = 0;
                                for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                                    ++count;
                                damage /= count;
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
            case SPELLFAMILY_HUNTER:
            {
                switch (m_spellInfo->Id)
                {
                    case 13812: // Explosive Trap
                    {
                        if (m_caster->HasAura(119403)) // Glyph of Explosive Trap
                            return;
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
            case SPELLFAMILY_MONK:
            {
                switch (m_spellInfo->Id)
                {
                    case 115080:// Touch of Death
                    {
                        if (Unit* caster = GetCaster())
                        {
                            if (unitTarget)
                            {
                                uint32 damage = unitTarget->GetHealth();
                                m_caster->SendSpellNonMeleeDamageLog(unitTarget, m_spellInfo->Id, damage, m_spellInfo->GetSchoolMask(), 0, 0, false, 0, false);
                                m_caster->DealDamageMods(unitTarget, damage, NULL);
                                m_caster->DealDamage(unitTarget, damage, NULL, SPELL_DIRECT_DAMAGE, m_spellInfo->GetSchoolMask(), m_spellInfo, false);
                            }
                        }
                        return;
                    }
                    default:
                        break;
                }
                break;
            }
        }

        if (m_originalCaster && damage >= 0 && apply_direct_bonus)
        {
            damage = m_originalCaster->SpellDamageBonusDone(unitTarget, m_spellInfo, (uint32)damage, effIndex, SPELL_DIRECT_DAMAGE);
            damage = unitTarget->SpellDamageBonusTaken(m_originalCaster, m_spellInfo, (uint32)damage, SPELL_DIRECT_DAMAGE);
        }

        m_damage += damage;
    }
}

void Spell::EffectDummy(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget && !gameObjTarget && !itemTarget)
        return;

    uint32 spell_id = 0;
    int32 bp = 0;
    bool triggered = true;
    SpellCastTargets targets;

    // Fishing dummy
    if (m_spellInfo->Id == 131474)
        m_caster->CastSpell(m_caster, 131476, true);

    // Capture point
    if ((m_spellInfo->Id == 97388 || m_spellInfo->Id == 97372) && m_caster->ToPlayer() && m_caster->ToPlayer()->GetBattleground())
        m_caster->ToPlayer()->GetBattleground()->EventPlayerClickedOnFlag(m_caster->ToPlayer(), unitTarget);

    // selection by spell family
    switch (m_spellInfo->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (m_spellInfo->Id)
            {
                // Flask of Enhancement
                case 79637:
                {
                    uint32 stat_str = m_caster->GetStat(STAT_STRENGTH);
                    uint32 stat_agi = m_caster->GetStat(STAT_AGILITY);
                    uint32 stat_int = m_caster->GetStat(STAT_INTELLECT);

                    if (stat_str > stat_agi && stat_str > stat_int)
                        m_caster->CastSpell(m_caster, 79638, true);
                    else if (stat_agi > stat_str && stat_agi > stat_int)
                        m_caster->CastSpell(m_caster, 79639, true);
                    else if (stat_int > stat_agi && stat_int > stat_str)
                        m_caster->CastSpell(m_caster, 79640, true);

                    break;
                }
                case 126734:// Synapse Springs
                {
                    Stats usedStat = STAT_INTELLECT;
                    int32 stat = m_caster->GetStat(STAT_INTELLECT);
                    int32 agility = m_caster->GetStat(STAT_AGILITY);
                    int32 strength = m_caster->GetStat(STAT_STRENGTH);

                    if (stat < agility)
                    {
                        stat = agility;
                        usedStat = STAT_AGILITY;
                    }
                    if (stat < strength)
                    {
                        stat = strength;
                        usedStat = STAT_STRENGTH;
                    }

                    switch (usedStat)
                    {
                    case STAT_INTELLECT:
                        m_caster->CastCustomSpell(m_caster, 96230, &damage, NULL, NULL, true);
                        break;
                    case STAT_AGILITY:
                        m_caster->CastCustomSpell(m_caster, 96228, &damage, NULL, NULL, true);
                        break;
                    case STAT_STRENGTH:
                        m_caster->CastCustomSpell(m_caster, 96229, &damage, NULL, NULL, true);
                        break;
                    default:
                        break;
                    }

                    break;
                }
                case 96934: // Blessing of Khaz'goroth
                case 97127: // Blessing of Khaz'goroth (H)
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (!m_caster->HasAura(96923))
                        return;

                    uint32 crit = m_caster->ToPlayer()->GetUInt32Value(PLAYER_FIELD_COMBAT_RATINGS + CR_CRIT_MELEE);
                    uint32 mastery = m_caster->ToPlayer()->GetUInt32Value(PLAYER_FIELD_COMBAT_RATINGS + CR_MASTERY);
                    uint32 haste = m_caster->ToPlayer()->GetUInt32Value(PLAYER_FIELD_COMBAT_RATINGS + CR_HASTE_MELEE);

                    uint8 stacks = 1;
                    if (AuraPtr aur = m_caster->GetAura(96923))
                        stacks = aur->GetStackAmount();

                    int32 bp0 = damage * stacks;

                    if (crit > mastery && crit > haste)
                        m_caster->CastCustomSpell(m_caster, 96928, &bp0, 0, 0, true);
                    else if (haste > mastery && haste > crit)
                        m_caster->CastCustomSpell(m_caster, 96927, &bp0, 0, 0, true);
                    else if (mastery > haste && mastery > crit)
                        m_caster->CastCustomSpell(m_caster, 96929, &bp0, 0, 0, true);

                    m_caster->RemoveAurasDueToSpell(96923);

                    break;
                }
                case 25952: // Reindeer Dust Effect
                {
                    if (m_caster->GetTypeId() == TYPEID_UNIT)
                        m_caster->ToCreature()->DisappearAndDie();
                    return;
                }
                case 101992:// Put up Darkmoon Banner
                {
                    m_caster->CastSpell(m_caster, 102006, true);
                    break;
                }
                case 122180:// Tap to the Spirit World
                {
                    m_caster->CastSpell(unitTarget, 122181, true);
                    break;
                }
                case 47468: // Ghoul: Claw
                {
                    if (!unitTarget)
                        return;

                    // Dark Transformation - Replace spell
                    if (m_caster->HasAura(63560))
                        m_caster->CastSpell(unitTarget, 91778, true); // Sweeping Claws
                    else
                        m_caster->CastSpell(unitTarget, 91776, true); // Claw

                    break;
                }
                case 47484: // Ghoul: Huddle
                {
                    if (!unitTarget)
                        return;

                    // Dark Transformation - Replace spell
                    if (m_caster->HasAura(63560))
                        m_caster->CastSpell(unitTarget, 91837, true); // Putrid Bulwark
                    else
                        m_caster->CastSpell(unitTarget, 91838, true); // Huddle
                    break;
                }
                case 47482: // Ghoul: Leap
                {
                    if (!unitTarget)
                        return;

                    // Dark Transformation - Replace spell
                    if (m_caster->HasAura(63560))
                        m_caster->CastSpell(unitTarget, 91802, true); // Shambling Rush
                    else
                        m_caster->CastSpell(unitTarget, 91809, true); // Leap
                    break;
                }
                case 6203:  // Soulstone
                {
                    if (!unitTarget->isAlive())
                        unitTarget->CastSpell(unitTarget, 3026, true); // Self resurrect
                    break;
                }
                case 45206: // Copy Off-hand Weapon
                case 69892:
                {
                    m_caster->CastSpell(unitTarget, damage, true);
                    if (unitTarget->GetTypeId() == TYPEID_PLAYER)
                        break;

                    if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (Item* offItem = m_caster->ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
                            unitTarget->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 1, offItem->GetEntry());
                    }
                    else
                        unitTarget->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 1, m_caster->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 1));
                    break;
                }
                case 41055: // Copy Mainhand Weapon
                case 63416:
                case 69891:
                {
                    m_caster->CastSpell(unitTarget, damage, true);
                    if (unitTarget->GetTypeId() == TYPEID_PLAYER)
                        break;

                    if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (Item* mainItem = m_caster->ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
                            unitTarget->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, mainItem->GetEntry());
                    }
                    else
                        unitTarget->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, m_caster->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID));
                    break;
                }
                default:
                    break;
            }

            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            switch (m_spellInfo->Id)
            {
                case 31789: // Righteous Defense (step 1)
                {
                    // Clear targets for eff 1
                    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                        ihit->effectMask &= ~(1 << 1);

                    // not empty (checked), copy
                    Unit::AttackerSet attackers = unitTarget->getAttackers();

                    // remove invalid attackers
                    for (Unit::AttackerSet::iterator aItr = attackers.begin(); aItr != attackers.end();)
                        if (!(*aItr)->IsValidAttackTarget(m_caster))
                            attackers.erase(aItr++);
                        else
                            ++aItr;

                    // selected from list 3
                    uint32 maxTargets = std::min<uint32>(3, attackers.size());
                    for (uint32 i = 0; i < maxTargets; ++i)
                    {
                        Unit* attacker = JadeCore::Containers::SelectRandomContainerElement(attackers);
                        AddUnitTarget(attacker, 1 << 1);
                        attackers.erase(attacker);
                    }

                    // now let next effect cast spell at each target.
                    return;
                }
                default:
                    break;
            }

            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            switch (m_spellInfo->Id)
            {
                case 53478: // Last Stand (Tenacity)
                    m_caster->CastSpell(m_caster, 53479, true);
                    break;
                case 51753: // Camouflage
                    m_caster->CastSpell(m_caster, 51755, true);
                    m_caster->CastSpell(m_caster, 80326, true);

                    if (m_caster->isInCombat())
                        if (AuraPtr camouflage = m_caster->GetAura(51755))
                            camouflage->SetDuration(6000);

                    if (Unit* pet = m_caster->GetGuardianPet())
                        pet->CastSpell(pet, 51753, true);

                    break;
                case 63487: // Ice Trap
                    if (Unit* owner = m_caster->GetOwner())
                        owner->CastSpell(m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), 13810, true);
                    break;
                case 128997:// Spirit Beast Blessing
                    m_caster->CastSpell(m_caster, 127830, true);
                    break;
                default:
                    break;
            }

            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            switch (m_spellInfo->Id)
            {
                // Death Grip
                case 49576:
                {
                    if (!unitTarget)
                        return;

                    Unit *caster = m_caster;
                    if (m_UniqueTargetInfo.size())
                    {
                        if (m_UniqueTargetInfo.front().missCondition == SPELL_MISS_REFLECT)
                            caster = m_targets.GetUnitTarget();
                    }
                    caster->CastSpell(unitTarget, 49560, true);
                    break;
                }
                case 47481: // Ghoul: Gnaw
                {
                    if (!unitTarget)
                        return;

                    // Dark Transformation - Replace spell
                    if (m_caster->HasAura(63560))
                        m_caster->CastSpell(unitTarget, 91797, true); // Monstrous Blow
                    else
                        m_caster->CastSpell(unitTarget, 91800, true); // Gnaw

                    break;
                }
                case 47541: // Death Coil
                {
                    if (m_caster->IsFriendlyTo(unitTarget))
                    {
                        // Glyph of Death Coil
                        if (m_caster->HasAura(63333) && unitTarget->GetCreatureType() != CREATURE_TYPE_UNDEAD)
                            m_caster->CastSpell(unitTarget, 115635, true); // Death Barrier
                        else
                        {
                            int32 bp = (damage + m_caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack) * 0.514f) * 3.5f;
                            m_caster->CastCustomSpell(unitTarget, 47633, &bp, NULL, NULL, true);
                        }
                    }
                    else
                    {
                        int32 bp = damage + m_caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack) * 0.514f;
                        m_caster->CastCustomSpell(unitTarget, 47632, &bp, NULL, NULL, true);
                    }

                    return;
                }
                default:
                    break;
            }

            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            switch (m_spellInfo->Id)
            {
                case 119905:// Cauterize Master (Command Demon)
                {
                    if (m_caster->GetTypeId() == TYPEID_PLAYER)
                        if (Pet* pet = m_caster->ToPlayer()->GetPet())
                        {
                            pet->CastSpell(unitTarget, 119899, true);
                            m_caster->ToPlayer()->AddSpellCooldown(119905, 0, 30 * IN_MILLISECONDS);
                        }

                    break;
                }
                case 119907:// Disarm (Command Demon)
                {
                    if (!m_caster->IsFriendlyTo(unitTarget))
                        if (m_caster->GetTypeId() == TYPEID_PLAYER)
                            if (Pet* pet = m_caster->ToPlayer()->GetPet())
                            {
                                pet->CastSpell(unitTarget, damage, true);
                                m_caster->ToPlayer()->AddSpellCooldown(119907, 0, 60 * IN_MILLISECONDS);
                            }
                    break;
                }
                case 119910:// Spell Lock (Command Demon)
                {
                    if (!m_caster->IsFriendlyTo(unitTarget))
                        if (m_caster->GetTypeId() == TYPEID_PLAYER)
                            if (Pet* pet = m_caster->ToPlayer()->GetPet())
                            {
                                pet->CastSpell(unitTarget, damage, true);
                                m_caster->ToPlayer()->AddSpellCooldown(119910, 0, 24 * IN_MILLISECONDS);
                            }
                    break;
                }
                case 132409:// Spell Lock (Command Demon - Warlock)
                    m_caster->CastSpell(unitTarget, 24259, true);
                    break;
                case 119911:// Optical Blast (Command Demon)
                {
                    if (!m_caster->IsFriendlyTo(unitTarget))
                        if (m_caster->GetTypeId() == TYPEID_PLAYER)
                            if (Pet* pet = m_caster->ToPlayer()->GetPet())
                            {
                                pet->CastSpell(unitTarget, damage, true);
                                m_caster->ToPlayer()->AddSpellCooldown(119911, 0, 24 * IN_MILLISECONDS);
                            }
                    break;
                }
                case 119909:// Whiplash (Command Demon)
                {
                    if (m_caster->GetTypeId() == TYPEID_PLAYER)
                        if (targets.GetDstPos())
                            if (Pet* pet = m_caster->ToPlayer()->GetPet())
                            {
                                pet->CastSpell(targets.GetDstPos()->GetPositionX(), targets.GetDstPos()->GetPositionY(), targets.GetDstPos()->GetPositionZ(), damage, true);
                                m_caster->ToPlayer()->AddSpellCooldown(119909, 0, 25 * IN_MILLISECONDS);
                            }

                    break;
                }
                case 119913:// Fellash (Command Demon)
                {
                    if (m_caster->GetTypeId() == TYPEID_PLAYER)
                        if (targets.GetDstPos())
                            if (Pet* pet = m_caster->ToPlayer()->GetPet())
                            {
                                pet->CastSpell(targets.GetDstPos()->GetPositionX(), targets.GetDstPos()->GetPositionY(), targets.GetDstPos()->GetPositionZ(), damage, true);
                                m_caster->ToPlayer()->AddSpellCooldown(119913, 0, 25 * IN_MILLISECONDS);
                            }

                    break;
                }
                default:
                    break;
            }

            break;
        }
        case SPELLFAMILY_MAGE:
        {
            switch (m_spellInfo->Id)
            {
                case 43987: // Conjure Refreshment Table
                {
                    if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    {
                        m_caster->ToPlayer()->RemoveSpellCooldown(120056, true); // Rank 1
                        m_caster->ToPlayer()->RemoveSpellCooldown(120055, true); // Rank 2
                        m_caster->ToPlayer()->RemoveSpellCooldown(120054, true); // Rank 3
                        m_caster->ToPlayer()->RemoveSpellCooldown(120053, true); // Rank 4

                        if (m_caster->getLevel() <= 70)
                            m_caster->CastSpell(m_caster, 120056, true);
                        else if (m_caster->getLevel() <= 80)
                            m_caster->CastSpell(m_caster, 120055, true);
                        else if (m_caster->getLevel() <= 85)
                            m_caster->CastSpell(m_caster, 120054, true);
                        else
                            m_caster->CastSpell(m_caster, 120053, true);
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }

    switch (m_spellInfo->Id)
    {
    case 111397:// Bloody Fear
    {
        m_caster->DealDamage(m_caster, m_caster->CountPctFromMaxHealth(5), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        break;
    }
    case 120165:// Conflagrate
    {
        UnitList friends;
        JadeCore::AnyFriendlyUnitInObjectRangeCheck u_check(m_caster, m_caster, 5.0f);
        JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> searcher(m_caster, friends, u_check);
        m_caster->VisitNearbyObject(5.0f, searcher);

        for (auto unit : friends)
        {
            if (m_caster->GetGUID() == unit->GetGUID())
                continue;
            GetOriginalCaster()->CastSpell(unit, 120160, true);
            GetOriginalCaster()->CastSpell(unit, 120201, true);
        }

        break;
    }
    case 107045:// Jade Fire
        m_caster->CastSpell(unitTarget, 107098, false);
        break;
    case 106299:// Summon Living Air
    {
        TempSummon* enne = m_caster->SummonCreature(54631, m_caster->GetPositionX() + rand() % 5, m_caster->GetPositionY() + 2 + rand() % 5, m_caster->GetPositionZ() + 1, 3.3f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
        enne->AddThreat(m_caster, 2000.f);
        break;
    }
    case 120202:// Gate of the Setting Sun | Boss 3 | Bombard
        spell_id = GetSpellInfo()->Effects[0].BasePoints;
        break;
    }

    // spells triggered by dummy effect should not miss
    if (spell_id)
    {
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spell_id);

        if (!spellInfo)
            return;

        targets.SetUnitTarget(unitTarget);
        Spell* spell = new Spell(m_caster, spellInfo, triggered ? TRIGGERED_FULL_MASK : TRIGGERED_NONE, m_originalCasterGUID, true);
        if (bp) spell->SetSpellValue(SPELLVALUE_BASE_POINT0, bp);
        spell->prepare(&targets);
    }

    // pet auras
    if (PetAura const* petSpell = sSpellMgr->GetPetAura(m_spellInfo->Id, effIndex))
    {
        m_caster->AddPetAura(petSpell);
        return;
    }

    // normal DB scripted effect
    m_caster->GetMap()->ScriptsStart(sSpellScripts, uint32(m_spellInfo->Id | (effIndex << 24)), m_caster, unitTarget);

    // Script based implementation. Must be used only for not good for implementation in core spell effects
    // So called only for not proccessed cases
    if (gameObjTarget)
        sScriptMgr->OnDummyEffect(m_caster, m_spellInfo->Id, effIndex, gameObjTarget);
    else if (unitTarget && unitTarget->GetTypeId() == TYPEID_UNIT)
        sScriptMgr->OnDummyEffect(m_caster, m_spellInfo->Id, effIndex, unitTarget->ToCreature());
    else if (itemTarget)
        sScriptMgr->OnDummyEffect(m_caster, m_spellInfo->Id, effIndex, itemTarget);
}

void Spell::EffectTriggerSpell(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH_TARGET
        && effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH)
        return;

    uint32 triggered_spell_id = m_spellInfo->Effects[effIndex].TriggerSpell;

    // todo: move those to spell scripts
    if (m_spellInfo->Effects[effIndex].Effect == SPELL_EFFECT_TRIGGER_SPELL
        && effectHandleMode == SPELL_EFFECT_HANDLE_LAUNCH_TARGET)
    {
        // special cases
        switch (triggered_spell_id)
        {
            case 131361:///< Vanish
            {
                unitTarget->RemoveMovementImpairingAuras();
                unitTarget->RemoveAurasByType(SPELL_AURA_MOD_STALKED);

                /// If this spell is given to an NPC, it must handle the rest using its own AI
                if (unitTarget->GetTypeId() != TYPEID_PLAYER)
                    return;

                /// See if we already are stealthed. If so, we're done.
                if (unitTarget->HasAura(1784) || unitTarget->HasAura(115191))
                    return;

                /// Reset cooldown on stealth if needed
                if (unitTarget->ToPlayer()->HasSpellCooldown(1784))
                    unitTarget->ToPlayer()->RemoveSpellCooldown(1784, true);
                if (unitTarget->ToPlayer()->HasSpellCooldown(115191))
                    unitTarget->ToPlayer()->RemoveSpellCooldown(115191, true);

                if (!unitTarget->HasAura(108208))
                    unitTarget->CastSpell(unitTarget, 1784, true);
                else
                    unitTarget->CastSpell(unitTarget, 115191, true);
                return;
            }
            // Demonic Empowerment -- succubus
            case 54437:
            {
                unitTarget->RemoveMovementImpairingAuras();
                unitTarget->RemoveAurasByType(SPELL_AURA_MOD_STALKED);
                unitTarget->RemoveAurasByType(SPELL_AURA_MOD_STUN);

                // Cast Lesser Invisibility
                unitTarget->CastSpell(unitTarget, 7870, true);
                return;
            }
            // Brittle Armor - (need add max stack of 24575 Brittle Armor)
            case 29284:
            {
                // Brittle Armor
                SpellInfo const* spell = sSpellMgr->GetSpellInfo(24575);
                if (!spell)
                    return;

                for (uint32 j = 0; j < spell->StackAmount; ++j)
                    m_caster->CastSpell(unitTarget, spell->Id, true);
                return;
            }
            // Mercurial Shield - (need add max stack of 26464 Mercurial Shield)
            case 29286:
            {
                // Mercurial Shield
                SpellInfo const* spell = sSpellMgr->GetSpellInfo(26464);
                if (!spell)
                    return;

                for (uint32 j = 0; j < spell->StackAmount; ++j)
                    m_caster->CastSpell(unitTarget, spell->Id, true);
                return;
            }
            // Righteous Defense
            case 31980:
            {
                m_caster->CastSpell(unitTarget, 31790, true);
                return;
            }
        }
    }

    // normal case
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(triggered_spell_id);
    if (!spellInfo)
        return;

    SpellCastTargets targets;
    if (effectHandleMode == SPELL_EFFECT_HANDLE_LAUNCH_TARGET)
    {
        if (!spellInfo->NeedsToBeTriggeredByCaster())
            return;
        targets.SetUnitTarget(unitTarget);
    }
    else //if (effectHandleMode == SPELL_EFFECT_HANDLE_LAUNCH)
    {
        if (spellInfo->NeedsToBeTriggeredByCaster() && (m_spellInfo->Effects[effIndex].GetProvidedTargetMask() & TARGET_FLAG_UNIT_MASK))
            return;

        if (spellInfo->GetExplicitTargetMask() & TARGET_FLAG_DEST_LOCATION)
            targets.SetDst(m_targets);

        targets.SetUnitTarget(m_targets.GetUnitTarget());
    }

    CustomSpellValues values;
    // set basepoints for trigger with value effect
    if (m_spellInfo->Effects[effIndex].Effect == SPELL_EFFECT_TRIGGER_SPELL_WITH_VALUE)
    {
        // maybe need to set value only when basepoints == 0?
        values.AddSpellMod(SPELLVALUE_BASE_POINT0, damage);
        values.AddSpellMod(SPELLVALUE_BASE_POINT1, damage);
        values.AddSpellMod(SPELLVALUE_BASE_POINT2, damage);
    }

    // Remove spell cooldown (not category) if spell triggering spell with cooldown and same category
    if (m_caster->GetTypeId() == TYPEID_PLAYER && m_spellInfo->CategoryRecoveryTime && spellInfo->CategoryRecoveryTime
        && m_spellInfo->Category == spellInfo->Category)
        m_caster->ToPlayer()->RemoveSpellCooldown(spellInfo->Id);

    // original caster guid only for GO cast
    m_caster->CastSpell(targets, spellInfo, &values, TRIGGERED_FULL_MASK, NULL, NULLAURA_EFFECT, m_originalCasterGUID);
}

void Spell::EffectTriggerMissileSpell(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET
        && effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    uint32 triggered_spell_id = m_spellInfo->Effects[effIndex].TriggerSpell;

    // normal case
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(triggered_spell_id);
    if (!spellInfo)
        return;

    SpellCastTargets targets;
    if (effectHandleMode == SPELL_EFFECT_HANDLE_HIT_TARGET)
    {
        if (!spellInfo->NeedsToBeTriggeredByCaster())
            return;
        targets.SetUnitTarget(unitTarget);
    }
    else //if (effectHandleMode == SPELL_EFFECT_HANDLE_HIT)
    {
        if (spellInfo->NeedsToBeTriggeredByCaster() && (m_spellInfo->Effects[effIndex].GetProvidedTargetMask() & TARGET_FLAG_UNIT_MASK))
            return;

        if (spellInfo->GetExplicitTargetMask() & TARGET_FLAG_DEST_LOCATION)
            targets.SetDst(m_targets);

        targets.SetUnitTarget(m_caster);
    }

    CustomSpellValues values;
    // set basepoints for trigger with value effect
    if (m_spellInfo->Effects[effIndex].Effect == SPELL_EFFECT_TRIGGER_MISSILE_SPELL_WITH_VALUE)
    {
        // maybe need to set value only when basepoints == 0?
        values.AddSpellMod(SPELLVALUE_BASE_POINT0, damage);
        values.AddSpellMod(SPELLVALUE_BASE_POINT1, damage);
        values.AddSpellMod(SPELLVALUE_BASE_POINT2, damage);
    }

    // Remove spell cooldown (not category) if spell triggering spell with cooldown and same category
    if (m_caster->GetTypeId() == TYPEID_PLAYER && m_spellInfo->CategoryRecoveryTime && spellInfo->CategoryRecoveryTime
        && m_spellInfo->Category == spellInfo->Category)
        m_caster->ToPlayer()->RemoveSpellCooldown(spellInfo->Id);

    // original caster guid only for GO cast
    m_caster->CastSpell(targets, spellInfo, &values, TRIGGERED_FULL_MASK, NULL, NULLAURA_EFFECT, m_originalCasterGUID);
}

void Spell::EffectForceCast(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    switch (m_spellInfo->Id)
    {
    case 66548: // teleports outside (Isle of Conquest)
    case 66549: // teleports inside (Isle of Conquest)
    {
        if (Creature* teleportTarget = m_caster->FindNearestCreature((m_spellInfo->Id == 66548 ? 23472 : 22515), 60.0f, true))
        {
            float x, y, z, o;
            teleportTarget->GetPosition(x, y, z, o);

            if (m_caster->GetTypeId() == TYPEID_PLAYER)
                m_caster->ToPlayer()->TeleportTo(628, x, y, z, o);
        }
        break;
    }
    }

    if (!unitTarget)
        return;

    uint32 triggered_spell_id = m_spellInfo->Effects[effIndex].TriggerSpell;

    // normal case
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(triggered_spell_id);

    if (!spellInfo)
        return;

    if (m_spellInfo->Effects[effIndex].Effect == SPELL_EFFECT_FORCE_CAST && damage)
    {
        switch (m_spellInfo->Id)
        {
        case 52588: // Skeletal Gryphon Escape
        case 48598: // Ride Flamebringer Cue
            unitTarget->RemoveAura(damage);
            break;
        case 52463: // Hide In Mine Car
        case 52349: // Overtake
            unitTarget->CastCustomSpell(unitTarget, spellInfo->Id, &damage, NULL, NULL, true, NULL, NULLAURA_EFFECT, m_originalCasterGUID);
            return;
        }
    }

    CustomSpellValues values;
    // set basepoints for trigger with value effect
    if (m_spellInfo->Effects[effIndex].Effect == SPELL_EFFECT_FORCE_CAST_WITH_VALUE)
    {
        // maybe need to set value only when basepoints == 0?
        values.AddSpellMod(SPELLVALUE_BASE_POINT0, damage);
        values.AddSpellMod(SPELLVALUE_BASE_POINT1, damage);
        values.AddSpellMod(SPELLVALUE_BASE_POINT2, damage);
    }

    SpellCastTargets targets;
    targets.SetUnitTarget(m_caster);

    unitTarget->CastSpell(targets, spellInfo, &values, TRIGGERED_FULL_MASK);
}

void Spell::EffectTriggerRitualOfSummoning(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    uint32 triggered_spell_id = m_spellInfo->Effects[effIndex].TriggerSpell;
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(triggered_spell_id);

    if (!spellInfo)
        return;

    finish();

    m_caster->CastSpell((Unit*)NULL, spellInfo, false);
}

void Spell::EffectJump(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH_TARGET)
        return;

    if (m_caster->isInFlight())
        return;

    if (!unitTarget)
        return;

    float x, y, z;
    unitTarget->GetContactPoint(m_caster, x, y, z, CONTACT_DISTANCE);

    float speedXY, speedZ;
    CalculateJumpSpeeds(effIndex, m_caster->GetExactDist2d(x, y), speedXY, speedZ);
    m_caster->GetMotionMaster()->MoveJump(x, y, z, speedXY, speedZ);
}

void Spell::EffectJumpDest(SpellEffIndex p_EffIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH)
        return;

    if (m_caster->isInFlight())
        return;

    if (!m_targets.HasDst())
        return;

    /// Init dest coordinates
    float l_X, l_Y, l_Z;
    destTarget->GetPosition(l_X, l_Y, l_Z);

    float l_SpeedXY, l_SpeedZ;
    CalculateJumpSpeeds(p_EffIndex, m_caster->GetExactDist2d(l_X, l_Y), l_SpeedXY, l_SpeedZ);

    switch (m_spellInfo->Id)
    {
        case 49575: ///< Death Grip
        case 92832: ///< Leap of Faith
            m_caster->GetMotionMaster()->CustomJump(l_X, l_Y, l_Z, l_SpeedXY, l_SpeedZ);
            break;
        case 49376: ///< Wild Charge
            m_caster->GetMotionMaster()->MoveJump(l_X, l_Y, l_Z, l_SpeedXY, l_SpeedZ, destTarget->GetOrientation());
            break;
        default:
            m_caster->GetMotionMaster()->MoveJump(l_X, l_Y, l_Z, l_SpeedXY, l_SpeedZ, m_caster->GetOrientation(), m_spellInfo->Id);
            break;
    }
}

void Spell::CalculateJumpSpeeds(uint8 i, float dist, float & speedXY, float & speedZ)
{
    if (m_spellInfo->Effects[i].MiscValue)
        speedZ = float(m_spellInfo->Effects[i].MiscValue) / 10;
    else if (m_spellInfo->Effects[i].MiscValueB)
        speedZ = float(m_spellInfo->Effects[i].MiscValueB) / 10;
    else
        speedZ = 10.0f;

    if (m_spellInfo->Effects[i].ValueMultiplier)
        speedXY = m_spellInfo->Effects[i].ValueMultiplier;
    else
        speedXY = dist * 10.0f / speedZ;
}

void Spell::EffectTeleportUnits(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->isInFlight())
        return;

    if (unitTarget->GetTypeId() == TYPEID_PLAYER && m_caster->GetTypeId() == TYPEID_PLAYER)
        if (Player* l_Target = unitTarget->ToPlayer())
            sScriptMgr->OnTeleport(l_Target, m_spellInfo);

    // Pre effects
    uint8 uiMaxSafeLevel = 0;
    switch (m_spellInfo->Id)
    {
    case 48129:  // Scroll of Recall
        uiMaxSafeLevel = 40;
    case 60320:  // Scroll of Recall II
        if (!uiMaxSafeLevel)
            uiMaxSafeLevel = 70;
    case 60321:  // Scroll of Recal III
        if (!uiMaxSafeLevel)
            uiMaxSafeLevel = 80;

        if (unitTarget->getLevel() > uiMaxSafeLevel)
        {
            unitTarget->AddAura(60444, unitTarget); //Apply Lost! Aura

            // ALLIANCE from 60323 to 60330 - HORDE from 60328 to 60335

            uint32 spellId = 60323;
            if (m_caster->ToPlayer()->GetTeam() == HORDE)
                spellId += 5;
            spellId += urand(0, 7);
            m_caster->CastSpell(m_caster, spellId, true);
            return;
        }
        break;
    case 66550: // teleport outside (Isle of Conquest)
        if (Player* target = unitTarget->ToPlayer())
        {
            if (target->GetTeamId() == TEAM_ALLIANCE)
                m_targets.SetDst(442.24f, -835.25f, 44.30f, 0.06f, 628);
            else
                m_targets.SetDst(1120.43f, -762.11f, 47.92f, 2.94f, 628);
        }
        break;
    case 66551: // teleport inside (Isle of Conquest)
        if (Player* target = unitTarget->ToPlayer())
        {
            if (target->GetTeamId() == TEAM_ALLIANCE)
                m_targets.SetDst(389.57f, -832.38f, 48.65f, 3.00f, 628);
            else
                m_targets.SetDst(1174.85f, -763.24f, 48.72f, 6.26f, 628);
        }
        break;
    }

    // If not exist data for dest location - return
    if (!m_targets.HasDst())
        return;

    // Init dest coordinates
    uint32 mapid = destTarget->GetMapId();
    if (mapid == MAPID_INVALID)
        mapid = unitTarget->GetMapId();
    float x, y, z, orientation;
    destTarget->GetPosition(x, y, z, orientation);
    if (!orientation && m_targets.GetUnitTarget())
        orientation = m_targets.GetUnitTarget()->GetOrientation();

    if (mapid == unitTarget->GetMapId())
        unitTarget->NearTeleportTo(x, y, z, orientation, unitTarget == m_caster);
    else if (unitTarget->GetTypeId() == TYPEID_PLAYER)
        unitTarget->ToPlayer()->TeleportTo(mapid, x, y, z, orientation, unitTarget == m_caster ? TELE_TO_SPELL : 0);

    // post effects for TARGET_DEST_DB
    switch (m_spellInfo->Id)
    {
        // Dimensional Ripper - Everlook
    case 23442:
    {
        int32 r = irand(0, 119);
        if (r >= 70)                                  // 7/12 success
        {
            if (r < 100)                              // 4/12 evil twin
                m_caster->CastSpell(m_caster, 23445, true);
            else                                        // 1/12 fire
                m_caster->CastSpell(m_caster, 23449, true);
        }
        return;
    }
    // Ultra safe Transporter: Toshley's Station
    case 36941:
    {
        if (roll_chance_i(50))                        // 50% success
        {
            int32 rand_eff = urand(1, 7);
            switch (rand_eff)
            {
            case 1:
                // soul split - evil
                m_caster->CastSpell(m_caster, 36900, true);
                break;
            case 2:
                // soul split - good
                m_caster->CastSpell(m_caster, 36901, true);
                break;
            case 3:
                // Increase the size
                m_caster->CastSpell(m_caster, 36895, true);
                break;
            case 4:
                // Decrease the size
                m_caster->CastSpell(m_caster, 36893, true);
                break;
            case 5:
                // Transform
            {
                if (m_caster->ToPlayer()->GetTeam() == ALLIANCE)
                    m_caster->CastSpell(m_caster, 36897, true);
                else
                    m_caster->CastSpell(m_caster, 36899, true);
                break;
            }
            case 6:
                // chicken
                m_caster->CastSpell(m_caster, 36940, true);
                break;
            case 7:
                // evil twin
                m_caster->CastSpell(m_caster, 23445, true);
                break;
            }
        }
        return;
    }
    // Dimensional Ripper - Area 52
    case 36890:
    {
        if (roll_chance_i(50))                        // 50% success
        {
            int32 rand_eff = urand(1, 4);
            switch (rand_eff)
            {
            case 1:
                // soul split - evil
                m_caster->CastSpell(m_caster, 36900, true);
                break;
            case 2:
                // soul split - good
                m_caster->CastSpell(m_caster, 36901, true);
                break;
            case 3:
                // Increase the size
                m_caster->CastSpell(m_caster, 36895, true);
                break;
            case 4:
                // Transform
            {
                if (m_caster->ToPlayer()->GetTeam() == ALLIANCE)
                    m_caster->CastSpell(m_caster, 36897, true);
                else
                    m_caster->CastSpell(m_caster, 36899, true);
                break;
            }
            }
        }
        return;
    }
    }
}

void Spell::EffectApplyAura(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!m_spellAura || !unitTarget)
        return;

    ASSERT(unitTarget == m_spellAura->GetOwner());

    for (int i = 0; i < MAX_SPELL_EFFECTS; i++)
    {
        if (m_spellAura->GetEffect(i) && m_spellAura->GetEffect(i)->GetAuraType() == SPELL_AURA_SCHOOL_ABSORB)
        {
            float AbsorbMod2 = 0.0f;

            float minval = (float)unitTarget->GetMaxNegativeAuraModifier(SPELL_AURA_MOD_ABSORPTION_PCT);
            float maxval = (float)unitTarget->GetMaxPositiveAuraModifier(SPELL_AURA_MOD_ABSORPTION_PCT);

            AbsorbMod2 = minval + maxval;

            int currentValue = m_spellAura->GetEffect(i)->GetAmount();
            AddPct(currentValue, AbsorbMod2);
            m_spellAura->GetEffect(i)->SetAmount(currentValue);
        }
    }

    m_spellAura->_ApplyEffectForTargets(effIndex);
}

void Spell::EffectApplyAreaAura(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!m_spellAura || !unitTarget)
        return;
    ASSERT(unitTarget == m_spellAura->GetOwner());
    m_spellAura->_ApplyEffectForTargets(effIndex);
}

void Spell::EffectUnlearnSpecialization(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = unitTarget->ToPlayer();
    uint32 spellToUnlearn = m_spellInfo->Effects[effIndex].TriggerSpell;

    player->removeSpell(spellToUnlearn);
}

void Spell::EffectPowerDrain(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (m_spellInfo->Effects[effIndex].MiscValue < 0 || m_spellInfo->Effects[effIndex].MiscValue >= int8(MAX_POWERS))
        return;

    Powers powerType = Powers(m_spellInfo->Effects[effIndex].MiscValue);

    if (!unitTarget || !unitTarget->isAlive() || unitTarget->getPowerType() != powerType || damage < 0)
        return;

    // add spell damage bonus
    damage = m_caster->SpellDamageBonusDone(unitTarget, m_spellInfo, uint32(damage), effIndex, SPELL_DIRECT_DAMAGE);
    damage = unitTarget->SpellDamageBonusTaken(m_caster, m_spellInfo, uint32(damage), SPELL_DIRECT_DAMAGE);

    int32 newDamage = -(unitTarget->ModifyPower(powerType, -damage));

    float gainMultiplier = 0.0f;

    // Don't restore from self drain
    if (m_caster != unitTarget)
    {
        gainMultiplier = m_spellInfo->Effects[effIndex].CalcValueMultiplier(m_originalCaster, this);

        int32 gain = int32(newDamage* gainMultiplier);

        m_caster->EnergizeBySpell(m_caster, m_spellInfo->Id, gain, powerType);
    }
    ExecuteLogEffectTakeTargetPower(effIndex, unitTarget, powerType, newDamage, gainMultiplier);
}

void Spell::EffectSendEvent(SpellEffIndex effIndex)
{
    // we do not handle a flag dropping or clicking on flag in battleground by sendevent system
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET
        && effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    WorldObject* target = NULL;

    // call events for object target if present
    if (effectHandleMode == SPELL_EFFECT_HANDLE_HIT_TARGET)
    {
        if (unitTarget)
            target = unitTarget;
        else if (gameObjTarget)
            target = gameObjTarget;
    }
    else // if (effectHandleMode == SPELL_EFFECT_HANDLE_HIT)
    {
        // let's prevent executing effect handler twice in case when spell effect is capable of targeting an object
        // this check was requested by scripters, but it has some downsides:
        // now it's impossible to script (using sEventScripts) a cast which misses all targets
        // or to have an ability to script the moment spell hits dest (in a case when there are object targets present)
        if (m_spellInfo->Effects[effIndex].GetProvidedTargetMask() & (TARGET_FLAG_UNIT_MASK | TARGET_FLAG_GAMEOBJECT_MASK))
            return;
        // some spells have no target entries in dbc and they use focus target
        if (focusObject)
            target = focusObject;
        // TODO: there should be a possibility to pass dest target to event script
    }

    if (ZoneScript* zoneScript = m_caster->GetZoneScript())
        zoneScript->ProcessEvent(target, m_spellInfo->Effects[effIndex].MiscValue);
    else if (InstanceScript* instanceScript = m_caster->GetInstanceScript())    // needed in case Player is the caster
        instanceScript->ProcessEvent(target, m_spellInfo->Effects[effIndex].MiscValue);

    m_caster->GetMap()->ScriptsStart(sEventScripts, m_spellInfo->Effects[effIndex].MiscValue, m_caster, target);
}

void Spell::EffectPowerBurn(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (m_spellInfo->Effects[effIndex].MiscValue < 0 || m_spellInfo->Effects[effIndex].MiscValue >= int8(MAX_POWERS))
        return;

    Powers powerType = Powers(m_spellInfo->Effects[effIndex].MiscValue);

    if (!unitTarget || !unitTarget->isAlive() || unitTarget->getPowerType() != powerType || damage < 0)
        return;

    // burn x% of target's mana, up to maximum of 2x% of caster's mana (Mana Burn)
    if (m_spellInfo->Id == 8129)
    {
        int32 maxDamage = int32(CalculatePct(m_caster->GetMaxPower(powerType), damage * 2));
        damage = int32(CalculatePct(unitTarget->GetMaxPower(powerType), damage));
        damage = std::min(damage, maxDamage);
    }

    int32 newDamage = -(unitTarget->ModifyPower(powerType, -damage));

    // NO - Not a typo - EffectPowerBurn uses effect value multiplier - not effect damage multiplier
    float dmgMultiplier = m_spellInfo->Effects[effIndex].CalcValueMultiplier(m_originalCaster, this);

    // add log data before multiplication (need power amount, not damage)
    ExecuteLogEffectTakeTargetPower(effIndex, unitTarget, powerType, newDamage, 0.0f);

    newDamage = int32(newDamage* dmgMultiplier);

    m_damage += newDamage;
}

void Spell::EffectHeal(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH_TARGET)
        return;

    if (unitTarget && unitTarget->isAlive() && damage >= 0)
    {
        // Try to get original caster
        Unit* caster = m_originalCasterGUID ? m_originalCaster : m_caster;

        // Skip if m_originalCaster not available
        if (!caster)
            return;

        int32 addhealth = damage;

        switch (m_spellInfo->Id)
        {
            // Tipping of the Scales, Scales of Life
            case 96880:
            {
                if (constAuraEffectPtr aurEff = m_caster->GetAuraEffect(96881, EFFECT_0))
                {
                    addhealth = aurEff->GetAmount();
                    m_caster->RemoveAurasDueToSpell(96881);
                }
                else
                    return;
                break;
            }
            case 15290: // Vampiric Embrace
            {
                uint32 count = 0;
                for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                    if (ihit->effectMask & (1 << effIndex))
                        ++count;

                damage /= count;                    // divide to all targets
                break;
            }
            case 45064: // Vessel of the Naaru (Vial of the Sunwell trinket)
            {
                if (!caster)
                    break;

                // Amount of heal - depends from stacked Holy Energy
                int damageAmount = 0;
                if (constAuraEffectPtr aurEff = caster->GetAuraEffect(45062, 0))
                {
                    damageAmount += aurEff->GetAmount();
                    caster->RemoveAurasDueToSpell(45062);
                }

                addhealth += damageAmount;
                addhealth = caster->SpellHealingBonusDone(unitTarget, m_spellInfo, addhealth, effIndex, HEAL);

                break;
            }
            case 67489: // Runic Healing Injector (heal increased by 25% for engineers - 3.2.0 patch change)
                if (!caster)
                    break;

                if (Player* player = caster->ToPlayer())
                    if (player->HasSkill(SKILL_ENGINEERING))
                        AddPct(addhealth, 25);
                break;
            case 86961: // Cleansing Waters
            {
                addhealth = m_caster->CountPctFromMaxHealth(4);
                break;
            }
            case 73921: // Healing Rain
            {
                addhealth = caster->SpellHealingBonusDone(unitTarget, m_spellInfo, addhealth, effIndex, HEAL);

                // Increase the effectiveness by 30% with Unleashed Life
                if (AuraEffectPtr healingRain = caster->GetAuraEffect(142923, EFFECT_1))
                    AddPct(addhealth, healingRain->GetAmount());
                break;
            }
            default:
                if (!caster)
                    break;

                addhealth = caster->SpellHealingBonusDone(unitTarget, m_spellInfo, addhealth, effIndex, HEAL);
                break;
        }

        addhealth = unitTarget->SpellHealingBonusTaken(caster, m_spellInfo, addhealth, HEAL);

        // Remove Grievous bite if fully healed
        if (unitTarget->HasAura(48920) && (unitTarget->GetHealth() + addhealth >= unitTarget->GetMaxHealth()))
            unitTarget->RemoveAura(48920);

        // Custom MoP Script
        // 77495 - Mastery : Harmony
        if (caster && caster->GetTypeId() == TYPEID_PLAYER && caster->getClass() == CLASS_DRUID)
        {
            if (caster->HasAura(77495))
            {
                if (addhealth)
                {
                    float Mastery = caster->GetFloatValue(PLAYER_FIELD_MASTERY) * 1.25 / 100.0f;

                    if (m_spellInfo->HasEffect(SPELL_EFFECT_HEAL))
                    {
                        addhealth *= (1 + Mastery);

                        int32 bp = int32(100.0f * Mastery);

                        caster->CastCustomSpell(caster, 100977, &bp, NULL, NULL, true);
                    }
                }
            }
        }
        // 77226 - Mastery : Deep Healing
        if (caster && caster->GetTypeId() == TYPEID_PLAYER && caster->getClass() == CLASS_SHAMAN)
        {
            if (caster->HasAura(77226))
            {
                if (addhealth)
                {
                    float Mastery = caster->GetFloatValue(PLAYER_FIELD_MASTERY) * 3.0f / 100.0f;
                    float healthpct = unitTarget->GetHealthPct();

                    float bonus = 0;
                    bonus = CalculatePct((1 + (100.0f - healthpct)), Mastery);

                    addhealth *= 1 + bonus;
                }
            }
        }

        // 77485 - Mastery : Echo of Light
        if (caster && caster->getClass() == CLASS_PRIEST && caster->HasAura(77485) && caster->getLevel() >= 80 && addhealth)
        {
            float Mastery = caster->GetFloatValue(PLAYER_FIELD_MASTERY) * 1.25f / 100.0f;
            int32 bp = (Mastery * addhealth) / 6;

            bp += unitTarget->GetRemainingPeriodicAmount(caster->GetGUID(), 77489, SPELL_AURA_PERIODIC_HEAL);

            m_caster->CastCustomSpell(unitTarget, 77489, &bp, NULL, NULL, true);
        }

        // Chakra : Serenity - 81208
        if (caster && addhealth && caster->HasAura(81208) && m_spellInfo->Effects[0].TargetA.GetTarget() == TARGET_UNIT_TARGET_ALLY) // Single heal target
            if (AuraPtr renew = unitTarget->GetAura(139, caster->GetGUID()))
                renew->RefreshDuration();

        // Mogu'Shan Vault
        if (caster && (caster->HasAura(116161) || unitTarget->HasAura(116161))) // SPELL_CROSSED_OVER
        {
            // http://fr.wowhead.com/spell=117549#english-comments
            // uint32 targetSpec = unitTarget->ToPlayer()->GetSpecializationId(unitTarget->ToPlayer()->GetActiveSpec());
            uint32 innervationId = 0;

            if (unitTarget == caster)
            {
                int32 bp1 = addhealth / 2;
                int32 bp2 = 15;

                caster->CastCustomSpell(unitTarget, 117543, &bp1, &bp2, NULL, NULL, NULL, NULL, true); // Mana regen bonus
            }
            else
            {
                int32 bp1 = 10;
                int32 bp2 = 15;
                int32 bp3 = 20;
                int32 bp4 = 25;
                int32 bp5 = 30;
                int32 bp6 = 35;

                caster->CastCustomSpell(unitTarget, 117549, &bp1, &bp2, &bp3, &bp4, &bp5, &bp6, true);
            }

            if (unitTarget->GetHealth() + addhealth >= unitTarget->GetMaxHealth())
                unitTarget->CastSpell(unitTarget, 120717, true);  // Revitalized Spirit
        }

        if (m_spellInfo->Id == 27827)
            addhealth = m_caster->GetMaxHealth();

        m_damage -= addhealth;
    }
}

void Spell::EffectHealPct(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->isAlive() || damage < 0)
        return;

    // Skip if m_originalCaster not available
    if (!m_originalCaster)
        return;

    switch (m_spellInfo->Id)
    {
    case 6262:  // Healthstone
        if (m_caster->HasAura(56224)) // Glyph of Healthstone
            return;
        break;
    case 59754: // Rune Tap - Party
        if (unitTarget == m_caster)
            return;
        break;
    case 115450: // Detox
        if (!m_caster->HasAura(146954))
            return;
        break;
    case 118340:// Impending Victory - Heal
        // Victorious State causes your next Impending Victory to heal for 20% of your maximum health.
        if (m_caster->HasAura(32216))
        {
            damage = 20;
            m_caster->RemoveAurasDueToSpell(32216);
        }
        break;
    case 137562:// Nimble Brew
        if (!m_caster->HasAura(146952)) // Glyph of Nimble Brew
            return;
        break;
    default:
        break;
    }

    uint32 heal = m_originalCaster->SpellHealingBonusDone(unitTarget, m_spellInfo, unitTarget->CountPctFromMaxHealth(damage), effIndex, HEAL);
    heal = unitTarget->SpellHealingBonusTaken(m_originalCaster, m_spellInfo, heal, HEAL);

    m_healing += heal;
}

void Spell::EffectHealMechanical(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->isAlive() || damage < 0)
        return;

    // Skip if m_originalCaster not available
    if (!m_originalCaster)
        return;

    uint32 heal = m_originalCaster->SpellHealingBonusDone(unitTarget, m_spellInfo, uint32(damage), effIndex, HEAL);

    m_healing += unitTarget->SpellHealingBonusTaken(m_originalCaster, m_spellInfo, heal, HEAL);
}

void Spell::EffectHealthLeech(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->isAlive() || damage < 0)
        return;

    damage = m_caster->SpellDamageBonusDone(unitTarget, m_spellInfo, uint32(damage), effIndex, SPELL_DIRECT_DAMAGE);
    damage = unitTarget->SpellDamageBonusTaken(m_caster, m_spellInfo, uint32(damage), SPELL_DIRECT_DAMAGE);

    float healMultiplier = m_spellInfo->Effects[effIndex].CalcValueMultiplier(m_originalCaster, this);

    m_damage += damage;
    // get max possible damage, don't count overkill for heal
    uint32 healthGain = uint32(-unitTarget->GetHealthGain(-damage) * healMultiplier);

    if (m_caster->isAlive())
    {
        healthGain = m_caster->SpellHealingBonusDone(m_caster, m_spellInfo, healthGain, effIndex, HEAL);
        healthGain = m_caster->SpellHealingBonusTaken(m_caster, m_spellInfo, healthGain, HEAL);

        m_caster->HealBySpell(m_caster, m_spellInfo, uint32(healthGain));
    }
}

void Spell::DoCreateItem(uint32 /*i*/, uint32 itemtype, bool vellum)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = unitTarget->ToPlayer();

    uint32 newitemid = itemtype;
    ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(newitemid);
    if (!pProto)
    {
        player->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
        return;
    }

    // bg reward have some special in code work
    uint32 bgType = 0;
    switch (m_spellInfo->Id)
    {
    case SPELL_AV_MARK_WINNER:
    case SPELL_AV_MARK_LOSER:
        bgType = BATTLEGROUND_AV;
        break;
    case SPELL_WS_MARK_WINNER:
    case SPELL_WS_MARK_LOSER:
        bgType = BATTLEGROUND_WS;
        break;
    case SPELL_AB_MARK_WINNER:
    case SPELL_AB_MARK_LOSER:
        bgType = BATTLEGROUND_AB;
        break;
    default:
        break;
    }

    uint32 num_to_add = vellum ? 1 : damage;

    if (num_to_add < 1)
        num_to_add = 1;
    if (num_to_add > pProto->GetMaxStackSize())
        num_to_add = pProto->GetMaxStackSize();

    // init items_count to 1, since 1 item will be created regardless of specialization
    int items_count = 1;
    // the chance to create additional items
    float additionalCreateChance = 0.0f;
    // the maximum number of created additional items
    uint8 additionalMaxNum = 0;
    // get the chance and maximum number for creating extra items
    if (canCreateExtraItems(player, m_spellInfo->Id, additionalCreateChance, additionalMaxNum))
    {
        // roll with this chance till we roll not to create or we create the max num
        while (roll_chance_f(additionalCreateChance) && items_count <= additionalMaxNum)
            ++items_count;
    }

    // really will be created more items
    num_to_add *= items_count;

    // can the player store the new item?
    ItemPosCountVec dest;
    uint32 no_space = 0;
    InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, newitemid, num_to_add, &no_space);
    if (msg != EQUIP_ERR_OK)
    {
        // convert to possible store amount
        if (msg == EQUIP_ERR_INV_FULL || msg == EQUIP_ERR_ITEM_MAX_COUNT)
            num_to_add -= no_space;
        else
        {
            // if not created by another reason from full inventory or unique items amount limitation
            player->SendEquipError(msg, NULL, NULL, newitemid);
            return;
        }
    }

    if (num_to_add)
    {
        // create the new item and store it
        Item* pItem = player->StoreNewItem(dest, newitemid, true, Item::GenerateItemRandomPropertyId(newitemid));

        /*if (pProto->Quality > ITEM_QUALITY_EPIC || (pProto->Quality == ITEM_QUALITY_EPIC && pProto->ItemLevel >= MinNewsItemLevel[sWorld->getIntConfig(CONFIG_EXPANSION)]))
        if (Guild* guild = sGuildMgr->GetGuildById(player->GetGuildId()))
        guild->GetNewsLog().AddNewEvent(GUILD_NEWS_ITEM_CRAFTED, time(NULL), player->GetGUID(), 0, pProto->ItemId);*/

        // was it successful? return error if not
        if (!pItem)
        {
            player->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
            return;
        }

        // set the "Crafted by ..." property of the item
        if (pItem->GetTemplate()->Class != ITEM_CLASS_CONSUMABLE && pItem->GetTemplate()->Class != ITEM_CLASS_QUEST && newitemid != 6265 && newitemid != 6948)
            pItem->SetGuidValue(ITEM_FIELD_CREATOR, player->GetGUID());

        // send info to the client
        player->SendNewItem(pItem, num_to_add, true, bgType == 0);

        // we succeeded in creating at least one item, so a level up is possible
        if (bgType == 0)
            player->UpdateCraftSkill(m_spellInfo->Id);
    }

    if (Guild* l_Guild = player->GetGuild())
        l_Guild->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_CRAFT_ITEMS_GUILD, 1, 0, 0, nullptr, player);
}

void Spell::EffectCreateItem(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (m_caster->GetTypeId() == TYPEID_PLAYER && (m_spellInfo->IsAbilityOfSkillType(SKILL_ARCHAEOLOGY) || m_spellInfo->IsCustomArchaeologySpell()))
    {
        if (!m_caster->ToPlayer()->GetArchaeologyMgr().SolveResearchProject(m_spellInfo->ResearchProject))
            return;
    }

    DoCreateItem(effIndex, m_spellInfo->Effects[effIndex].ItemType);
    ExecuteLogEffectCreateItem(effIndex, m_spellInfo->Effects[effIndex].ItemType);
}

void Spell::EffectCreateItem2(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = unitTarget->ToPlayer();

    if (player && (m_spellInfo->IsAbilityOfSkillType(SKILL_ARCHAEOLOGY) || m_spellInfo->IsCustomArchaeologySpell()))
    {
        if (!player->GetArchaeologyMgr().SolveResearchProject(m_spellInfo->ResearchProject))
            return;

        // Some MoP's projects give no skill points
        SkillLineAbilityMapBounds bounds = sSpellMgr->GetSkillLineAbilityMapBounds(m_spellInfo->Id);

        bool found = false;
        for (SkillLineAbilityMap::const_iterator _spell_idx = bounds.first; _spell_idx != bounds.second; ++_spell_idx)
        {
            if (_spell_idx->second->skillId)
            {
                found = true;
                break;
            }
        }

        if (!found)
            player->UpdateSkillPro(SKILL_ARCHAEOLOGY, 1000, 5);
    }

    uint32 item_id = m_spellInfo->Effects[effIndex].ItemType;

    // Some recipes can proc for more valueable items (cataclysm)
    uint32 new_id = 0;
    switch (item_id)
    {
    case 52309: new_id = 52314; break; // Nightstone Choker
    case 52308: new_id = 52316; break; // Hessonite Band
    case 52307: new_id = 52312; break; // Alicite Pendant
    case 52306: new_id = 52310; break; // Jasper Ring
    default: break;
    }

    if (new_id && roll_chance_i(10))
        item_id = new_id;

    if (item_id)
        DoCreateItem(effIndex, item_id);

    // special case: fake item replaced by generate using spell_loot_template
    if (m_spellInfo->IsLootCrafting())
    {
        if (item_id)
        {
            if (!player->HasItemCount(item_id))
                return;

            // remove reagent
            uint32 count = 1;
            player->DestroyItemCount(item_id, count, true);

            // create some random items
            player->AutoStoreLoot(m_spellInfo->Id, LootTemplates_Spell);
        }
        else
            player->AutoStoreLoot(m_spellInfo->Id, LootTemplates_Spell);    // create some random items
    }
    // TODO: ExecuteLogEffectCreateItem(i, m_spellInfo->Effects[i].ItemType);
}

void Spell::EffectCreateRandomItem(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    Player* player = unitTarget->ToPlayer();

    // create some random items
    player->AutoStoreLoot(m_spellInfo->Id, LootTemplates_Spell);
    // TODO: ExecuteLogEffectCreateItem(i, m_spellInfo->Effects[i].ItemType);
}

void Spell::EffectPersistentAA(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (!m_spellAura)
    {
        Unit* caster = m_caster->GetEntry() == WORLD_TRIGGER ? m_originalCaster : m_caster;
        float radius = m_spellInfo->Effects[effIndex].CalcRadius(caster);

        // Caster not in world, might be spell triggered from aura removal
        if (!caster->IsInWorld())
            return;
        DynamicObject* dynObj = new DynamicObject(false);
        if (!dynObj->CreateDynamicObject(sObjectMgr->GenerateLowGuid(HIGHGUID_DYNAMICOBJECT), caster, m_spellInfo, *destTarget, radius, DYNAMIC_OBJECT_AREA_SPELL))
        {
            delete dynObj;
            return;
        }

        // Glyph of Explosive Trap
        if (m_spellInfo->Id == 13812 && caster->HasAura(119403))
        {
            m_caster->CastSpell(dynObj->GetPositionX(), dynObj->GetPositionY(), dynObj->GetPositionZ(), 149575, true);
            dynObj->SetDuration(0);
            return;
        }

        AuraPtr aura = Aura::TryCreate(m_spellInfo, MAX_EFFECT_MASK, dynObj, caster, &m_spellValue->EffectBasePoints[0]);
        if (aura != NULLAURA)
        {
            m_spellAura = aura;
            m_spellAura->_RegisterForTargets();
        }
        else
            return;
    }

    ASSERT(m_spellAura->GetDynobjOwner());
    m_spellAura->_ApplyEffectForTargets(effIndex);
}

void Spell::EffectEnergize(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;
    if (!unitTarget->isAlive())
        return;

    if (m_spellInfo->Effects[effIndex].MiscValue < 0 || m_spellInfo->Effects[effIndex].MiscValue >= int8(MAX_POWERS))
        return;

    Powers power = Powers(m_spellInfo->Effects[effIndex].MiscValue);

    // Some level depends spells
    int level_multiplier = 0;
    int level_diff = 0;
    switch (m_spellInfo->Id)
    {
        case 6572:  // Revenge
            if (m_caster->GetShapeshiftForm() != FORM_DEFENSIVESTANCE)
                return;
            break;
        case 9512:                                          // Restore Energy
            level_diff = m_caster->getLevel() - 40;
            level_multiplier = 2;
            break;
        case 24571:                                         // Blood Fury
            level_diff = m_caster->getLevel() - 60;
            level_multiplier = 10;
            break;
        case 24532:                                         // Burst of Energy
            level_diff = m_caster->getLevel() - 60;
            level_multiplier = 4;
            break;
        case 63375:                                         // Primal Wisdom
            damage = int32(CalculatePct(unitTarget->GetCreateMana(), damage));
            break;
        case 67490:                                         // Runic Mana Injector (mana gain increased by 25% for engineers - 3.2.0 patch change)
        {
            if (Player* player = m_caster->ToPlayer())
                if (player->HasSkill(SKILL_ENGINEERING))
                    AddPct(damage, 25);
            break;
        }
        case 92601: // Detonate Mana, Tyrande's Favorite Doll
            if (constAuraEffectPtr aurEff = m_caster->GetAuraEffect(92596, EFFECT_0))
            {
                damage = aurEff->GetAmount();
                m_caster->RemoveAurasDueToSpell(92596);
            }
            break;
        case 99069: // Fires of Heaven, Item - Paladin T12 Holy 2P Bonus
        case 99007: // Heartfire, Item - Druid T12 Restoration 2P Bonus
        case 99131: // Divine Fire, Item - Mage T12 2P Bonus
        case 99189: // Flametide, Item - Shaman T12 Restoration 2P Bonus
            damage = int32(CalculatePct(unitTarget->GetCreateMana(), damage));
            break;
        case 35395: // Crusader Strike
            if (uint64 targetGUID = m_targets.GetUnitTargetGUID())
                for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                {
                    if (ihit->targetGUID == targetGUID)
                    {
                        if (ihit->missCondition != SPELL_MISS_NONE)
                            return;
                    }
                }
            break;
        default:
            break;
    }

    if (level_diff > 0)
        damage -= level_multiplier * level_diff;

    if (!damage)
        return;

    if (unitTarget->GetMaxPower(power) == 0)
        return;

    m_caster->EnergizeBySpell(unitTarget, m_spellInfo->Id, damage, power);

    // Mad Alchemist's Potion
    if (m_spellInfo->Id == 45051)
    {
        // find elixirs on target
        bool guardianFound = false;
        bool battleFound = false;
        Unit::AuraApplicationMap& Auras = unitTarget->GetAppliedAuras();
        for (Unit::AuraApplicationMap::iterator itr = Auras.begin(); itr != Auras.end(); ++itr)
        {
            uint32 spell_id = itr->second->GetBase()->GetId();
            if (!guardianFound)
            {
                if (sSpellMgr->IsSpellMemberOfSpellGroup(spell_id, SPELL_GROUP_ELIXIR_GUARDIAN))
                    guardianFound = true;
            }
            if (!battleFound)
            {
                if (sSpellMgr->IsSpellMemberOfSpellGroup(spell_id, SPELL_GROUP_ELIXIR_BATTLE))
                    battleFound = true;
            }
            if (battleFound && guardianFound)
                break;
        }

        // get all available elixirs by mask and spell level
        std::set<uint32> avalibleElixirs;
        if (!guardianFound)
            sSpellMgr->GetSetOfSpellsInSpellGroup(SPELL_GROUP_ELIXIR_GUARDIAN, avalibleElixirs);
        if (!battleFound)
            sSpellMgr->GetSetOfSpellsInSpellGroup(SPELL_GROUP_ELIXIR_BATTLE, avalibleElixirs);
        for (std::set<uint32>::iterator itr = avalibleElixirs.begin(); itr != avalibleElixirs.end();)
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(*itr);
            if (spellInfo->SpellLevel < m_spellInfo->SpellLevel || spellInfo->SpellLevel > unitTarget->getLevel())
                avalibleElixirs.erase(itr++);
            else if (sSpellMgr->IsSpellMemberOfSpellGroup(*itr, SPELL_GROUP_ELIXIR_SHATTRATH))
                avalibleElixirs.erase(itr++);
            else if (sSpellMgr->IsSpellMemberOfSpellGroup(*itr, SPELL_GROUP_ELIXIR_UNSTABLE))
                avalibleElixirs.erase(itr++);
            else
                ++itr;
        }

        if (!avalibleElixirs.empty())
        {
            // cast random elixir on target
            m_caster->CastSpell(unitTarget, JadeCore::Containers::SelectRandomContainerElement(avalibleElixirs), true, m_CastItem);
        }
    }
}

void Spell::EffectEnergizePct(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;
    if (!unitTarget->isAlive())
        return;

    if (m_spellInfo->Effects[effIndex].MiscValue < 0 || m_spellInfo->Effects[effIndex].MiscValue >= int8(MAX_POWERS))
        return;

    Powers power = Powers(m_spellInfo->Effects[effIndex].MiscValue);

    uint32 maxPower = unitTarget->GetMaxPower(power);
    if (maxPower == 0)
        return;

    uint32 gain = CalculatePct(maxPower, damage);
    m_caster->EnergizeBySpell(unitTarget, m_spellInfo->Id, gain, power);
}

void Spell::SendLoot(uint64 guid, LootType loottype)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = m_caster->ToPlayer();
    if (!player)
        return;

    if (gameObjTarget)
    {
        // Players shouldn't be able to loot gameobjects that are currently despawned
        if (!gameObjTarget->isSpawned() && !player->isGameMaster())
        {
            sLog->outError(LOG_FILTER_SPELLS_AURAS, "Possible hacking attempt: Player %s [guid: %u] tried to loot a gameobject [entry: %u id: %u] which is on respawn time without being in GM mode!",
                player->GetName(), player->GetGUIDLow(), gameObjTarget->GetEntry(), gameObjTarget->GetGUIDLow());
            return;
        }
        // special case, already has GossipHello inside so return and avoid calling twice
        if (gameObjTarget->GetGoType() == GAMEOBJECT_TYPE_GOOBER)
        {
            gameObjTarget->Use(m_caster);
            return;
        }

        if (sScriptMgr->OnGossipHello(player, gameObjTarget))
            return;

        if (gameObjTarget->AI()->GossipHello(player))
            return;

        switch (gameObjTarget->GetGoType())
        {
        case GAMEOBJECT_TYPE_DOOR:
        case GAMEOBJECT_TYPE_BUTTON:
            gameObjTarget->UseDoorOrButton(0, false, player);
            player->GetMap()->ScriptsStart(sGameObjectScripts, gameObjTarget->GetDBTableGUIDLow(), player, gameObjTarget);
            return;

        case GAMEOBJECT_TYPE_QUESTGIVER:
            player->PrepareGossipMenu(gameObjTarget, gameObjTarget->GetGOInfo()->questgiver.gossipID, true);
            player->SendPreparedGossip(gameObjTarget);
            return;

        case GAMEOBJECT_TYPE_SPELL_FOCUS:
            // triggering linked GO
            if (uint32 trapEntry = gameObjTarget->GetGOInfo()->spellFocus.linkedTrap)
                gameObjTarget->TriggeringLinkedGameObject(trapEntry, m_caster);
            return;

        case GAMEOBJECT_TYPE_CHEST:
            // TODO: possible must be moved to loot release (in different from linked triggering)
            if (gameObjTarget->GetGOInfo()->chest.triggeredEvent)
                player->GetMap()->ScriptsStart(sEventScripts, gameObjTarget->GetGOInfo()->chest.triggeredEvent, player, gameObjTarget);

            // triggering linked GO
            if (uint32 trapEntry = gameObjTarget->GetGOInfo()->chest.linkedTrap)
                gameObjTarget->TriggeringLinkedGameObject(trapEntry, m_caster);

            // Don't return, let loots been taken
        default:
            break;
        }
    }

    // Send loot
    player->SendLoot(guid, loottype);
}

void Spell::EffectOpenLock(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = m_caster->ToPlayer();

    uint32 lockId = 0;
    uint64 guid = 0;

    // Get lockId
    if (gameObjTarget)
    {
        GameObjectTemplate const* goInfo = gameObjTarget->GetGOInfo();

        if (goInfo->type == GAMEOBJECT_TYPE_GOOBER && player->GetGarrison())
            player->GetGarrison()->SetLastUsedActivationGameObject(gameObjTarget->GetGUID());

        // Arathi Basin banner opening. // TODO: Verify correctness of this check
        if ((goInfo->type == GAMEOBJECT_TYPE_BUTTON && goInfo->button.noDamageImmune) ||
            (goInfo->type == GAMEOBJECT_TYPE_GOOBER && goInfo->goober.requireLOS) ||
            (goInfo->type == GAMEOBJECT_TYPE_CAPTURE_POINT))
        {
            //CanUseBattlegroundObject() already called in CheckCast()
            // in battleground check
            if (Battleground* bg = player->GetBattleground())
            {
                bg->EventPlayerClickedOnFlag(player, gameObjTarget);
                return;
            }
        }
        else if (goInfo->type == GAMEOBJECT_TYPE_FLAGSTAND)
        {
            //CanUseBattlegroundObject() already called in CheckCast()
            // in battleground check
            if (Battleground* bg = player->GetBattleground())
            {
                if (bg->GetTypeID(true) == BATTLEGROUND_EY || bg->GetTypeID(true) == BATTLEGROUND_EYR)
                    bg->EventPlayerClickedOnFlag(player, gameObjTarget);
                return;
            }
        }
        else if (m_spellInfo->Id == 1842 && gameObjTarget->GetGOInfo()->type == GAMEOBJECT_TYPE_TRAP && gameObjTarget->GetOwner())
        {
            gameObjTarget->SetLootState(GO_JUST_DEACTIVATED);
            return;
        }
        // TODO: Add script for spell 41920 - Filling, becouse server it freze when use this spell
        // handle outdoor pvp object opening, return true if go was registered for handling
        // these objects must have been spawned by outdoorpvp!
        else if (gameObjTarget->GetGOInfo()->type == GAMEOBJECT_TYPE_GOOBER && sOutdoorPvPMgr->HandleOpenGo(player, gameObjTarget->GetGUID()))
            return;
        lockId = goInfo->GetLockId();
        guid = gameObjTarget->GetGUID();
    }
    else if (itemTarget)
    {
        lockId = itemTarget->GetTemplate()->LockID;
        guid = itemTarget->GetGUID();
    }
    else
        return;

    SkillType skillId = SKILL_NONE;
    int32 reqSkillValue = 0;
    int32 skillValue;

    SpellCastResult res = CanOpenLock(effIndex, lockId, skillId, reqSkillValue, skillValue);
    if (res != SPELL_CAST_OK)
    {
        SendCastResult(res);
        return;
    }

    if (reqSkillValue == 0)
    {
        switch (skillId)
        {
            case SKILL_BLACKSMITHING:
            case SKILL_LEATHERWORKING:
            case SKILL_ALCHEMY:
            case SKILL_HERBALISM:
            case SKILL_COOKING:
            case SKILL_MINING:
            case SKILL_TAILORING:
            case SKILL_ENGINEERING:
            case SKILL_ENCHANTING:
            case SKILL_SKINNING:
            case SKILL_JEWELCRAFTING:
            case SKILL_RUNEFORGING:
            case SKILL_ARCHAEOLOGY:
                reqSkillValue = skillValue;
                break;

            default:
                break;
        }
    }

    if (gameObjTarget)
        SendLoot(guid, LOOT_SKINNING);
    else if (itemTarget)
        itemTarget->SetFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FLAG_UNLOCKED);

    // not allow use skill grow at item base open
    if (!m_CastItem && skillId != SKILL_NONE)
    {
        // update skill if really known
        if (uint32 pureSkillValue = player->GetPureSkillValue(skillId))
        {
            if (gameObjTarget)
            {
                // Allow one skill-up until respawned
                if (!gameObjTarget->IsInSkillupList(player->GetGUIDLow()) &&
                    player->UpdateGatherSkill(skillId, pureSkillValue, reqSkillValue))
                {
                    gameObjTarget->AddToSkillupList(player->GetGUIDLow());

                    // Update player XP
                    // Patch 4.0.1 (2010-10-12): Gathering herbs and Mining will give XP
                    if (skillId == SKILL_MINING || skillId == SKILL_HERBALISM)
                        player->GiveGatheringXP();
                }
            }
            else if (itemTarget)
            {
                // Do one skill-up
                player->UpdateGatherSkill(skillId, pureSkillValue, reqSkillValue);
            }
        }
    }
    ExecuteLogEffectOpenLock(effIndex, gameObjTarget ? (Object*)gameObjTarget : (Object*)itemTarget);
}

void Spell::EffectSummonChangeItem(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = m_caster->ToPlayer();

    // applied only to using item
    if (!m_CastItem)
        return;

    // ... only to item in own inventory/bank/equip_slot
    if (m_CastItem->GetOwnerGUID() != player->GetGUID())
        return;

    uint32 newitemid = m_spellInfo->Effects[effIndex].ItemType;
    if (!newitemid)
        return;

    uint16 pos = m_CastItem->GetPos();

    Item* pNewItem = Item::CreateItem(newitemid, 1, player);
    if (!pNewItem)
        return;

    for (uint8 j = PERM_ENCHANTMENT_SLOT; j <= TEMP_ENCHANTMENT_SLOT; ++j)
        if (m_CastItem->GetEnchantmentId(EnchantmentSlot(j)))
            pNewItem->SetEnchantment(EnchantmentSlot(j), m_CastItem->GetEnchantmentId(EnchantmentSlot(j)), m_CastItem->GetEnchantmentDuration(EnchantmentSlot(j)), m_CastItem->GetEnchantmentCharges(EnchantmentSlot(j)));

    if (m_CastItem->GetUInt32Value(ITEM_FIELD_DURABILITY) < m_CastItem->GetUInt32Value(ITEM_FIELD_MAX_DURABILITY))
    {
        double lossPercent = 1 - m_CastItem->GetUInt32Value(ITEM_FIELD_DURABILITY) / double(m_CastItem->GetUInt32Value(ITEM_FIELD_MAX_DURABILITY));
        player->DurabilityLoss(pNewItem, lossPercent);
    }

    if (player->IsInventoryPos(pos))
    {
        ItemPosCountVec dest;
        InventoryResult msg = player->CanStoreItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), dest, pNewItem, true);
        if (msg == EQUIP_ERR_OK)
        {
            player->DestroyItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), true);

            // prevent crash at access and unexpected charges counting with item update queue corrupt
            if (m_CastItem == m_targets.GetItemTarget())
                m_targets.SetItemTarget(NULL);

            m_CastItem = NULL;

            player->StoreItem(dest, pNewItem, true);
            return;
        }
    }
    else if (player->IsBankPos(pos))
    {
        ItemPosCountVec dest;
        uint8 msg = player->CanBankItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), dest, pNewItem, true);
        if (msg == EQUIP_ERR_OK)
        {
            player->DestroyItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), true);

            // prevent crash at access and unexpected charges counting with item update queue corrupt
            if (m_CastItem == m_targets.GetItemTarget())
                m_targets.SetItemTarget(NULL);

            m_CastItem = NULL;

            player->BankItem(dest, pNewItem, true);
            return;
        }
    }
    else if (player->IsEquipmentPos(pos))
    {
        uint16 dest;

        player->DestroyItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), true);

        uint8 msg = player->CanEquipItem(m_CastItem->GetSlot(), dest, pNewItem, true);

        if (msg == EQUIP_ERR_OK || msg == EQUIP_ERR_CLIENT_LOCKED_OUT)
        {
            if (msg == EQUIP_ERR_CLIENT_LOCKED_OUT) dest = EQUIPMENT_SLOT_MAINHAND;

            // prevent crash at access and unexpected charges counting with item update queue corrupt
            if (m_CastItem == m_targets.GetItemTarget())
                m_targets.SetItemTarget(NULL);

            m_CastItem = NULL;

            player->EquipItem(dest, pNewItem, true);
            player->AutoUnequipOffhandIfNeed();
            return;
        }
    }

    // fail
    delete pNewItem;
}

void Spell::EffectProficiency(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;
    Player* p_target = m_caster->ToPlayer();

    uint32 subClassMask = m_spellInfo->EquippedItemSubClassMask;
    if (m_spellInfo->EquippedItemClass == ITEM_CLASS_WEAPON && !(p_target->GetWeaponProficiency() & subClassMask))
    {
        p_target->AddWeaponProficiency(subClassMask);
        p_target->SendProficiency(ITEM_CLASS_WEAPON, p_target->GetWeaponProficiency());
    }
    if (m_spellInfo->EquippedItemClass == ITEM_CLASS_ARMOR && !(p_target->GetArmorProficiency() & subClassMask))
    {
        p_target->AddArmorProficiency(subClassMask);
        p_target->SendProficiency(ITEM_CLASS_ARMOR, p_target->GetArmorProficiency());
    }
}

void Spell::EffectSummonType(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    uint32 entry = m_spellInfo->Effects[effIndex].MiscValue;
    if (!entry)
        return;

    SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(m_spellInfo->Effects[effIndex].MiscValueB);
    if (!properties)
        return;

    if (!m_originalCaster)
        return;

    switch (m_spellInfo->Id)
    {
        case 124927:// Call Dog
            if (m_originalCaster->ToPlayer())
                m_originalCaster->ToPlayer()->AddSpellCooldown(m_spellInfo->Id, 0, 60 * IN_MILLISECONDS);
            break;
        case 123040:// Mindbender
            // Glyph of the Sha
            if (m_originalCaster->HasAura(147776))
            {
                entry = sSpellMgr->GetSpellInfo(132604)->Effects[effIndex].MiscValue;

                SummonPropertiesEntry const* newProperties = sSummonPropertiesStore.LookupEntry(sSpellMgr->GetSpellInfo(132604)->Effects[effIndex].MiscValueB);
                if (newProperties)
                    properties = newProperties;
            }

            ((SummonPropertiesEntry*)properties)->Flags |= 0x200; // Controllable guardian ?
            ((SummonPropertiesEntry*)properties)->Category = SUMMON_CATEGORY_PET;
            ((SummonPropertiesEntry*)properties)->Type = SUMMON_TYPE_PET;
            break;
        case 113890:// Demonic Gateway : Remove old summon when cast an other gate
        case 113886:// Demonic Gateway : Remove old summon when cast an other gate
            if (m_spellInfo->Id == 113890)
            {
                std::list<Creature*> tempList;
                std::list<Creature*> gatewayList;

                m_caster->GetCreatureListWithEntryInGrid(tempList, 59271, 500.0f);

                if (!tempList.empty())
                {
                    for (auto itr : tempList)
                        gatewayList.push_back(itr);

                    // Remove other players mushrooms
                    for (std::list<Creature*>::iterator i = tempList.begin(); i != tempList.end(); ++i)
                    {
                        Unit* owner = (*i)->GetOwner();
                        if (owner && owner == m_caster && (*i)->isSummon())
                            continue;

                        gatewayList.remove((*i));
                    }

                    // 1 gateway max
                    if ((int32)gatewayList.size() >= 1)
                        gatewayList.back()->ToTempSummon()->UnSummon();
                }
            }
            else
            {
                std::list<Creature*> tempList;
                std::list<Creature*> gatewayList;

                m_caster->GetCreatureListWithEntryInGrid(tempList, 59262, 500.0f);

                if (!tempList.empty())
                {
                    for (auto itr : tempList)
                        gatewayList.push_back(itr);

                    // Remove other players mushrooms
                    for (std::list<Creature*>::iterator i = tempList.begin(); i != tempList.end(); ++i)
                    {
                        Unit* owner = (*i)->GetOwner();
                        if (owner && owner == m_caster && (*i)->isSummon())
                            continue;

                        gatewayList.remove((*i));
                    }

                    // 1 gateway max
                    if ((int32)gatewayList.size() >= 1)
                        gatewayList.back()->ToTempSummon()->UnSummon();
                }
            }
            break;
        case 33663:
        case 117663:
            if (m_originalCaster->GetTypeId() == TYPEID_UNIT)
            {
                if (m_originalCaster->isTotem() && m_originalCaster->GetOwner())
                {
                    if (m_originalCaster->GetOwner()->HasAura(117013))
                    {
                        m_originalCaster->CastSpell(m_originalCaster, m_spellInfo->Id == 33663 ? 118323 : 118291, true);
                        return;
                    }
                }
            }
            break;
        default:
            break;
    }

    int32 duration = m_spellInfo->GetDuration();
    if (Player* modOwner = m_originalCaster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_DURATION, duration);

    // Item - Warlock T13 2P Bonus (Doomguard and Infernal)
    if (entry == 11859 || entry == 89)
        if (constAuraEffectPtr aurEff = m_originalCaster->GetAuraEffect(105888, EFFECT_1))
            duration += aurEff->GetAmount() * 1000;

    TempSummon* summon = NULL;

    // determine how many units should be summoned
    uint32 numSummons;

    // some spells need to summon many units, for those spells number of summons is stored in effect value
    // however so far noone found a generic check to find all of those (there's no related data in summonproperties.dbc
    // and in spell attributes, possibly we need to add a table for those)
    // so here's a list of MiscValueB values, which is currently most generic check
    switch (properties->Id)
    {
        case 61:
        case 64:
        case 66:
        case 181:
        case 629:
        case 648:
        case 715:
        case 833:
        case 2301:
        case 1061:
        case 1101:
        case 1161:
        case 1261:
        case 1562:
        case 2929: // Summon Unbound Flamesparks, Flameseer's Staff
        case 3097:// Force of Nature
        case 3245:
            numSummons = (damage > 0) ? damage : 1;
            break;
        default:
            numSummons = 1;
            break;
    }

    switch (properties->Category)
    {
        case SUMMON_CATEGORY_WILD:
        case SUMMON_CATEGORY_ALLY:
        case SUMMON_CATEGORY_UNK:
            if ((properties->Flags & 512) || m_spellInfo->Id == 114192 || m_spellInfo->Id == 114203 || m_spellInfo->Id == 114207)
            {
                SummonGuardian(effIndex, entry, properties, numSummons);
                break;
            }
            switch (properties->Type)
            {
                case SUMMON_TYPE_PET:
                case SUMMON_TYPE_GUARDIAN:
                case SUMMON_TYPE_GUARDIAN2:
                case SUMMON_TYPE_MINION:
                    SummonGuardian(effIndex, entry, properties, numSummons);
                    break;
                    // Summons a vehicle, but doesn't force anyone to enter it (see SUMMON_CATEGORY_VEHICLE)
                case SUMMON_TYPE_VEHICLE:
                case SUMMON_TYPE_VEHICLE2:
                    summon = m_caster->GetMap()->SummonCreature(entry, *destTarget, properties, duration, m_originalCaster, m_spellInfo->Id);
                    break;
                case SUMMON_TYPE_TOTEM:
                {
                    summon = m_caster->GetMap()->SummonCreature(entry, *destTarget, properties, duration, m_originalCaster, m_spellInfo->Id);
                    if (!summon || !summon->isTotem())
                        return;

                    // Mana Tide Totem
                    if (m_spellInfo->Id == 16190)
                        damage = m_caster->CountPctFromMaxHealth(10);

                    if (damage)                                            // if not spell info, DB values used
                    {
                        summon->SetMaxHealth(damage);
                        summon->SetHealth(damage);
                    }
                    break;
                }
                case SUMMON_TYPE_MINIPET:
                {
                    summon = m_caster->GetMap()->SummonCreature(entry, *destTarget, properties, duration, m_originalCaster, m_spellInfo->Id);
                    if (!summon || !summon->HasUnitTypeMask(UNIT_MASK_MINION))
                        return;

                    summon->SelectLevel(summon->GetCreatureTemplate());       // some summoned creaters have different from 1 DB data for level/hp
                    summon->SetUInt32Value(UNIT_FIELD_NPC_FLAGS, summon->GetCreatureTemplate()->NpcFlags1);
                    summon->SetUInt32Value(UNIT_FIELD_NPC_FLAGS + 1, summon->GetCreatureTemplate()->NpcFlags2);

                    summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);

                    summon->AI()->EnterEvadeMode();
                    break;
                }
                default:
                {
                    float radius = m_spellInfo->Effects[effIndex].CalcRadius();

                    TempSummonType summonType = (duration == 0) ? TEMPSUMMON_DEAD_DESPAWN : TEMPSUMMON_TIMED_DESPAWN;

                    for (uint32 count = 0; count < numSummons; ++count)
                    {
                        Position pos;
                        if (count == 0)
                            pos = *destTarget;
                        else
                            // randomize position for multiple summons
                            m_caster->GetRandomPoint(*destTarget, radius, pos);

                        summon = m_originalCaster->SummonCreature(entry, *destTarget, summonType, duration);
                        if (!summon)
                            continue;

                        switch (properties->Id)
                        {
                        case 3347: // Orphelins
                        {
                            if (uint32 slot = properties->Slot)
                            {
                                if (m_caster->m_SummonSlot[slot] && m_caster->m_SummonSlot[slot] != summon->GetGUID())
                                {
                                    Creature* oldSummon = m_caster->GetMap()->GetCreature(m_caster->m_SummonSlot[slot]);
                                    if (oldSummon && oldSummon->isSummon())
                                        oldSummon->ToTempSummon()->UnSummon();
                                }
                                m_caster->m_SummonSlot[slot] = summon->GetGUID();
                            }
                        }
                        default:
                            break;
                        }

                        if (properties->Category == SUMMON_CATEGORY_ALLY)
                        {
                            summon->SetOwnerGUID(m_originalCaster->GetGUID());
                            summon->setFaction(m_originalCaster->getFaction());
                            summon->SetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL, m_spellInfo->Id);
                        }

                        // Explosive Decoy and Explosive Decoy 2.0
                        if (m_spellInfo->Id == 54359 || m_spellInfo->Id == 62405)
                        {
                            summon->SetMaxHealth(damage);
                            summon->SetHealth(damage);
                            summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                        }

                        // Wild Mushroom : Plague
                        if (summon && m_spellInfo->Id == 113516)
                            m_originalCaster->CastSpell(m_originalCaster, 113517, true); // Wild Mushroom : Plague (periodic dummy)

                        ExecuteLogEffectSummonObject(effIndex, summon);
                    }
                    return;
                }
            }//switch
            break;
        case SUMMON_CATEGORY_PET:
            SummonGuardian(effIndex, entry, properties, numSummons);
            break;
        case SUMMON_CATEGORY_PUPPET:
            summon = m_caster->GetMap()->SummonCreature(entry, *destTarget, properties, duration, m_originalCaster, m_spellInfo->Id);
            break;
        case SUMMON_CATEGORY_VEHICLE:
            // Summoning spells (usually triggered by npc_spellclick) that spawn a vehicle and that cause the clicker
            // to cast a ride vehicle spell on the summoned unit.
            float x, y, z;
            m_caster->GetClosePoint(x, y, z, DEFAULT_WORLD_OBJECT_SIZE);
            summon = m_originalCaster->GetMap()->SummonCreature(entry, *destTarget, properties, duration, m_caster, m_spellInfo->Id);
            if (!summon || !summon->IsVehicle())
                return;

            // The spell that this effect will trigger. It has SPELL_AURA_CONTROL_VEHICLE
            uint32 spellId = VEHICLE_SPELL_RIDE_HARDCODED;
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(m_spellInfo->Effects[effIndex].CalcValue());
            if (spellInfo && spellInfo->HasAura(SPELL_AURA_CONTROL_VEHICLE))
                spellId = spellInfo->Id;

            // Hard coded enter vehicle spell
            m_originalCaster->CastSpell(summon, spellId, true);

            uint32 faction = properties->Faction;
            if (!faction)
                faction = m_originalCaster->getFaction();

            summon->setFaction(faction);
            break;
    }

    /// Ring of frost
    if (m_spellInfo->Id == 113724 && summon != nullptr)
        summon->SetReactState(REACT_PASSIVE);

    if (summon)
    {
        summon->SetCreatorGUID(m_originalCaster->GetGUID());
        ExecuteLogEffectSummonObject(effIndex, summon);
    }
}

void Spell::EffectLearnSpell(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
    {
        if (unitTarget->ToPet())
            EffectLearnPetSpell(effIndex);
        return;
    }

    Player* player = unitTarget->ToPlayer();

    uint32 spellToLearn = (m_spellInfo->Id == 483 || m_spellInfo->Id == 55884) ? damage : m_spellInfo->Effects[effIndex].TriggerSpell;
    player->learnSpell(spellToLearn, false);
}

typedef std::list< std::pair<uint32, uint64> > DispelList;

void Spell::EffectDispel(SpellEffIndex p_EffectIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    /// Create dispel mask by dispel type
    uint32 l_DispelType = m_spellInfo->Effects[p_EffectIndex].MiscValue;
    uint32 l_DispelMask = SpellInfo::GetDispelMask(DispelType(l_DispelType));

    /// Mass Dispel invisibility removal
    if (m_spellInfo->Id == 32592)
    {
        DispelChargesList l_InvDispelList;
        unitTarget->GetDispellableAuraList(m_caster, 1 << DISPEL_INVISIBILITY, l_InvDispelList);

        /// will not call scripted dispel hook
        for (DispelChargesList::iterator l_It = l_InvDispelList.begin(); l_It != l_InvDispelList.end(); ++l_It)
        {
            if (AuraPtr l_Aura = l_It->first)
                l_Aura->Remove(AURA_REMOVE_BY_ENEMY_SPELL);
        }
    }

    DispelChargesList l_DispelList;
    unitTarget->GetDispellableAuraList(m_caster, l_DispelMask, l_DispelList);

    if (l_DispelList.empty())
        return;

    /// dispel N = damage buffs (or while exist buffs for dispel)
    DispelChargesList l_SuccessList;

    uint64 l_CasterGUID = m_caster->GetGUID();
    uint64 l_VictimGUID = unitTarget->GetGUID();

    uint32 l_DispelSpellID = m_spellInfo->Id;

    std::vector<uint32> l_FailedSpells;

    int32 l_Count;
    for (l_Count = 0; l_Count < damage && !l_DispelList.empty();)
    {
        /// Random select buff for dispel
        DispelChargesList::iterator l_DispelChargeIT = l_DispelList.begin();
        std::advance(l_DispelChargeIT, urand(0, l_DispelList.size() - 1));

        int32 l_Chance = l_DispelChargeIT->first->CalcDispelChance(unitTarget, !unitTarget->IsFriendlyTo(m_caster));

        /// 2.4.3 Patch Notes: "Dispel effects will no longer attempt to remove effects that have 100% dispel resistance."
        if (!l_Chance)
        {
            l_DispelList.erase(l_DispelChargeIT);
            continue;
        }
        else
        {
            if (roll_chance_i(l_Chance))
            {
                bool l_AlreadyListed = false;

                for (DispelChargesList::iterator l_SuccessItr = l_SuccessList.begin(); l_SuccessItr != l_SuccessList.end(); ++l_SuccessItr)
                {
                    if (l_SuccessItr->first->GetId() == l_DispelChargeIT->first->GetId())
                    {
                        ++l_SuccessItr->second;
                        l_AlreadyListed = true;
                    }
                }

                if (!l_AlreadyListed)
                    l_SuccessList.push_back(std::make_pair(l_DispelChargeIT->first, 1));

                --l_DispelChargeIT->second;

                if (l_DispelChargeIT->second <= 0)
                    l_DispelList.erase(l_DispelChargeIT);
            }
            else
            {
                /// Append failed spell id
                l_FailedSpells.push_back(l_DispelChargeIT->first->GetId());
            }

            ++l_Count;
        }
    }

    if (!l_FailedSpells.empty())
    {
        WorldPacket l_SpellFailedPacket(SMSG_DISPEL_FAILED, 2 * (16 + 2) + 4 + (4 * l_FailedSpells.size()));
        l_SpellFailedPacket.appendPackGUID(l_CasterGUID);
        l_SpellFailedPacket.appendPackGUID(l_VictimGUID);
        l_SpellFailedPacket << uint32(l_DispelSpellID);
        l_SpellFailedPacket << uint32(l_FailedSpells.size());

        for (uint32 l_I = 0; l_I < l_FailedSpells.size(); ++l_I)
            l_SpellFailedPacket << uint32(l_FailedSpells[l_I]);

        m_caster->SendMessageToSet(&l_SpellFailedPacket, true);
    }

    if (l_SuccessList.empty())
        return;

    // Custom effects
    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        // Glyph of Dispel Magic (discipline and priest) -- Purify
        if (m_spellInfo->Id == 97960)
        {
            if (AuraEffectPtr aurEff = m_caster->GetAuraEffect(55677, 0))
            {
                if (m_caster->IsFriendlyTo(unitTarget))
                {
                    int32 bp = int32(m_caster->CountPctFromMaxHealth(aurEff->GetAmount() * l_Count));
                    m_caster->CastCustomSpell(unitTarget, 56131, &bp, 0, 0, true);
                }
            }
        }
        // Glyph of Dispel Magic
        if (m_spellInfo->Id == 528)
        {
            if (AuraEffectPtr aurEff = m_caster->GetAuraEffect(119864, 0))
                m_caster->CastSpell(unitTarget, 119856, true);
        }
    }

    WorldPacket dataSuccess(SMSG_SPELL_DISPELL_LOG, 8 + 8 + 4 + 1 + 4 + l_SuccessList.size() * 5);
    // Send packet header
    dataSuccess.append(unitTarget->GetPackGUID());         // Victim GUID
    dataSuccess.append(m_caster->GetPackGUID());           // Caster GUID
    dataSuccess << uint32(m_spellInfo->Id);                // dispel spell id
    dataSuccess << uint8(0);                               // not used
    dataSuccess << uint32(l_SuccessList.size());            // count

    for (DispelChargesList::iterator itr = l_SuccessList.begin(); itr != l_SuccessList.end(); ++itr)
    {
        // Send dispelled spell info
        dataSuccess << uint32(itr->first->GetId());              // Spell Id
        dataSuccess << uint8(0);                        // 0 - dispelled !=0 cleansed
        unitTarget->RemoveAurasDueToSpellByDispel(itr->first->GetId(), m_spellInfo->Id, itr->first->GetCasterGUID(), m_caster, itr->second);
    }

    m_caster->SendMessageToSet(&dataSuccess, true);

    // On success dispel
    switch (m_spellInfo->Id)
    {
        // Purify
    case 527:
        // Glyph of Purify
        if (m_caster->HasAura(55677))
            m_caster->CastSpell(unitTarget, 56131, true);

        break;

        // Devour Magic
    case 19505:
    {
        int32 l_HealAmount = m_spellInfo->Effects[EFFECT_1].CalcValue(m_caster);
        m_caster->CastCustomSpell(m_caster, 19658, &l_HealAmount, NULL, NULL, true);

        // Glyph of Felhunter
        if (Unit* l_Owner = m_caster->GetOwner())
            if (l_Owner->GetAura(56249))
                l_Owner->CastCustomSpell(l_Owner, 19658, &l_HealAmount, NULL, NULL, true);

        break;
    }

    // Cleanse Spirit
    case 51886:
        // Purify Spirit
    case 77130:
        if (m_caster->HasAura(86959)) // Glyph of Cleansing Waters
            m_caster->CastSpell(unitTarget, 86961, true);

        break;

        // Remove Curse
    case 475:
        if (m_caster->HasAura(115700))
            m_caster->AddAura(115701, m_caster);

        break;

    default:
        break;
    }
}

void Spell::EffectDualWield(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    // Mistweaver monks cannot dual wield
    if (unitTarget->ToPlayer())
        if (unitTarget->ToPlayer()->GetSpecializationId(unitTarget->ToPlayer()->GetActiveSpec()) == SPEC_MONK_MISTWEAVER)
            return;

    unitTarget->SetCanDualWield(true);

    if (unitTarget->GetTypeId() == TYPEID_UNIT)
        unitTarget->ToCreature()->UpdateDamagePhysical(WeaponAttackType::OffAttack);
}

void Spell::EffectPull(SpellEffIndex effIndex)
{
    // TODO: create a proper pull towards distract spell center for distract
    EffectNULL(effIndex);
}

void Spell::EffectDistract(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    // Check for possible target
    if (!unitTarget || unitTarget->isInCombat())
        return;

    // target must be OK to do this
    if (unitTarget->HasUnitState(UNIT_STATE_CONFUSED | UNIT_STATE_STUNNED | UNIT_STATE_FLEEING))
        return;

    unitTarget->SetFacingTo(unitTarget->GetAngle(destTarget));
    unitTarget->ClearUnitState(UNIT_STATE_MOVING);

    if (unitTarget->GetTypeId() == TYPEID_UNIT)
        unitTarget->GetMotionMaster()->MoveDistract(damage * IN_MILLISECONDS);
}

void Spell::EffectPickPocket(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    // victim must be creature and attackable
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT || m_caster->IsFriendlyTo(unitTarget))
        return;

    // victim have to be alive and humanoid or undead
    if (unitTarget->isAlive() && (unitTarget->GetCreatureTypeMask() & CREATURE_TYPEMASK_HUMANOID_OR_UNDEAD) != 0)
    {
        if (m_caster->HasAura(63268)) // Glyph of Disguise
            unitTarget->AddAura(121308, m_caster);

        m_caster->ToPlayer()->SendLoot(unitTarget->GetGUID(), LOOT_PICKPOCKETING);
    }
}

void Spell::EffectAddFarsight(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    float radius = m_spellInfo->Effects[effIndex].CalcRadius();
    int32 duration = m_spellInfo->GetDuration();
    // Caster not in world, might be spell triggered from aura removal
    if (!m_caster->IsInWorld())
        return;

    DynamicObject* dynObj = new DynamicObject(true);
    if (!dynObj->CreateDynamicObject(sObjectMgr->GenerateLowGuid(HIGHGUID_DYNAMICOBJECT), m_caster, m_spellInfo, *destTarget, radius, DYNAMIC_OBJECT_FARSIGHT_FOCUS))
    {
        delete dynObj;
        return;
    }

    dynObj->SetDuration(duration);
    dynObj->SetCasterViewpoint();
}

void Spell::EffectUntrainTalents(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || m_caster->GetTypeId() == TYPEID_PLAYER)
        return;

    if (uint64 guid = m_caster->GetGUID()) // the trainer is the caster
        unitTarget->ToPlayer()->SendTalentWipeConfirm(guid, false);
}

void Spell::EffectTeleUnitsFaceCaster(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    if (unitTarget->isInFlight())
        return;

    float dis = m_spellInfo->Effects[effIndex].CalcRadius(m_caster);

    float fx, fy, fz;
    m_caster->GetClosePoint(fx, fy, fz, unitTarget->GetObjectSize(), dis);

    unitTarget->NearTeleportTo(fx, fy, fz, -m_caster->GetOrientation(), unitTarget == m_caster);
}

void Spell::EffectLearnSkill(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if (damage < 0)
        return;

    uint32 skillid = m_spellInfo->Effects[effIndex].MiscValue;
    uint16 skillval = unitTarget->ToPlayer()->GetPureSkillValue(skillid);

    uint32 l_Multiplier  = damage;
    uint32 l_NewMaxValue = 0;

    /// Since WoD palier 9 give 100 points not 75
    if (l_Multiplier < 9)
        l_NewMaxValue = l_Multiplier * 75;
    else
        l_NewMaxValue = ((l_Multiplier - 8) * 100) + (8 * 75);

    unitTarget->ToPlayer()->SetSkill(skillid, m_spellInfo->Effects[effIndex].CalcValue(), skillval ? skillval : 1, l_NewMaxValue);

    // Archaeology
    if (skillid == SKILL_ARCHAEOLOGY)
    {
        unitTarget->ToPlayer()->GetArchaeologyMgr().GenerateResearchSites();
        unitTarget->ToPlayer()->GetArchaeologyMgr().GenerateResearchProjects();
    }
}

void Spell::EffectPlayMovie(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    uint32 movieId = GetSpellInfo()->Effects[effIndex].MiscValue;
    if (!sMovieStore.LookupEntry(movieId))
        return;

    unitTarget->ToPlayer()->SendMovieStart(movieId);
}

void Spell::EffectTradeSkill(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;
    // uint32 skillid =  m_spellInfo->Effects[i].MiscValue;
    // uint16 skillmax = unitTarget->ToPlayer()->(skillid);
    // m_caster->ToPlayer()->SetSkill(skillid, skillval?skillval:1, skillmax+75);
}

void Spell::EffectEnchantItemPerm(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;
    if (!itemTarget)
        return;

    Player* p_caster = (Player*)m_caster;

    // Handle vellums
    if (itemTarget->IsVellum())
    {
        // destroy one vellum from stack
        uint32 count = 1;
        p_caster->DestroyItemCount(itemTarget, count, true);
        unitTarget = p_caster;
        // and add a scroll
        DoCreateItem(effIndex, m_spellInfo->Effects[effIndex].ItemType, true);
        itemTarget = NULL;
        m_targets.SetItemTarget(NULL);
    }
    else
    {
        // do not increase skill if vellum used
        if (!(m_CastItem && m_CastItem->GetTemplate()->Flags & ITEM_PROTO_FLAG_TRIGGERED_CAST))
            p_caster->UpdateCraftSkill(m_spellInfo->Id);

        uint32 enchant_id = m_spellInfo->Effects[effIndex].MiscValue;
        if (!enchant_id)
            return;

        SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
        if (!pEnchant)
            return;

        // item can be in trade slot and have owner diff. from caster
        Player* item_owner = itemTarget->GetOwner();
        if (!item_owner)
            return;

        /*if (item_owner != p_caster && !AccountMgr::IsPlayerAccount(p_caster->GetSession()->GetSecurity()) && sWorld->getBoolConfig(CONFIG_GM_LOG_TRADE))
        {
        sLog->outCommand(p_caster->GetSession()->GetAccountId(), "GM %s (Account: %u) enchanting(perm): %s (Entry: %d) for player: %s (Account: %u)",
        p_caster->GetName(), p_caster->GetSession()->GetAccountId(),
        itemTarget->GetTemplate()->Name1.c_str(), itemTarget->GetEntry(),
        item_owner->GetName(), item_owner->GetSession()->GetAccountId());
        }*/

        // remove old enchanting before applying new if equipped
        item_owner->ApplyEnchantment(itemTarget, PERM_ENCHANTMENT_SLOT, false);

        itemTarget->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchant_id, 0, 0);

        // add new enchanting if equipped
        item_owner->ApplyEnchantment(itemTarget, PERM_ENCHANTMENT_SLOT, true);

        item_owner->RemoveTradeableItem(itemTarget);
        itemTarget->ClearSoulboundTradeable(item_owner);
    }
}

void Spell::EffectEnchantItemPrismatic(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;
    if (!itemTarget)
        return;

    Player* p_caster = (Player*)m_caster;

    uint32 enchant_id = m_spellInfo->Effects[effIndex].MiscValue;
    if (!enchant_id)
        return;

    SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
    if (!pEnchant)
        return;

    // support only enchantings with add socket in this slot
    {
        bool add_socket = false;
        for (uint8 i = 0; i < MAX_ENCHANTMENT_SPELLS; ++i)
        {
            if (pEnchant->type[i] == ITEM_ENCHANTMENT_TYPE_PRISMATIC_SOCKET)
            {
                add_socket = true;
                break;
            }
        }
        if (!add_socket)
        {
            sLog->outError(LOG_FILTER_SPELLS_AURAS, "Spell::EffectEnchantItemPrismatic: attempt apply enchant spell %u with SPELL_EFFECT_ENCHANT_ITEM_PRISMATIC (%u) but without ITEM_ENCHANTMENT_TYPE_PRISMATIC_SOCKET (%u), not suppoted yet.",
                m_spellInfo->Id, SPELL_EFFECT_ENCHANT_ITEM_PRISMATIC, ITEM_ENCHANTMENT_TYPE_PRISMATIC_SOCKET);
            return;
        }
    }

    // item can be in trade slot and have owner diff. from caster
    Player* item_owner = itemTarget->GetOwner();
    if (!item_owner)
        return;

    // remove old enchanting before applying new if equipped
    item_owner->ApplyEnchantment(itemTarget, PRISMATIC_ENCHANTMENT_SLOT, false);

    itemTarget->SetEnchantment(PRISMATIC_ENCHANTMENT_SLOT, enchant_id, 0, 0);

    // add new enchanting if equipped
    item_owner->ApplyEnchantment(itemTarget, PRISMATIC_ENCHANTMENT_SLOT, true);

    item_owner->RemoveTradeableItem(itemTarget);
    itemTarget->ClearSoulboundTradeable(item_owner);
}

void Spell::EffectEnchantItemTmp(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* p_caster = (Player*)m_caster;

    if (!itemTarget)
        return;

    uint32 enchant_id = m_spellInfo->Effects[effIndex].MiscValue;

    if (!enchant_id)
    {
        sLog->outError(LOG_FILTER_SPELLS_AURAS, "Spell %u Effect %u (SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY) have 0 as enchanting id", m_spellInfo->Id, effIndex);
        return;
    }

    SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
    if (!pEnchant)
    {
        sLog->outError(LOG_FILTER_SPELLS_AURAS, "Spell %u Effect %u (SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY) have not existed enchanting id %u ", m_spellInfo->Id, effIndex, enchant_id);
        return;
    }

    // select enchantment duration
    uint32 duration;

    // rogue family enchantments exception by duration
    if (m_spellInfo->Id == 38615)
        duration = 1800;                                    // 30 mins
    // other rogue family enchantments always 1 hour (some have spell damage=0, but some have wrong data in EffBasePoints)
    else if (m_spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE)
        duration = 3600;                                    // 1 hour
    // shaman family enchantments
    else if (m_spellInfo->SpellFamilyName == SPELLFAMILY_SHAMAN)
        duration = 3600;                                    // 1 hour
    // other cases with this SpellVisual already selected
    else if (m_spellInfo->SpellVisual[0] == 215)
        duration = 1800;                                    // 30 mins
    // some fishing pole bonuses except Glow Worm which lasts full hour
    else if (m_spellInfo->SpellVisual[0] == 563 && m_spellInfo->Id != 64401)
        duration = 600;                                     // 10 mins
    // shaman rockbiter enchantments
    else if (m_spellInfo->SpellVisual[0] == 0)
        duration = 1800;                                    // 30 mins
    else if (m_spellInfo->Id == 29702)
        duration = 300;                                     // 5 mins
    else if (m_spellInfo->Id == 37360)
        duration = 300;                                     // 5 mins
    // default case
    else
        duration = 3600;                                    // 1 hour

    // item can be in trade slot and have owner diff. from caster
    Player* item_owner = itemTarget->GetOwner();
    if (!item_owner)
        return;

    // remove old enchanting before applying new if equipped
    item_owner->ApplyEnchantment(itemTarget, TEMP_ENCHANTMENT_SLOT, false);

    itemTarget->SetEnchantment(TEMP_ENCHANTMENT_SLOT, enchant_id, duration * 1000, 0);

    // add new enchanting if equipped
    item_owner->ApplyEnchantment(itemTarget, TEMP_ENCHANTMENT_SLOT, true);
}

void Spell::EffectTameCreature(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    if (unitTarget->GetTypeId() != TYPEID_UNIT)
        return;

    Creature* creatureTarget = unitTarget->ToCreature();

    if (creatureTarget->isPet())
        return;

    if (m_caster->getClass() != CLASS_HUNTER)
        return;

    // cast finish successfully
    //SendChannelUpdate(0);
    finish();

    Pet* pet = m_caster->CreateTamedPetFrom(creatureTarget, m_spellInfo->Id);
    if (!pet)                                               // in very specific state like near world end/etc.
        return;

    // "kill" original creature
    creatureTarget->DespawnOrUnsummon();

    uint8 level = m_caster->getLevel();

    // prepare visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL, level - 1);

    // add to world
    pet->GetMap()->AddToMap(pet->ToCreature());

    // visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL, level);

    // caster have pet now
    m_caster->SetMinion(pet, true, m_caster->ToPlayer()->getSlotForNewPet(), pet->m_Stampeded);

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        pet->SavePetToDB(PET_SLOT_ACTUAL_PET_SLOT, pet->m_Stampeded);
        m_caster->ToPlayer()->PetSpellInitialize();
        m_caster->ToPlayer()->GetSession()->SendStablePet(0);
    }
}

void Spell::EffectSummonPet(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    Player* owner = NULL;
    if (m_originalCaster)
    {
        owner = m_originalCaster->ToPlayer();
        if (!owner && m_originalCaster->ToCreature()->isTotem())
            owner = m_originalCaster->GetCharmerOrOwnerPlayerOrPlayerItself();
    }

    uint32 petentry = m_spellInfo->Effects[effIndex].MiscValue;

    PetSlot slot = PetSlot(m_spellInfo->Effects[effIndex].BasePoints);
    if (petentry)
        slot = PET_SLOT_UNK_SLOT;

    if (!owner)
    {
        SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(67);
        if (properties)
            SummonGuardian(effIndex, petentry, properties, 1);
        return;
    }

    Pet* OldSummon = owner->GetPet();

    // if pet requested type already exist
    if (OldSummon)
    {
        if (petentry == 0 || OldSummon->GetEntry() == petentry)
        {
            // pet in corpse state can't be summoned
            if (OldSummon->isDead())
                return;

            ASSERT(OldSummon->GetMap() == owner->GetMap());

            //OldSummon->GetMap()->Remove(OldSummon->ToCreature(), false);

            float px, py, pz;
            owner->GetClosePoint(px, py, pz, OldSummon->GetObjectSize());

            OldSummon->NearTeleportTo(px, py, pz, OldSummon->GetOrientation());
            //OldSummon->Relocate(px, py, pz, OldSummon->GetOrientation());
            //OldSummon->SetMap(owner->GetMap());
            //owner->GetMap()->Add(OldSummon->ToCreature());

            if (owner->GetTypeId() == TYPEID_PLAYER && OldSummon->isControlled())
                owner->ToPlayer()->PetSpellInitialize();

            return;
        }

        if (owner->GetTypeId() == TYPEID_PLAYER)
            owner->ToPlayer()->RemovePet(OldSummon, (OldSummon->getPetType() == HUNTER_PET ? PET_SLOT_DELETED : PET_SLOT_OTHER_PET), false, OldSummon->m_Stampeded);
        else
            return;
    }

    float x, y, z;
    owner->GetClosePoint(x, y, z, owner->GetObjectSize());
    Pet* pet = owner->SummonPet(petentry, x, y, z, owner->GetOrientation(), SUMMON_PET, 0, PetSlot(slot));
    if (!pet)
        return;

    if (m_caster->GetTypeId() == TYPEID_UNIT)
    {
        if (m_caster->ToCreature()->isTotem())
            pet->SetReactState(REACT_AGGRESSIVE);
        else
            pet->SetReactState(REACT_DEFENSIVE);
    }

    pet->SetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL, m_spellInfo->Id);

    if (m_caster->GetTypeId() == TYPEID_UNIT)
    {
        if (m_caster->ToCreature()->isTotem())
        {
            if (pet->GetEntry() == 61029 || pet->GetEntry() == 61056)
            {
                if (Unit* owner = m_caster->GetOwner())
                {
                    if (Player* _plr = owner->ToPlayer())
                    {
                        if (pet->GetEntry() == 61029)
                        {
                            pet->addSpell(118297);  // Immolate
                            pet->addSpell(118350);  // Empower
                        }
                        else
                        {
                            pet->addSpell(118337);  // Harden Skin
                            pet->addSpell(118345);  // Pulverize
                            pet->addSpell(118347);  // Reinforce
                        }

                        _plr->PetSpellInitialize();
                    }
                }
            }
        }
    }

    // generate new name for summon pet
    if (petentry)
    {
        std::string new_name = sObjectMgr->GeneratePetName(petentry);
        if (!new_name.empty())
            pet->SetName(new_name);
    }

    ExecuteLogEffectSummonObject(effIndex, pet);
}

void Spell::EffectLearnPetSpell(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    if (unitTarget->ToPlayer())
    {
        EffectLearnSpell(effIndex);
        return;
    }
    Pet* pet = unitTarget->ToPet();
    if (!pet)
        return;

    SpellInfo const* learn_spellproto = sSpellMgr->GetSpellInfo(m_spellInfo->Effects[effIndex].TriggerSpell);
    if (!learn_spellproto)
        return;

    pet->learnSpell(learn_spellproto->Id);
    pet->SavePetToDB(PET_SLOT_ACTUAL_PET_SLOT, pet->m_Stampeded);
    pet->GetOwner()->PetSpellInitialize();
}

void Spell::EffectTaunt(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    if (m_spellInfo->Id == 49560)
        if (m_caster->HasAura(63335))
            return;

    // this effect use before aura Taunt apply for prevent taunt already attacking target
    // for spell as marked "non effective at already attacking target"
    if (!unitTarget || !unitTarget->CanHaveThreatList()
        || unitTarget->getVictim() == m_caster)
    {
        SendCastResult(SPELL_FAILED_DONT_REPORT);
        return;
    }

    // Also use this effect to set the taunter's threat to the taunted creature's highest value
    if (unitTarget->getThreatManager().getCurrentVictim())
    {
        float myThreat = unitTarget->getThreatManager().getThreat(m_caster);
        float itsThreat = unitTarget->getThreatManager().getCurrentVictim()->getThreat();
        if (itsThreat > myThreat)
            unitTarget->getThreatManager().addThreat(m_caster, itsThreat - myThreat);
    }

    //Set aggro victim to caster
    if (!unitTarget->getThreatManager().getOnlineContainer().empty())
        if (HostileReference* forcedVictim = unitTarget->getThreatManager().getOnlineContainer().getReferenceByTarget(m_caster))
            unitTarget->getThreatManager().setCurrentVictim(forcedVictim);

    if (unitTarget->GetTypeId() == TYPEID_UNIT && unitTarget->ToCreature()->IsAIEnabled
        && (!unitTarget->ToCreature()->HasReactState(REACT_PASSIVE) || (unitTarget->IsPetGuardianStuff() && IS_PLAYER_GUID(unitTarget->GetCharmerOrOwnerGUID()))))
    {
        // taken from case COMMAND_ATTACK:                        //spellid=1792  //ATTACK PetHandler.cpp
        if (CharmInfo* charmInfo = unitTarget->GetCharmInfo())
        {
            unitTarget->AttackStop();
            charmInfo->SetIsCommandAttack(true);
            charmInfo->SetIsAtStay(false);
            charmInfo->SetIsFollowing(false);
            charmInfo->SetIsReturning(false);
        }
        unitTarget->ToCreature()->AI()->AttackStart(m_caster);
    }
}

void Spell::EffectWeaponDmg(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH_TARGET)
        return;

    if (!unitTarget || !unitTarget->isAlive())
        return;

    // multiple weapon dmg effect workaround
    // execute only the last weapon damage
    // and handle all effects at once
    for (uint32 j = effIndex + 1; j < MAX_SPELL_EFFECTS; ++j)
    {
        switch (m_spellInfo->Effects[j].Effect)
        {
        case SPELL_EFFECT_WEAPON_DAMAGE:
        case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
        case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
        case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
            return;     // we must calculate only at last weapon effect
            break;
        }
    }

    // some spell specific modifiers
    float totalDamagePercentMod = 1.0f;                    // applied to final bonus+weapon damage
    int32 fixed_bonus = 0;
    int32 spell_bonus = 0;                                  // bonus specific for spell
    float final_bonus = 0;

    switch (m_spellInfo->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (m_spellInfo->Id)
            {
                case 69055: // Saber Lash
                case 70814: // Saber Lash
                {
                    uint32 count = 0;
                    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                        if (ihit->effectMask & (1 << effIndex))
                            ++count;

                    totalDamagePercentMod /= count;
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_ROGUE:
        {
            switch (m_spellInfo->Id)
            {
                case 8676:  // Ambush
                {
                    // 44.7% more damage with daggers
                    if (m_caster->GetTypeId() == TYPEID_PLAYER)
                        if (Item* item = m_caster->ToPlayer()->GetWeaponForAttack(m_attackType, true))
                            if (item->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
                                totalDamagePercentMod *= 1.447f;
                    break;
                }
                case 16511: // Hemorrhage
                {
                    // 40% more damage with daggers
                    if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (Item* l_Item = m_caster->ToPlayer()->GetWeaponForAttack(m_attackType, true))
                        {
                            if (l_Item->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
                                totalDamagePercentMod *= 1.40f;
                        }
                    }
                    break;
                }
                default:
                    break;
            }

            break;
        }
        case SPELLFAMILY_SHAMAN:
        {
            // Skyshatter Harness item set bonus
            // Stormstrike
            if (AuraEffectPtr aurEff = m_caster->IsScriptOverriden(m_spellInfo, 5634))
                m_caster->CastSpell(m_caster, 38430, true, NULL, aurEff);
            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            switch (m_spellInfo->Id)
            {
                case 75:
                {
                    m_caster->Attack(unitTarget, false);
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            switch (m_spellInfo->Id)
            {
                case 52374: // Blood Strike
                {
                    float bonusPct = ((m_spellInfo->Effects[EFFECT_0].BasePoints * m_spellInfo->Effects[EFFECT_1].BasePoints) / 100) * unitTarget->GetDiseasesByCaster(m_caster->GetGUID());
            
                    AddPct(totalDamagePercentMod, bonusPct);
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }

    bool normalized = false;
    float weaponDamagePercentMod = 1.0f;
    for (int j = 0; j < MAX_SPELL_EFFECTS; ++j)
    {
        switch (m_spellInfo->Effects[j].Effect)
        {
            case SPELL_EFFECT_WEAPON_DAMAGE:
            case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
                fixed_bonus += CalculateDamage(j, unitTarget);
                break;
            case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
                fixed_bonus += CalculateDamage(j, unitTarget);
                normalized = true;
                break;
            case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
                ApplyPct(weaponDamagePercentMod, CalculateDamage(j, unitTarget));
                break;
            default:
                break;                                      // not weapon damage effect, just skip
        }
    }

    // apply to non-weapon bonus weapon total pct effect, weapon total flat effect included in weapon damage
    if (fixed_bonus || spell_bonus)
    {
        UnitMods unitMod;
        switch (m_attackType)
        {
            case WeaponAttackType::OffAttack:
                unitMod = UNIT_MOD_DAMAGE_OFFHAND;
                break;
            case WeaponAttackType::RangedAttack:
                unitMod = UNIT_MOD_DAMAGE_RANGED;
                break;
            case WeaponAttackType::BaseAttack:
            default:
                unitMod = UNIT_MOD_DAMAGE_MAINHAND;
                break;
        }

        float weapon_total_pct = 1.0f;
        if (m_spellInfo->SchoolMask & SPELL_SCHOOL_MASK_NORMAL)
            weapon_total_pct = m_caster->GetModifierValue(unitMod, TOTAL_PCT);

        if (fixed_bonus)
            fixed_bonus = int32(fixed_bonus * weapon_total_pct);
        if (spell_bonus)
            spell_bonus = int32(spell_bonus * weapon_total_pct);
    }

    int32 weaponDamage = m_caster->CalculateDamage(m_attackType, normalized, true);
    int32 autoAttacksBonus = std::max(1 + (m_caster->GetTotalAuraModifier(SPELL_AURA_MOD_AUTOATTACK_DAMAGE) / 100), 1);
    weaponDamage /= autoAttacksBonus;

    // Sequence is important
    for (int j = 0; j < MAX_SPELL_EFFECTS; ++j)
    {
        // We assume that a spell have at most one fixed_bonus
        // and at most one weaponDamagePercentMod
        switch (m_spellInfo->Effects[j].Effect)
        {
            case SPELL_EFFECT_WEAPON_DAMAGE:
            case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
            case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
                weaponDamage += fixed_bonus;
                break;
            case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
                weaponDamage = int32(weaponDamage* weaponDamagePercentMod);
            default:
                break;                                      // not weapon damage effect, just skip
        }
    }

    if (spell_bonus)
        weaponDamage += spell_bonus;

    if (final_bonus)
        weaponDamage *= final_bonus;

    if (totalDamagePercentMod != 1.0f)
        weaponDamage = int32(weaponDamage* totalDamagePercentMod);

    // prevent negative damage
    uint32 eff_damage(std::max(weaponDamage, 0));

    // Add melee damage bonuses (also check for negative)
    uint32 damage = m_caster->MeleeDamageBonusDone(unitTarget, eff_damage, m_attackType, m_spellInfo);

    m_damage += unitTarget->MeleeDamageBonusTaken(m_caster, damage, m_attackType, m_spellInfo);

    // Legion Strike
    if (m_spellInfo->Id == 30213)
    {
        uint32 count = 0;
        for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
        {
            if (ihit->effectMask & (1 << effIndex))
                ++count;
        }

        m_damage /= count;
    }
}

void Spell::EffectThreat(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->isAlive() || !m_caster->isAlive())
        return;

    if (!unitTarget->CanHaveThreatList())
        return;

    unitTarget->AddThreat(m_caster, float(damage));
}

void Spell::EffectHealMaxHealth(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->isAlive())
        return;

    int32 addhealth = 0;

    // damage == 0 - heal for caster max health
    if (damage == 0)
        addhealth = m_caster->GetMaxHealth();
    else
        addhealth = unitTarget->GetMaxHealth() - unitTarget->GetHealth();

    m_healing += addhealth;
    m_healing = unitTarget->SpellHealingBonusTaken(m_caster, m_spellInfo, m_healing, HEAL);
}

void Spell::EffectInterruptCast(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->isAlive())
        return;

    /// Deadly Throw - Interrupt spell only if used with 3 combo points
    if (m_spellInfo->Id == 26679)
    {
        if (m_originalCaster && m_originalCaster->GetPower(Powers::POWER_COMBO_POINT) < 5)
            return;
    }

    // TODO: not all spells that used this effect apply cooldown at school spells
    // also exist case: apply cooldown to interrupted cast only and to all spells
    // there is no CURRENT_AUTOREPEAT_SPELL spells that can be interrupted
    for (uint32 i = CURRENT_FIRST_NON_MELEE_SPELL; i < CURRENT_AUTOREPEAT_SPELL; ++i)
    {
        if (Spell* spell = unitTarget->GetCurrentSpell(CurrentSpellTypes(i)))
        {
            SpellInfo const* curSpellInfo = spell->m_spellInfo;
            // check if we can interrupt spell
            if ((spell->getState() == SPELL_STATE_CASTING
                || (spell->getState() == SPELL_STATE_PREPARING && spell->GetCastTime() > 0.0f))
                && (curSpellInfo->PreventionType == SPELL_PREVENTION_TYPE_SILENCE || curSpellInfo->PreventionType == SPELL_PREVENTION_TYPE_UNK1 || curSpellInfo->PreventionType == SPELL_PREVENTION_TYPE_UNK3)
                && ((i == CURRENT_GENERIC_SPELL && curSpellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_INTERRUPT)
                || (i == CURRENT_CHANNELED_SPELL && curSpellInfo->ChannelInterruptFlags & CHANNEL_INTERRUPT_FLAG_INTERRUPT)))
            {
                if (m_originalCaster)
                {
                    // Furious Stone Breath cannot be interrupted except by Shell Concussion
                    if (curSpellInfo->Id == 133939 && m_spellInfo->Id != 134091)
                        continue;

                    int32 duration = m_spellInfo->GetDuration();
                    unitTarget->ProhibitSpellSchool(curSpellInfo->GetSchoolMask(), unitTarget->ModSpellDuration(m_spellInfo, unitTarget, duration, false, 1 << effIndex));

                    WorldPacket interrupt(SMSG_SPELL_INTERRUPT_LOG);
                    ObjectGuid targetGuid = unitTarget->GetGUID();
                    ObjectGuid casterGuid = m_originalCasterGUID;

                    interrupt.WriteBit(casterGuid[5]);
                    interrupt.WriteBit(targetGuid[5]);
                    interrupt.WriteBit(targetGuid[4]);
                    interrupt.WriteBit(casterGuid[2]);
                    interrupt.WriteBit(targetGuid[0]);
                    interrupt.WriteBit(casterGuid[6]);
                    interrupt.WriteBit(0);
                    interrupt.WriteBit(targetGuid[6]);
                    interrupt.WriteBit(casterGuid[4]);
                    interrupt.WriteBit(casterGuid[1]);
                    interrupt.WriteBit(targetGuid[2]);
                    interrupt.WriteBit(casterGuid[0]);
                    interrupt.WriteBit(targetGuid[3]);
                    interrupt.WriteBit(casterGuid[3]);
                    interrupt.WriteBit(targetGuid[1]);
                    interrupt.WriteBit(casterGuid[7]);
                    interrupt.WriteBit(targetGuid[7]);

                    interrupt.WriteByteSeq(casterGuid[0]);
                    interrupt.WriteByteSeq(casterGuid[4]);
                    interrupt.WriteByteSeq(targetGuid[3]);
                    interrupt.WriteByteSeq(casterGuid[3]);
                    interrupt << uint32(m_spellInfo->Id);
                    interrupt.WriteByteSeq(targetGuid[4]);
                    interrupt.WriteByteSeq(casterGuid[1]);
                    interrupt.WriteByteSeq(targetGuid[7]);
                    interrupt.WriteByteSeq(targetGuid[0]);
                    interrupt.WriteByteSeq(casterGuid[5]);
                    interrupt << uint32(curSpellInfo->Id);
                    interrupt.WriteByteSeq(targetGuid[1]);
                    interrupt.WriteByteSeq(targetGuid[5]);
                    interrupt.WriteByteSeq(casterGuid[7]);
                    interrupt.WriteByteSeq(targetGuid[6]);
                    interrupt.WriteByteSeq(casterGuid[6]);
                    interrupt.WriteByteSeq(casterGuid[2]);
                    interrupt.WriteByteSeq(targetGuid[2]);

                    m_originalCaster->SendMessageToSet(&interrupt, true);
                }

                ExecuteLogEffectInterruptCast(effIndex, unitTarget, curSpellInfo->Id);
                unitTarget->InterruptSpell(CurrentSpellTypes(i), false);
            }
        }
    }
}

void Spell::EffectSummonObjectWild(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    uint32 gameobject_id = m_spellInfo->Effects[effIndex].MiscValue;

    GameObject* pGameObj = new GameObject;

    WorldObject* target = focusObject;
    if (!target)
        target = m_caster;

    float x, y, z;
    if (m_targets.HasDst())
        destTarget->GetPosition(x, y, z);
    else
        m_caster->GetClosePoint(x, y, z, DEFAULT_WORLD_OBJECT_SIZE);

    Map* map = target->GetMap();

    if (!pGameObj->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_GAMEOBJECT), gameobject_id, map,
        m_caster->GetPhaseMask(), x, y, z, target->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 100, GO_STATE_READY))
    {
        delete pGameObj;
        return;
    }

    int32 duration = m_spellInfo->GetDuration();

    pGameObj->SetRespawnTime(duration > 0 ? duration / IN_MILLISECONDS : 0);
    pGameObj->SetSpellId(m_spellInfo->Id);

    ExecuteLogEffectSummonObject(effIndex, pGameObj);

    // Wild object not have owner and check clickable by players
    map->AddToMap(pGameObj);

    if (pGameObj->GetGoType() == GAMEOBJECT_TYPE_FLAGDROP && m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        Player* player = m_caster->ToPlayer();
        Battleground* bg = player->GetBattleground();

        switch (pGameObj->GetMapId())
        {
        case 489:                                       //WS
        {
            if (bg && bg->GetTypeID(true) == BATTLEGROUND_WS && bg->GetStatus() == STATUS_IN_PROGRESS)
            {
                uint32 team = ALLIANCE;

                if (player->GetTeam() == team)
                    team = HORDE;

                ((BattlegroundWS*)bg)->SetDroppedFlagGUID(pGameObj->GetGUID(), team);
            }
            break;
        }
        case 566:                                       //EY
        {
            if (bg && (bg->GetTypeID(true) == BATTLEGROUND_EY || bg->GetTypeID(true) == BATTLEGROUND_EYR) && bg->GetStatus() == STATUS_IN_PROGRESS)
                ((BattlegroundEY*)bg)->SetDroppedFlagGUID(pGameObj->GetGUID());

            break;
        }
        case 726:                                       //TP
        {
            if (bg && bg->GetTypeID(true) == BATTLEGROUND_TP && bg->GetStatus() == STATUS_IN_PROGRESS)
            {
                uint32 team = TEAM_ALLIANCE;

                if (player->GetTeamId() == team)
                    team = TEAM_HORDE;

                ((BattlegroundTP*)bg)->SetDroppedFlagGUID(pGameObj->GetGUID(), team);
            }
            break;
        }
        }
    }

    if (uint32 linkedEntry = pGameObj->GetGOInfo()->GetLinkedGameObjectEntry())
    {
        GameObject* linkedGO = new GameObject;
        if (linkedGO->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_GAMEOBJECT), linkedEntry, map,
            m_caster->GetPhaseMask(), x, y, z, target->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 100, GO_STATE_READY))
        {
            linkedGO->SetRespawnTime(duration > 0 ? duration / IN_MILLISECONDS : 0);
            linkedGO->SetSpellId(m_spellInfo->Id);

            ExecuteLogEffectSummonObject(effIndex, linkedGO);

            // Wild object not have owner and check clickable by players
            map->AddToMap(linkedGO);
        }
        else
        {
            delete linkedGO;
            linkedGO = NULL;
            return;
        }
    }
}

void Spell::EffectScriptEffect(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    // TODO: we must implement hunter pet summon at login there (spell 6962)
    switch (m_spellInfo->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (m_spellInfo->Id)
            {
                case 111922:// Fortitude (Runescroll of Fortitude III)
                    if (!unitTarget)
                        m_caster->CastSpell(m_caster, 111923, true);
                    else
                        m_caster->CastSpell(unitTarget, 111923, true);
                    break;
                case 86958: // Mobile Banking
                    m_caster->CastSpell(m_caster, 88304, true);
                    break;
                case 63975: // Glyph of Backstab
                {
                    if (!unitTarget)
                        break;

                    // search our Rupture aura on target
                    if (constAuraEffectPtr aurEff = unitTarget->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_ROGUE, 0x00100000, 0, 0, m_caster->GetGUID()))
                    {
                        uint32 countMin = aurEff->GetBase()->GetMaxDuration();
                        uint32 countMax = 12000; // this can be wrong, duration should be based on combo-points
                        countMax += m_caster->HasAura(56801) ? 4000 : 0;

                        if (countMin < countMax)
                        {
                            aurEff->GetBase()->SetDuration(uint32(aurEff->GetBase()->GetDuration() + 3000));
                            aurEff->GetBase()->SetMaxDuration(countMin + 2000);
                        }

                    }
                    return;
                }
                case 69961: // Glyph of Scourge Strike
                {
                    if (!unitTarget)
                        break;

                    Unit::AuraEffectList const &mPeriodic = unitTarget->GetAuraEffectsByType(SPELL_AURA_PERIODIC_DAMAGE);
                    for (Unit::AuraEffectList::const_iterator i = mPeriodic.begin(); i != mPeriodic.end(); ++i)
                    {
                        constAuraEffectPtr aurEff = *i;
                        SpellInfo const* spellInfo = aurEff->GetSpellInfo();
                        // search our Blood Plague and Frost Fever on target
                        if (spellInfo->SpellFamilyName == SPELLFAMILY_DEATHKNIGHT && spellInfo->SpellFamilyFlags[2] & 0x2 &&
                            aurEff->GetCasterGUID() == m_caster->GetGUID())
                        {
                            uint32 countMin = aurEff->GetBase()->GetMaxDuration();
                            uint32 countMax = spellInfo->GetMaxDuration();

                            // this Glyph
                            countMax += 9000;
                            // talent Epidemic
                            if (constAuraEffectPtr epidemic = m_caster->GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELLFAMILY_DEATHKNIGHT, 234, EFFECT_0))
                                countMax += epidemic->GetAmount();

                            if (countMin < countMax)
                            {
                                aurEff->GetBase()->SetDuration(aurEff->GetBase()->GetDuration() + 3000);
                                aurEff->GetBase()->SetMaxDuration(countMin + 3000);
                            }
                        }
                    }
                    return;
                }
                case 45204: // Clone Me!
                    if (unitTarget)
                        m_caster->CastSpell(unitTarget, damage, true);
                    break;
                case 55693:                                 // Remove Collapsing Cave Aura
                    if (!unitTarget)
                        return;
                    unitTarget->RemoveAurasDueToSpell(m_spellInfo->Effects[effIndex].CalcValue());
                    break;
                case 26275: // PX-238 Winter Wondervolt TRAP
                {
                    uint32 spells[4] = { 26272, 26157, 26273, 26274 };

                    // check presence
                    for (uint8 j = 0; j < 4; ++j)
                        if (unitTarget->HasAuraEffect(spells[j], 0))
                            return;

                    // select spell
                    uint32 iTmpSpellId = spells[urand(0, 3)];

                    // cast
                    unitTarget->CastSpell(unitTarget, iTmpSpellId, true);
                    return;
                }
                case 8856:  // Bending Shinbone
                {
                    if (!itemTarget && m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spell_id = roll_chance_i(20) ? 8854 : 8855;

                    m_caster->CastSpell(m_caster, spell_id, true, NULL);
                    return;
                }
                case 24590: // Brittle Armor - need remove one 24575 Brittle Armor aura
                    if (unitTarget)
                        unitTarget->RemoveAuraFromStack(24575);
                    return;
                case 26465: // Mercurial Shield - need remove one 26464 Mercurial Shield aura
                    if (unitTarget)
                        unitTarget->RemoveAuraFromStack(26464);
                    return;
                case 22539:
                case 22972:
                case 22975:
                case 22976:
                case 22977:
                case 22978:
                case 22979:
                case 22980:
                case 22981:
                case 22982:
                case 22983:
                case 22984:
                case 22985: // Shadow Flame (All script effects, not just end ones to prevent player from dodging the last triggered spell)
                {
                    if (!unitTarget || !unitTarget->isAlive())
                        return;

                    // Onyxia Scale Cloak
                    if (unitTarget->HasAura(22683))
                        return;

                    // Shadow Flame
                    m_caster->CastSpell(unitTarget, 22682, true);
                    return;
                }
                case 17512: // Piccolo of the Flaming Fire
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;
                    unitTarget->HandleEmoteCommand(EMOTE_STATE_DANCE);
                    return;
                }
                case 28374: // Decimate
                case 54426:
                    if (unitTarget)
                    {
                        int32 damage = int32(unitTarget->GetHealth()) - int32(unitTarget->CountPctFromMaxHealth(5));
                        if (damage > 0)
                            m_caster->CastCustomSpell(28375, SPELLVALUE_BASE_POINT0, damage, unitTarget);
                    }
                    return;
                case 29830: // Mirren's Drinking Hat
                {
                    uint32 item = 0;
                    switch (urand(1, 6))
                    {
                        case 1:
                        case 2:
                        case 3:
                            item = 23584; break;            // Loch Modan Lager
                        case 4:
                        case 5:
                            item = 23585; break;            // Stouthammer Lite
                        case 6:
                            item = 23586; break;            // Aerie Peak Pale Ale
                    }
                    if (item)
                        DoCreateItem(effIndex, item);
                    break;
                }
                case 20589: // Escape artist
                case 30918: // Improved Sprint
                {
                    // Removes snares and roots.
                    unitTarget->RemoveMovementImpairingAuras();
                    break;
                }
                case 32307: // Plant Warmaul Ogre Banner
                    if (Player* caster = m_caster->ToPlayer())
                    {
                        caster->RewardPlayerAndGroupAtEvent(18388, unitTarget);
                        if (Creature* target = unitTarget->ToCreature())
                        {
                            target->setDeathState(CORPSE);
                            target->RemoveCorpse();
                        }
                    }
                    break;
                case 41931: // Mug Transformation
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint8 bag = 19;
                    uint8 slot = 0;
                    Item* item = NULL;

                    while (bag) // 256 = 0 due to var type
                    {
                        item = m_caster->ToPlayer()->GetItemByPos(bag, slot);
                        if (item && item->GetEntry() == 38587)
                            break;

                        ++slot;
                        if (slot == 39)
                        {
                            slot = 0;
                            ++bag;
                        }
                    }
                    if (bag)
                    {
                        if (m_caster->ToPlayer()->GetItemByPos(bag, slot)->GetCount() == 1) m_caster->ToPlayer()->RemoveItem(bag, slot, true);
                        else m_caster->ToPlayer()->GetItemByPos(bag, slot)->SetCount(m_caster->ToPlayer()->GetItemByPos(bag, slot)->GetCount() - 1);
                        // Spell 42518 (Braufest - Gratisprobe des Braufest herstellen)
                        m_caster->CastSpell(m_caster, 42518, true);
                        return;
                    }
                    break;
                }
                case 45141: // Brutallus - Burn
                case 45151:
                {
                    //Workaround for Range ... should be global for every ScriptEffect
                    float radius = m_spellInfo->Effects[effIndex].CalcRadius();
                    if (unitTarget && unitTarget->GetTypeId() == TYPEID_PLAYER && unitTarget->GetDistance(m_caster) >= radius && !unitTarget->HasAura(46394) && unitTarget != m_caster)
                        unitTarget->CastSpell(unitTarget, 46394, true);

                    break;
                }
                case 46203: // Goblin Weather Machine
                {
                    if (!unitTarget)
                        return;

                    uint32 spellId = 0;
                    switch (rand() % 4)
                    {
                        case 0: spellId = 46740; break;
                        case 1: spellId = 46739; break;
                        case 2: spellId = 46738; break;
                        case 3: spellId = 46736; break;
                    }
                    unitTarget->CastSpell(unitTarget, spellId, true);
                    break;
                }
                case 46642: // 5, 000 Gold
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    unitTarget->ToPlayer()->ModifyMoney(5000 * GOLD);

                    break;
                }
                case 47770: // Roll Dice - Decahedral Dwarven Dice
                {
                    char buf[128];
                    const char *gender = "his";
                    if (m_caster->getGender() > 0)
                        gender = "her";
                    sprintf(buf, "%s rubs %s [Decahedral Dwarven Dice] between %s hands and rolls. One %u and one %u.", m_caster->GetName(), gender, gender, urand(1, 10), urand(1, 10));
                    m_caster->MonsterTextEmote(buf, 0);
                    break;
                }
                case 47776: // Roll 'dem Bones - Worn Troll Dice
                {
                    char buf[128];
                    const char *gender = "his";
                    if (m_caster->getGender() > 0)
                        gender = "her";
                    sprintf(buf, "%s causually tosses %s [Worn Troll Dice]. One %u and one %u.", m_caster->GetName(), gender, urand(1, 6), urand(1, 6));
                    m_caster->MonsterTextEmote(buf, 0);
                    break;
                }
                case 50725: // Vigilance
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Remove Taunt cooldown
                    unitTarget->ToPlayer()->RemoveSpellCooldown(355, true);

                    return;
                }
                case 51519: // Death Knight Initiate Visual
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    uint32 iTmpSpellId = 0;
                    switch (unitTarget->GetDisplayId())
                    {
                        case 25369:
                            iTmpSpellId = 51552;
                            break; // bloodelf female
                        case 25373:
                            iTmpSpellId = 51551;
                            break; // bloodelf male
                        case 25363:
                            iTmpSpellId = 51542;
                            break; // draenei female
                        case 25357:
                            iTmpSpellId = 51541;
                            break; // draenei male
                        case 25361:
                            iTmpSpellId = 51537;
                            break; // dwarf female
                        case 25356:
                            iTmpSpellId = 51538;
                            break; // dwarf male
                        case 25372:
                            iTmpSpellId = 51550;
                            break; // forsaken female
                        case 25367:
                            iTmpSpellId = 51549;
                            break; // forsaken male
                        case 25362:
                            iTmpSpellId = 51540;
                            break; // gnome female
                        case 25359:
                            iTmpSpellId = 51539;
                            break; // gnome male
                        case 25355:
                            iTmpSpellId = 51534;
                            break; // human female
                        case 25354:
                            iTmpSpellId = 51520;
                            break; // human male
                        case 25360:
                            iTmpSpellId = 51536;
                            break; // nightelf female
                        case 25358:
                            iTmpSpellId = 51535;
                            break; // nightelf male
                        case 25368:
                            iTmpSpellId = 51544;
                            break; // orc female
                        case 25364:
                            iTmpSpellId = 51543;
                            break; // orc male
                        case 25371:
                            iTmpSpellId = 51548;
                            break; // tauren female
                        case 25366:
                            iTmpSpellId = 51547;
                            break; // tauren male
                        case 25370:
                            iTmpSpellId = 51545;
                            break; // troll female
                        case 25365:
                            iTmpSpellId = 51546;
                            break; // troll male
                        default:
                            return;
                    }

                    unitTarget->CastSpell(unitTarget, iTmpSpellId, true);
                    Creature* npc = unitTarget->ToCreature();
                    npc->LoadEquipment();
                    return;
                }
                case 51770: // Emblazon Runeblade
                {
                    if (!m_originalCaster)
                        return;

                    m_originalCaster->CastSpell(m_originalCaster, damage, false);
                    break;
                }
                // Deathbolt from Thalgran Blightbringer
                // reflected by Freya's Ward
                // Retribution by Sevenfold Retribution
                case 51854:
                {
                    if (!unitTarget)
                        return;
                    if (unitTarget->HasAura(51845))
                        unitTarget->CastSpell(m_caster, 51856, true);
                    else
                        m_caster->CastSpell(unitTarget, 51855, true);
                    break;
                }
                case 51904: // Summon Ghouls On Scarlet Crusade
                {
                    if (!m_targets.HasDst())
                        return;

                    float x, y, z;
                    float radius = m_spellInfo->Effects[effIndex].CalcRadius();
                    for (uint8 i = 0; i < 15; ++i)
                    {
                        m_caster->GetRandomPoint(*destTarget, radius, x, y, z);
                        m_caster->CastSpell(x, y, z, 54522, true);
                    }
                    break;
                }
                case 52173: // Coyote Spirit Despawn
                case 60243: // Blood Parrot Despawn
                    if (unitTarget->GetTypeId() == TYPEID_UNIT && unitTarget->ToCreature()->isSummon())
                        unitTarget->ToTempSummon()->UnSummon();
                    return;
                case 52479: // Gift of the Harvester
                    if (unitTarget && m_originalCaster)
                        m_originalCaster->CastSpell(unitTarget, urand(0, 1) ? damage : 52505, true);
                    return;
                case 53110: // Devour Humanoid
                    if (unitTarget)
                        unitTarget->CastSpell(m_caster, damage, true);
                    return;
                case 57347: // Retrieving (Wintergrasp RP-GG pickup spell)
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT || m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    unitTarget->ToCreature()->DespawnOrUnsummon();

                    return;
                }
                case 57349: // Drop RP-GG (Wintergrasp RP-GG at death drop spell)
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Delete item from inventory at death
                    m_caster->ToPlayer()->DestroyItemCount(damage, 5, true);

                    return;
                }
                case 58418:                                 // Portal to Orgrimmar
                case 58420:                                 // Portal to Stormwind
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER || effIndex != 0)
                        return;

                    uint32 spellID = m_spellInfo->Effects[EFFECT_0].CalcValue();
                    uint32 questID = m_spellInfo->Effects[EFFECT_1].CalcValue();

                    if (unitTarget->ToPlayer()->GetQuestStatus(questID) == QUEST_STATUS_COMPLETE)
                        unitTarget->CastSpell(unitTarget, spellID, true);

                    return;
                }
                case 58941:                                 // Rock Shards
                    if (unitTarget && m_originalCaster)
                    {
                        for (uint32 i = 0; i < 3; ++i)
                        {
                            m_originalCaster->CastSpell(unitTarget, 58689, true);
                            m_originalCaster->CastSpell(unitTarget, 58692, true);
                        }
                        if (((InstanceMap*)m_originalCaster->GetMap())->GetDifficulty() == REGULAR_5_DIFFICULTY)
                        {
                            m_originalCaster->CastSpell(unitTarget, 58695, true);
                            m_originalCaster->CastSpell(unitTarget, 58696, true);
                        }
                        else
                        {
                            m_originalCaster->CastSpell(unitTarget, 60883, true);
                            m_originalCaster->CastSpell(unitTarget, 60884, true);
                        }
                    }
                    return;
                case 58983: // Big Blizzard Bear
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Prevent stacking of mounts and client crashes upon dismounting
                    unitTarget->RemoveAurasByType(SPELL_AURA_MOUNTED);

                    // Triggered spell id dependent on riding skill
                    if (uint16 skillval = unitTarget->ToPlayer()->GetSkillValue(SKILL_RIDING))
                    {
                        if (skillval >= 150)
                            unitTarget->CastSpell(unitTarget, 58999, true);
                        else
                            unitTarget->CastSpell(unitTarget, 58997, true);
                    }
                    return;
                }
                case 63845: // Create Lance
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (m_caster->ToPlayer()->GetTeam() == ALLIANCE)
                        m_caster->CastSpell(m_caster, 63914, true);
                    else
                        m_caster->CastSpell(m_caster, 63919, true);
                    return;
                }
                case 59317:                                 // Teleporting
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // return from top
                    if (unitTarget->ToPlayer()->GetAreaId() == 4637)
                        unitTarget->CastSpell(unitTarget, 59316, true);
                    // teleport atop
                    else
                        unitTarget->CastSpell(unitTarget, 59314, true);

                    return;
                    // random spell learn instead placeholder
                case 60893:                                 // Northrend Alchemy Research
                case 61177:                                 // Northrend Inscription Research
                case 61288:                                 // Minor Inscription Research
                case 61756:                                 // Northrend Inscription Research (FAST QA VERSION)
                case 64323:                                 // Book of Glyph Mastery
                case 112996:                                // Scroll of Wisdom
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // learn random explicit discovery recipe (if any)
                    if (uint32 discoveredSpell = GetExplicitDiscoverySpell(m_spellInfo->Id, m_caster->ToPlayer()))
                        m_caster->ToPlayer()->learnSpell(discoveredSpell, false);
                    return;
                }
                case 62482: // Grab Crate
                {
                    if (unitTarget)
                    {
                        if (Unit* seat = m_caster->GetVehicleBase())
                        {
                            if (Unit* parent = seat->GetVehicleBase())
                            {
                                // TODO: a hack, range = 11, should after some time cast, otherwise too far
                                m_caster->CastSpell(parent, 62496, true);
                                unitTarget->CastSpell(parent, m_spellInfo->Effects[EFFECT_0].CalcValue());
                            }
                        }
                    }
                    return;
                }
                // Stoneclaw Totem
                case 55328: // Rank 1
                case 55329: // Rank 2
                case 55330: // Rank 3
                case 55332: // Rank 4
                case 55333: // Rank 5
                case 55335: // Rank 6
                case 55278: // Rank 7
                case 58589: // Rank 8
                case 58590: // Rank 9
                case 58591: // Rank 10
                {
                    int32 basepoints0 = damage;
                    // Cast Absorb on totems
                    for (uint8 slot = SUMMON_SLOT_TOTEM; slot < MAX_TOTEM_SLOT; ++slot)
                    {
                        if (!unitTarget->m_SummonSlot[slot])
                            continue;

                        Creature* totem = unitTarget->GetMap()->GetCreature(unitTarget->m_SummonSlot[slot]);
                        if (totem && totem->isTotem())
                        {
                            m_caster->CastCustomSpell(totem, 55277, &basepoints0, NULL, NULL, true);
                        }
                    }
                    // Glyph of Stoneclaw Totem
                    if (AuraEffectPtr aur = unitTarget->GetAuraEffect(63298, 0))
                    {
                        basepoints0 *= aur->GetAmount();
                        m_caster->CastCustomSpell(unitTarget, 55277, &basepoints0, NULL, NULL, true);
                    }
                    break;
                }
                case 66545: //Summon Memory
                {
                    uint8 uiRandom = urand(0, 25);
                    uint32 uiSpells[26] = { 66704, 66705, 66706, 66707, 66709, 66710, 66711, 66712, 66713, 66714, 66715, 66708, 66708, 66691, 66692, 66694, 66695, 66696, 66697, 66698, 66699, 66700, 66701, 66702, 66703, 66543 };

                    m_caster->CastSpell(m_caster, uiSpells[uiRandom], true);
                    break;
                }
                case 45668:                                 // Ultra-Advanced Proto-Typical Shortening Blaster
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    if (roll_chance_i(50))                  // chance unknown, using 50
                        return;

                    static uint32 const spellPlayer[5] =
                    {
                        45674,                            // Bigger!
                        45675,                            // Shrunk
                        45678,                            // Yellow
                        45682,                            // Ghost
                        45684                             // Polymorph
                    };

                    static uint32 const spellTarget[5] =
                    {
                        45673,                            // Bigger!
                        45672,                            // Shrunk
                        45677,                            // Yellow
                        45681,                            // Ghost
                        45683                             // Polymorph
                    };

                    m_caster->CastSpell(m_caster, spellPlayer[urand(0, 4)], true);
                    unitTarget->CastSpell(unitTarget, spellTarget[urand(0, 4)], true);
                    break;
                }
                // Fortune Cookie
                case 87604:
                {
                    DoCreateItem(effIndex, urand(62552, 62591));
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_POTION:
        {
            switch (m_spellInfo->Id)
            {
                // Netherbloom
                case 28702:
                {
                    if (!unitTarget)
                        return;
                    // 25% chance of casting a random buff
                    if (roll_chance_i(75))
                        return;

                    // triggered spells are 28703 to 28707
                    // Note: some sources say, that there was the possibility of
                    //       receiving a debuff. However, this seems to be removed by a patch.
                    const uint32 spellid = 28703;

                    // don't overwrite an existing aura
                    for (uint8 i = 0; i < 5; ++i)
                        if (unitTarget->HasAura(spellid + i))
                            return;
                    unitTarget->CastSpell(unitTarget, spellid + urand(0, 4), true);
                    break;
                }

                // Nightmare Vine
                case 28720:
                {
                    if (!unitTarget)
                        return;
                    // 25% chance of casting Nightmare Pollen
                    if (roll_chance_i(75))
                        return;
                    unitTarget->CastSpell(unitTarget, 28721, true);
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_WARRIOR:
        {
            // Shattering Throw
            if (m_spellInfo->SpellFamilyFlags[1] & 0x00400000)
            {
                if (!unitTarget)
                    return;
                // remove shields, will still display immune to damage part
                unitTarget->RemoveAurasWithMechanic(1 << MECHANIC_IMMUNE_SHIELD, AURA_REMOVE_BY_ENEMY_SPELL);
                return;
            }
            else if (m_spellInfo->Id == 107574)
                m_caster->RemoveAurasWithMechanic((1 << MECHANIC_SNARE) | (1 << MECHANIC_ROOT) | (1 << MECHANIC_CHARM));

            break;
        }
        case SPELLFAMILY_SHAMAN:
        {
            switch (m_spellInfo->Id)
            {
                // Earthquake (stun effect)
                case 77478:
                    if (roll_chance_i(10))
                        m_caster->CastSpell(unitTarget, 77505, true);
                    break;
                    // Taming the Flames, Item - Shaman T12 Elemental 2P Bonus
                case 99202:
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    m_caster->ToPlayer()->ReduceSpellCooldown(2894, 4000);
                    break;
            }
        }
    }

    // normal DB scripted effect
    m_caster->GetMap()->ScriptsStart(sSpellScripts, uint32(m_spellInfo->Id | (effIndex << 24)), m_caster, unitTarget);
}

void Spell::EffectSanctuary(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    unitTarget->getHostileRefManager().UpdateVisibility();

    unitTarget->CombatStop(false);

    // Vanish allows to remove all threat and cast regular stealth so other spells can be used
    if (m_caster->GetTypeId() == TYPEID_PLAYER && m_spellInfo->Id == 131369)
    {
        m_caster->ToPlayer()->RemoveAurasByType(SPELL_AURA_MOD_ROOT);
        m_caster->ToPlayer()->RemoveAurasByType(SPELL_AURA_MOD_ROOT_2);

        // Overkill
        if (m_caster->ToPlayer()->HasSpell(58426))
            m_caster->CastSpell(m_caster, 58427, true);
    }
    else if (!IsTriggered())
        unitTarget->m_lastSanctuaryTime = getMSTime();
}

void Spell::EffectAddComboPoints(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    if (damage <= 0)
        return;

    m_caster->AddComboPoints(damage);
}

void Spell::EffectDuel(SpellEffIndex p_EffectIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || m_caster->GetTypeId() != TYPEID_PLAYER || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* l_Caster = m_caster->ToPlayer();
    Player* l_Target = unitTarget->ToPlayer();

    // caster or target already have requested duel
    if (l_Caster->m_Duel || l_Target->m_Duel || !l_Target->GetSocial() || l_Target->GetSocial()->HasIgnore(l_Caster->GetGUIDLow()))
        return;

    // Players can only fight a duel in zones with this flag
    AreaTableEntry const* l_CasterAreaEntry = GetAreaEntryByAreaID(l_Caster->GetAreaId());
    if (l_CasterAreaEntry && !(l_CasterAreaEntry->Flags & AREA_FLAG_ALLOW_DUELS))
    {
        SendCastResult(SPELL_FAILED_NO_DUELING);            ///< Dueling isn't allowed here
        return;
    }

    AreaTableEntry const* l_TargetAreaEntry = GetAreaEntryByAreaID(l_Target->GetAreaId());
    if (l_TargetAreaEntry && !(l_TargetAreaEntry->Flags & AREA_FLAG_ALLOW_DUELS))
    {
        SendCastResult(SPELL_FAILED_NO_DUELING);            ///< Dueling isn't allowed here
        return;
    }

    //CREATE DUEL FLAG OBJECT
    GameObject* l_GameObj = new GameObject;

    uint32 l_GameobjectId = m_spellInfo->Effects[p_EffectIndex].MiscValue;

    Map* l_CasterMap = m_caster->GetMap();
    if (!l_GameObj->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_GAMEOBJECT), l_GameobjectId,
        l_CasterMap, m_caster->GetPhaseMask(),
        m_caster->GetPositionX() + (unitTarget->GetPositionX() - m_caster->GetPositionX()) / 2,
        m_caster->GetPositionY() + (unitTarget->GetPositionY() - m_caster->GetPositionY()) / 2,
        m_caster->GetPositionZ(),
        m_caster->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 0, GO_STATE_READY))
    {
        delete l_GameObj;
        return;
    }

    l_GameObj->SetUInt32Value(GAMEOBJECT_FIELD_FACTION_TEMPLATE, m_caster->getFaction());
    l_GameObj->SetUInt32Value(GAMEOBJECT_FIELD_LEVEL, m_caster->getLevel() + 1);
    int32 l_Duration = m_spellInfo->GetDuration();
    l_GameObj->SetRespawnTime(l_Duration > 0 ? l_Duration / IN_MILLISECONDS : 0);
    l_GameObj->SetSpellId(m_spellInfo->Id);

    ExecuteLogEffectSummonObject(p_EffectIndex, l_GameObj);

    m_caster->AddGameObject(l_GameObj);
    l_CasterMap->AddToMap(l_GameObj);
    //END

    // Send request
    uint64 l_ArbiterGUID = l_GameObj->GetGUID();
    uint64 l_RequestedByGUID = l_Caster->GetGUID();
    uint64 l_RequestByBNetGUID = l_Caster->GetSession()->GetBNetAccountGUID();

    WorldPacket l_Data(SMSG_DUEL_REQUESTED);
    l_Data.appendPackGUID(l_ArbiterGUID);
    l_Data.appendPackGUID(l_RequestedByGUID);
    l_Data.appendPackGUID(l_RequestByBNetGUID);

    l_Caster->GetSession()->SendPacket(&l_Data);
    l_Target->GetSession()->SendPacket(&l_Data);

    // create duel-info
    DuelInfo* l_Duel = new DuelInfo;
    l_Duel->initiator = l_Caster;
    l_Duel->opponent = l_Target;
    l_Duel->startTime = 0;
    l_Duel->startTimer = 0;
    l_Duel->isMounted = (GetSpellInfo()->Id == 62875); // Mounted Duel
    l_Duel->started = false;
    l_Caster->m_Duel = l_Duel;

    DuelInfo* l_Duel2 = new DuelInfo;
    l_Duel2->initiator = l_Caster;
    l_Duel2->opponent = l_Caster;
    l_Duel2->startTime = 0;
    l_Duel2->startTimer = 0;
    l_Duel2->isMounted = (GetSpellInfo()->Id == 62875); // Mounted Duel
    l_Duel2->started = false;
    l_Target->m_Duel = l_Duel2;

    l_Caster->SetGuidValue(PLAYER_FIELD_DUEL_ARBITER, l_GameObj->GetGUID());
    l_Target->SetGuidValue(PLAYER_FIELD_DUEL_ARBITER, l_GameObj->GetGUID());

    sScriptMgr->OnPlayerDuelRequest(l_Target, l_Caster);
}

void Spell::EffectStuck(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (!m_caster || m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    if (!sWorld->getBoolConfig(CONFIG_CAST_UNSTUCK))
        return;

    Player* l_Player = m_caster->ToPlayer();
    if (!l_Player)
        return;

    if (l_Player->isInFlight())
        return;

    // if player is dead without death timer is teleported to graveyard, otherwise not apply the effect
    if (l_Player->isDead() && !l_Player->GetDeathTimer())
    {
        l_Player->RepopAtGraveyard();
        return;
    }

    // the player dies if hearthstone is in cooldown
    if (l_Player->HasSpellCooldown(8690))
    {
        l_Player->Kill(l_Player);
        return;
    }

    // the player is teleported to home
    l_Player->TeleportTo(l_Player->m_homebindMapId, l_Player->m_homebindX, l_Player->m_homebindY, l_Player->m_homebindZ, l_Player->GetOrientation(), TELE_TO_SPELL);

    // Stuck spell trigger Hearthstone cooldown
    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(8690);
    if (!l_SpellInfo)
        return;
    Spell l_Spell(l_Player, l_SpellInfo, TRIGGERED_FULL_MASK);
    l_Spell.SendSpellCooldown();
}

void Spell::EffectSummonPlayer(SpellEffIndex /*effIndex*/)
{
    // workaround - this effect should not use target map
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    // Evil Twin (ignore player summon, but hide this for summoner)
    if (unitTarget->HasAura(23445))
        return;

    float x, y, z;
    m_caster->GetPosition(x, y, z);

    unitTarget->ToPlayer()->SetSummonPoint(m_caster->GetMapId(), x, y, z);

    WorldPacket l_Data(SMSG_SUMMON_REQUEST, 8 + 4 + 4);
    l_Data.appendPackGUID(m_caster->GetGUID());
    l_Data << uint32(MAX_PLAYER_SUMMON_DELAY * IN_MILLISECONDS);    // auto decline after msecs
    l_Data << uint32(m_caster->GetZoneId());                        // summoner zone
    l_Data.WriteBit(false);                                         // UnkBit
    unitTarget->ToPlayer()->GetSession()->SendPacket(&l_Data);
}

void Spell::EffectActivateObject(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!gameObjTarget)
        return;

    ScriptInfo activateCommand;
    activateCommand.command = SCRIPT_COMMAND_ACTIVATE_OBJECT;

    // int32 unk = m_spellInfo->Effects[effIndex].MiscValue; // This is set for EffectActivateObject spells; needs research

    gameObjTarget->GetMap()->ScriptCommandStart(activateCommand, 0, m_caster, gameObjTarget);
}

void Spell::EffectApplyGlyph(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER || m_glyphIndex >= MAX_GLYPH_SLOT_INDEX)
        return;

    Player* player = (Player*)m_caster;

    // glyph sockets level requirement
    uint8 minLevel = 0;
    switch (m_glyphIndex)
    {
    case 0:
    case 1:
    case 6: minLevel = 25; break;
    case 2:
    case 3:
    case 7: minLevel = 50; break;
    case 4:
    case 5:
    case 8: minLevel = 75; break;
    }

    if (minLevel && m_caster->getLevel() < minLevel)
    {
        SendCastResult(SPELL_FAILED_GLYPH_SOCKET_LOCKED);
        return;
    }

    // apply new one
    if (uint32 glyph = m_spellInfo->Effects[effIndex].MiscValue)
    {
        if (GlyphPropertiesEntry const* gp = sGlyphPropertiesStore.LookupEntry(glyph))
        {
            if (GlyphSlotEntry const* gs = sGlyphSlotStore.LookupEntry(player->GetGlyphSlot(m_glyphIndex)))
            {
                if (gp->TypeFlags != gs->TypeFlags)
                {
                    SendCastResult(SPELL_FAILED_INVALID_GLYPH);
                    return;                                 // glyph slot mismatch
                }
            }

            // remove old glyph
            if (uint32 oldglyph = player->GetGlyph(player->GetActiveSpec(), m_glyphIndex))
            {
                if (GlyphPropertiesEntry const* old_gp = sGlyphPropertiesStore.LookupEntry(oldglyph))
                {
                    player->RemoveAurasDueToSpell(old_gp->SpellId);
                    player->SetGlyph(m_glyphIndex, 0);
                }
            }

            player->CastSpell(m_caster, gp->SpellId, true);
            player->SetGlyph(m_glyphIndex, glyph);
            player->SendTalentsInfoData(false);
        }
    }
    else
    {
        // remove old glyph
        if (uint32 oldglyph = player->GetGlyph(player->GetActiveSpec(), m_glyphIndex))
        {
            if (GlyphPropertiesEntry const* old_gp = sGlyphPropertiesStore.LookupEntry(oldglyph))
            {
                player->RemoveAurasDueToSpell(old_gp->SpellId);
                player->SetGlyph(m_glyphIndex, 0);
                player->SendTalentsInfoData(false);
            }
        }
    }
}

void Spell::EffectEnchantHeldItem(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    // this is only item spell effect applied to main-hand weapon of target player (players in area)
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* item_owner = (Player*)unitTarget;
    Item* item = item_owner->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);

    if (!item)
        return;

    // must be equipped
    if (!item->IsEquipped())
        return;

    if (m_spellInfo->Effects[effIndex].MiscValue)
    {
        uint32 enchant_id = m_spellInfo->Effects[effIndex].MiscValue;
        int32 duration = m_spellInfo->GetDuration();          //Try duration index first ..
        if (!duration)
            duration = damage;//+1;            //Base points after ..
        if (!duration)
            duration = 10;                                  //10 seconds for enchants which don't have listed duration

        SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
        if (!pEnchant)
            return;

        // Always go to temp enchantment slot
        EnchantmentSlot slot = TEMP_ENCHANTMENT_SLOT;

        // Enchantment will not be applied if a different one already exists
        if (item->GetEnchantmentId(slot) && item->GetEnchantmentId(slot) != enchant_id)
            return;

        // Apply the temporary enchantment
        item->SetEnchantment(slot, enchant_id, duration*IN_MILLISECONDS, 0);
        item_owner->ApplyEnchantment(item, slot, true);
    }
}

void Spell::EffectDisEnchant(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!itemTarget || !itemTarget->GetTemplate()->DisenchantID)
        return;

    if (Player* caster = m_caster->ToPlayer())
    {
        caster->UpdateCraftSkill(m_spellInfo->Id);
        caster->SendLoot(itemTarget->GetGUID(), LOOT_DISENCHANTING);
    }

    // item will be removed at disenchanting end
}

void Spell::EffectInebriate(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = unitTarget->ToPlayer();
    uint8 currentDrunk = player->GetDrunkValue();
    uint8 drunkMod = damage;
    if (currentDrunk + drunkMod > 100)
    {
        currentDrunk = 100;
        if (rand_chance() < 25.0f)
            player->CastSpell(player, 67468, false);    // Drunken Vomit
    }
    else
        currentDrunk += drunkMod;

    player->SetDrunkValue(currentDrunk, m_CastItem ? m_CastItem->GetEntry() : 0);
}

void Spell::EffectFeedPet(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    Player* player = m_caster->ToPlayer();
    if (!player)
        return;

    Item* foodItem = itemTarget;
    if (!foodItem)
        return;

    Pet* pet = player->GetPet();
    if (!pet)
        return;

    if (!pet->isAlive())
        return;

    ExecuteLogEffectDestroyItem(effIndex, foodItem->GetEntry());

    uint32 count = 1;
    player->DestroyItemCount(foodItem, count, true);
    // TODO: fix crash when a spell has two effects, both pointed at the same item target

    m_caster->CastSpell(pet, m_spellInfo->Effects[effIndex].TriggerSpell, true);
}

void Spell::EffectDismissPet(SpellEffIndex p_EffIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->isPet())
        return;

    Pet* l_Pet = unitTarget->ToPet();

    G3D::Vector3 l_PetDismissPosition = G3D::Vector3(l_Pet->GetPositionX(), l_Pet->GetPositionY(), l_Pet->GetPositionZ());

    WorldPacket l_Data(SMSG_PET_DISMISS_SOUND);
    l_Data << uint32(l_Pet->GetDisplayId());
    l_Data.WriteVector3(l_PetDismissPosition);
    l_Pet->GetOwner()->GetSession()->SendPacket(&l_Data);

    ExecuteLogEffectUnsummonObject(p_EffIndex, l_Pet);
    l_Pet->GetOwner()->RemovePet(l_Pet, PET_SLOT_ACTUAL_PET_SLOT, false, l_Pet->m_Stampeded);
}

void Spell::EffectSummonObject(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    uint32 go_id = m_spellInfo->Effects[effIndex].MiscValue;

    float o = m_caster->GetOrientation();

    int32 duration = 0;

    // Archaeology
    if (m_spellInfo->Id == 80451)
    {
        if (m_caster->ToPlayer())
            go_id = m_caster->ToPlayer()->GetArchaeologyMgr().GetSurveyBotEntry(o);

        duration = 5000;
    }

    if (go_id == 0)
        return;

    int8 slot = 0;

    switch (m_spellInfo->Effects[effIndex].Effect)
    {
    case SPELL_EFFECT_SUMMON_OBJECT_SLOT1:
        slot = m_spellInfo->Effects[effIndex].MiscValueB;
        break;
    case SPELL_EFFECT_SUMMON_OBJECT_SLOT2:
        slot = 1;
        break;
    case SPELL_EFFECT_SUMMON_OBJECT_SLOT3:
        slot = 2;
        break;
    case SPELL_EFFECT_SUMMON_OBJECT_SLOT4:
        slot = 3;
        break;
    case SPELL_EFFECT_SUMMON_OBJECT:
        slot = -1;
        break;
    default:
        return;
    }

    switch (m_spellInfo->Id)
    {
    case 84996: // Raid Marker 1
    case 84997: // Raid Marker 2
    case 84998: // Raid Marker 3
    case 84999: // Raid Marker 4
    case 85000: // Raid Marker 5
    {
        if (m_caster->GetTypeId() != TYPEID_PLAYER)
            return;

        float x = 0.0f, y = 0.0f, z = 0.0f;
        if (m_targets.HasDst())
            destTarget->GetPosition(x, y, z);

        if (Group* group = m_caster->ToPlayer()->GetGroup())
            group->AddRaidMarker(m_spellInfo->Id, m_caster->GetMapId(), x, y, z);
        return;
    }
    default:
        break;
    }

    if (slot != -1)
    {
        uint64 guid = m_caster->m_ObjectSlot[slot];
        if (guid != 0)
        {
            GameObject* obj = NULL;
            if (m_caster)
                obj = m_caster->GetMap()->GetGameObject(guid);

            if (obj)
            {
                // Recast case - null spell id to make auras not be removed on object remove from world
                if (m_spellInfo->Id == obj->GetSpellId())
                    obj->SetSpellId(0);
                m_caster->RemoveGameObject(obj, true);
            }
            m_caster->m_ObjectSlot[slot] = 0;
        }
    }

    GameObject* pGameObj = new GameObject;

    float x, y, z;
    // If dest location if present
    if (m_targets.HasDst())
        destTarget->GetPosition(x, y, z);
    // Summon in random point all other units if location present
    else
        m_caster->GetClosePoint(x, y, z, DEFAULT_WORLD_OBJECT_SIZE);

    Map* map = m_caster->GetMap();
    if (!pGameObj->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_GAMEOBJECT), go_id, map,
        m_caster->GetPhaseMask(), x, y, z, o, 0.0f, 0.0f, 0.0f, 0.0f, 0, GO_STATE_READY))
    {
        delete pGameObj;
        return;
    }

    if (!duration)
        duration = m_spellInfo->GetDuration();

    pGameObj->SetRespawnTime(duration > 0 ? duration / IN_MILLISECONDS : 0);
    pGameObj->SetSpellId(m_spellInfo->Id);
    m_caster->AddGameObject(pGameObj);

    ExecuteLogEffectSummonObject(effIndex, pGameObj);

    map->AddToMap(pGameObj);

    if (slot != -1)
        m_caster->m_ObjectSlot[slot] = pGameObj->GetGUID();
}

void Spell::EffectResurrect(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if (unitTarget->isAlive())
        return;
    if (!unitTarget->IsInWorld())
        return;

    switch (m_spellInfo->Id)
    {
        // Defibrillate (Goblin Jumper Cables) have 33% chance on success
    case 8342:
        if (roll_chance_i(67))
        {
            m_caster->CastSpell(m_caster, 8338, true, m_CastItem);
            return;
        }
        break;
        // Defibrillate (Goblin Jumper Cables XL) have 50% chance on success
    case 22999:
        if (roll_chance_i(50))
        {
            m_caster->CastSpell(m_caster, 23055, true, m_CastItem);
            return;
        }
        break;
        // Defibrillate (Gnomish Army Knife) have 67% chance on success_list
    case 54732:
        if (roll_chance_i(33))
        {
            return;
        }
        break;
    default:
        break;
    }

    Player* target = unitTarget->ToPlayer();

    if (target->IsRessurectRequested())       // already have one active request
        return;

    uint32 health = target->CountPctFromMaxHealth(damage);
    uint32 mana = CalculatePct(target->GetMaxPower(POWER_MANA), damage);

    // Rebirth, soulstone ...
    if (m_spellInfo->Id == 20484 || m_spellInfo->Id == 3026)
        health = target->CountPctFromMaxHealth(60);

    if (m_spellInfo->Id == 61999) // Raise Ally
        mana = target->CountPctFromMaxMana(60);

    ExecuteLogEffectResurrect(effIndex, target);

    target->SetResurrectRequestData(m_caster, health, mana, 0);
    SendResurrectRequest(target);
}

void Spell::EffectAddExtraAttacks(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->isAlive() || !unitTarget->getVictim())
        return;

    if (unitTarget->m_extraAttacks)
        return;

    unitTarget->m_extraAttacks = damage;

    ExecuteLogEffectExtraAttacks(effIndex, unitTarget->getVictim(), damage);
}

void Spell::EffectParry(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
        m_caster->ToPlayer()->SetCanParry(true);
}

void Spell::EffectBlock(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
        m_caster->ToPlayer()->SetCanBlock(true);
}

void Spell::EffectLeap(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->isInFlight())
        return;

    if (!m_targets.HasDst())
        return;

    Position pos;
    destTarget->GetPosition(&pos);
    unitTarget->GetFirstCollisionPosition(pos, unitTarget->GetDistance(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ() + 2.0f), 0.0f);
    unitTarget->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation(), unitTarget == m_caster);
}

void Spell::EffectReputation(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = unitTarget->ToPlayer();

    int32  rep_change = damage;

    uint32 faction_id = m_spellInfo->Effects[effIndex].MiscValue;

    FactionEntry const* factionEntry = sFactionStore.LookupEntry(faction_id);

    if (!factionEntry)
        return;

    if (RepRewardRate const* repData = sObjectMgr->GetRepRewardRate(faction_id))
    {
        rep_change = int32((float)rep_change * repData->spell_rate);
    }

    // Bonus from spells that increase reputation gain
    float bonus = rep_change * player->GetTotalAuraModifier(SPELL_AURA_MOD_REPUTATION_GAIN) / 100.0f; // 10%
    rep_change += (int32)bonus;

    player->GetReputationMgr().ModifyReputation(factionEntry, rep_change);
}

void Spell::EffectQuestComplete(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    Player* player = unitTarget->ToPlayer();

    uint32 questId = m_spellInfo->Effects[effIndex].MiscValue;
    if (questId)
    {
        Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
        if (!quest)
            return;

        uint16 logSlot = player->FindQuestSlot(questId);
        if (logSlot < MAX_QUEST_LOG_SIZE)
            player->AreaExploredOrEventHappens(questId);
        else if (player->CanTakeQuest(quest, false))    // never rewarded before
            player->CompleteQuest(questId);             // quest not in log - for internal use
    }
}

void Spell::EffectForceDeselect(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    WorldPacket l_Data(SMSG_CLEAR_TARGET, 16 + 2);
    l_Data.appendPackGUID(m_caster->GetGUID());

    m_caster->SendMessageToSet(&l_Data, true);
}

void Spell::EffectSelfResurrect(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (!m_caster || m_caster->isAlive())
        return;
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;
    if (!m_caster->IsInWorld())
        return;

    uint32 health = 0;
    uint32 mana = 0;

    // flat case
    if (damage < 0)
    {
        health = uint32(-damage);
        mana = m_spellInfo->Effects[effIndex].MiscValue;
    }
    // percent case
    else
    {
        if (m_spellInfo->Id == 3026) // Soulstone resurrect
        {
            health = m_caster->CountPctFromMaxHealth(60);
            if (m_caster->GetMaxPower(POWER_MANA) > 0)
                mana = CalculatePct(m_caster->GetMaxPower(POWER_MANA), damage);
        }
        else
        {
            health = m_caster->CountPctFromMaxHealth(damage);
            if (m_caster->GetMaxPower(POWER_MANA) > 0)
                mana = CalculatePct(m_caster->GetMaxPower(POWER_MANA), damage);
        }
    }

    Player* player = m_caster->ToPlayer();
    player->ResurrectPlayer(0.0f);

    player->SetHealth(health);
    player->SetPower(POWER_MANA, mana);
    player->SetPower(POWER_RAGE, 0);
    player->SetPower(POWER_ENERGY, player->GetMaxPower(POWER_ENERGY));
    player->SetPower(POWER_FOCUS, 0);

    player->SpawnCorpseBones();
}

void Spell::EffectSkinning(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (unitTarget->GetTypeId() != TYPEID_UNIT)
        return;
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Creature* creature = unitTarget->ToCreature();
    int32 targetLevel = creature->getLevel();

    uint32 skill = creature->GetCreatureTemplate()->GetRequiredLootSkill();

    m_caster->ToPlayer()->SendLoot(creature->GetGUID(), LOOT_SKINNING);
    creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);

    int32 reqValue = 0;

    if (targetLevel < 10)
        reqValue = 0;
    else if (targetLevel < 20)
        reqValue = (targetLevel - 10) * 10;
    else if (targetLevel < 74)
        reqValue = targetLevel * 5;
    else if (targetLevel < 81)
        reqValue = (targetLevel * 5) + (targetLevel - 73) * 5;
    else if (targetLevel < 85)
        reqValue = (targetLevel * 5) + 35;
    else if (targetLevel == 85) // @TODO find a generic formula
        reqValue = 470;
    else if (targetLevel == 86)
        reqValue = 485;
    else if (targetLevel == 87)
        reqValue = 495;
    else if (targetLevel == 88)
        reqValue = 520;
    else if (targetLevel == 89)
        reqValue = 540;
    else
        reqValue = 560;

    int32 skillValue = m_caster->ToPlayer()->GetPureSkillValue(skill);

    // Double chances for elites
    m_caster->ToPlayer()->UpdateGatherSkill(skill, skillValue, reqValue, creature->isElite() ? 2 : 1);
}

void Spell::EffectCharge(SpellEffIndex /*effIndex*/)
{
    if (!unitTarget)
        return;

    if (effectHandleMode == SPELL_EFFECT_HANDLE_LAUNCH_TARGET)
    {
        float angle = unitTarget->GetRelativeAngle(m_caster);
        Position pos;

        unitTarget->GetContactPoint(m_caster, pos.m_positionX, pos.m_positionY, pos.m_positionZ);
        unitTarget->GetFirstCollisionPosition(pos, unitTarget->GetObjectSize(), angle);

        m_caster->GetMotionMaster()->MoveCharge(pos.m_positionX, pos.m_positionY, pos.m_positionZ + unitTarget->GetObjectSize(), 42.0f, m_spellInfo->Id);

        if (m_caster->GetTypeId() == TYPEID_PLAYER)
            m_caster->ToPlayer()->SetFallInformation(0, m_caster->GetPositionZ());
    }

    if (effectHandleMode == SPELL_EFFECT_HANDLE_HIT_TARGET)
    {
        // not all charge effects used in negative spells
        if (m_caster->GetTypeId() == TYPEID_PLAYER)
        {
            m_caster->ToPlayer()->SetFallInformation(0, m_caster->GetPositionZ());

            if (!m_spellInfo->IsPositive())
                m_caster->Attack(unitTarget, true);
        }
    }

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
        m_caster->ToPlayer()->SetFallInformation(0, unitTarget->GetPositionZ());
}

void Spell::EffectChargeDest(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH)
        return;

    if (m_targets.HasDst())
    {
        Position pos;
        destTarget->GetPosition(&pos);

        if (m_caster->GetEntry() != 36609)       // hack fix for Valkyr Shadowguard, ignore first collision
        {
            float angle = m_caster->GetRelativeAngle(pos.GetPositionX(), pos.GetPositionY());
            float dist = m_caster->GetDistance(pos);

            // Custom MoP Script
            // Hack Fix - Collision on charge for Clash
            if (m_spellInfo->Id == 126452)
                dist /= 2;

            m_caster->GetFirstCollisionPosition(pos, dist, angle);
        }

        // Racer Slam Hit Destination
        if (m_spellInfo->Id == 49302)
        {
            if (urand(0, 100) < 20)
            {
                m_caster->CastSpell(m_caster, 49336, false);
                m_caster->CastSpell((Unit*)NULL, 49444, false); // achievement counter
            }
        }

        m_caster->GetMotionMaster()->MoveCharge(pos.m_positionX, pos.m_positionY, pos.m_positionZ, 42.0f, m_spellValue->EffectBasePoints[0] > 0 ? m_spellInfo->Id : 1003);
    }
}

void Spell::EffectKnockBack(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    if (Creature* creatureTarget = unitTarget->ToCreature())
        if (creatureTarget->isWorldBoss() || creatureTarget->IsDungeonBoss())
            return;

    // Spells with SPELL_EFFECT_KNOCK_BACK can't knoback target if target has ROOT
    if (unitTarget->HasUnitState(UNIT_STATE_ROOT))
        return;

    switch (m_spellInfo->Id)
    {
        case 149575: // Explosive Trap
            if (!m_caster->HasAura(119403)) // Glyph of Explosive Trap
                return;
            break;
        case 51490: // Thunderstorm
            if (m_caster->HasAura(62132)) // Glyph of Thunderstorm
                return;
            break;
        default:
            break;
    }

    // Instantly interrupt non melee spells being casted
    if (unitTarget->IsNonMeleeSpellCasted(true))
        unitTarget->InterruptNonMeleeSpells(true);

    float ratio = 0.1f;
    float speedxy = float(m_spellInfo->Effects[effIndex].MiscValue) * ratio;
    float speedz = float(damage) * ratio;
    if (speedxy < 0.1f && speedz < 0.1f)
        return;

    float x, y;
    if (m_spellInfo->Effects[effIndex].Effect == SPELL_EFFECT_KNOCK_BACK_DEST)
    {
        if (m_targets.HasDst())
            destTarget->GetPosition(x, y);
        else
            return;
    }
    else
        m_caster->GetPosition(x, y);

    unitTarget->KnockbackFrom(x, y, speedxy, speedz);

    if (unitTarget->GetTypeId() == TYPEID_PLAYER)
        unitTarget->ToPlayer()->SetKnockBackTime(getMSTime());
}

void Spell::EffectLeapBack(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH_TARGET)
        return;

    if (!unitTarget)
        return;

    float speedxy = float(m_spellInfo->Effects[effIndex].MiscValue) / 10.0f;
    float speedz = float(damage / 10.0f);
    bool back = true;

    switch (m_spellInfo->Id)
    {
    case 56446: // Glyph of Disengage
        if (m_caster->HasAura(56844))
            speedz = (75.0f * 1.5f) / 10.0f;
        break;
    case 102383:// Wild Charge (Moonkin)
    case 140949:// Weak Link (Horridon - Heroic)
        back = false;
        break;
    default:
        break;
    }

    // Disengage
    if (m_spellInfo->SpellIconID == 1891)
        back = false;

    m_caster->JumpTo(speedxy, speedz, back);
}

void Spell::EffectQuestClear(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    Player* player = unitTarget->ToPlayer();

    uint32 quest_id = m_spellInfo->Effects[effIndex].MiscValue;

    Quest const* quest = sObjectMgr->GetQuestTemplate(quest_id);

    if (!quest)
        return;

    // Player has never done this quest
    if (player->GetQuestStatus(quest_id) == QUEST_STATUS_NONE)
        return;

    // remove all quest entries for 'entry' from quest log
    for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
    {
        uint32 logQuest = player->GetQuestSlotQuestId(slot);
        if (logQuest == quest_id)
        {
            player->SetQuestSlot(slot, 0);

            // we ignore unequippable quest items in this case, it's still be equipped
            player->TakeQuestSourceItem(logQuest, false);
        }
    }

    player->RemoveActiveQuest(quest_id);
    player->RemoveRewardedQuest(quest_id);
}

void Spell::EffectSendTaxi(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    unitTarget->ToPlayer()->ActivateTaxiPathTo(m_spellInfo->Effects[effIndex].MiscValue, m_spellInfo->Id);
}

void Spell::EffectPullTowards(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    float speedZ = (float)(m_spellInfo->Effects[effIndex].CalcValue() / 10);
    float speedXY = (float)(m_spellInfo->Effects[effIndex].MiscValue / 10);
    Position pos;
    if (m_spellInfo->Effects[effIndex].Effect == SPELL_EFFECT_PULL_TOWARDS_DEST)
    {
        if (m_targets.HasDst())
            pos.Relocate(*destTarget);
        else
            return;
    }
    else //if (m_spellInfo->Effects[i].Effect == SPELL_EFFECT_PULL_TOWARDS)
    {
        pos.Relocate(m_caster);
    }

    unitTarget->GetMotionMaster()->MoveJump(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), speedXY, speedZ);
}

void Spell::EffectDispelMechanic(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    uint32 mechanic = m_spellInfo->Effects[effIndex].MiscValue;

    std::queue < std::pair < uint32, uint64 > > dispel_list;

    Unit::AuraMap const& auras = unitTarget->GetOwnedAuras();
    for (Unit::AuraMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
    {
        AuraPtr aura = itr->second;
        if (!aura->GetApplicationOfTarget(unitTarget->GetGUID()))
            continue;
        if (roll_chance_i(aura->CalcDispelChance(unitTarget, !unitTarget->IsFriendlyTo(m_caster))))
            if ((aura->GetSpellInfo()->GetAllEffectsMechanicMask() & (1 << mechanic)))
                dispel_list.push(std::make_pair(aura->GetId(), aura->GetCasterGUID()));
    }

    for (; dispel_list.size(); dispel_list.pop())
    {
        unitTarget->RemoveAura(dispel_list.front().first, dispel_list.front().second, 0, AURA_REMOVE_BY_ENEMY_SPELL);
    }
}

void Spell::EffectSummonDeadPet(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    Player* player = m_caster->ToPlayer();
    if (!player)
        return;

    Pet* pet = player->GetPet();
    if (!pet)
        return;

    if (pet->isAlive())
        return;

    if (damage < 0)
        return;

    float x, y, z;
    player->GetPosition(x, y, z);

    player->GetMap()->CreatureRelocation(pet, x, y, z, player->GetOrientation());

    pet->SetUInt32Value(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_NONE);
    pet->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
    pet->setDeathState(ALIVE);
    pet->ClearUnitState(uint32(UNIT_STATE_ALL_STATE));
    pet->SetHealth(pet->CountPctFromMaxHealth(damage));

    //pet->AIM_Initialize();
    //player->PetSpellInitialize();
    pet->SavePetToDB(PET_SLOT_ACTUAL_PET_SLOT, pet->m_Stampeded);
}

void Spell::EffectDestroyAllTotems(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    int32 mana = 0;
    float manaCostPercentage = 0.00f;
    for (uint8 slot = SUMMON_SLOT_TOTEM; slot < MAX_TOTEM_SLOT; ++slot)
    {
        if (!m_caster->m_SummonSlot[slot])
            continue;

        Creature* totem = m_caster->GetMap()->GetCreature(m_caster->m_SummonSlot[slot]);
        if (totem && totem->isTotem())
        {
            uint32 spell_id = totem->GetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL);
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spell_id);
            if (spellInfo)
            {
                manaCostPercentage = spellInfo->ManaCostPercentage;
                mana += m_caster->CountPctFromMaxMana(int32(manaCostPercentage));
            }
            totem->ToTotem()->UnSummon();
        }
    }
    ApplyPct(mana, damage);
    if (mana)
        m_caster->CastCustomSpell(m_caster, 39104, &mana, NULL, NULL, true);
}

void Spell::EffectDurabilityDamage(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    int32 slot = m_spellInfo->Effects[effIndex].MiscValue;

    // FIXME: some spells effects have value -1/-2
    // Possibly its mean -1 all player equipped items and -2 all items
    if (slot < 0)
    {
        unitTarget->ToPlayer()->DurabilityPointsLossAll(damage, (slot < -1));
        return;
    }

    // invalid slot value
    if (slot >= INVENTORY_SLOT_BAG_END)
        return;

    if (Item* item = unitTarget->ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
        unitTarget->ToPlayer()->DurabilityPointsLoss(item, damage);

    ExecuteLogEffectDurabilityDamage(effIndex, unitTarget, slot, damage);
}

void Spell::EffectDurabilityDamagePCT(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    int32 slot = m_spellInfo->Effects[effIndex].MiscValue;

    // FIXME: some spells effects have value -1/-2
    // Possibly its mean -1 all player equipped items and -2 all items
    if (slot < 0)
    {
        unitTarget->ToPlayer()->DurabilityLossAll(float(damage) / 100.0f, (slot < -1));
        return;
    }

    // invalid slot value
    if (slot >= INVENTORY_SLOT_BAG_END)
        return;

    if (damage <= 0)
        return;

    if (Item* item = unitTarget->ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
        unitTarget->ToPlayer()->DurabilityLoss(item, float(damage) / 100.0f);
}

void Spell::EffectModifyThreatPercent(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    unitTarget->getThreatManager().modifyThreatPercent(m_caster, damage);
}

void Spell::EffectTransmitted(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    uint32 name_id = m_spellInfo->Effects[effIndex].MiscValue;

    GameObjectTemplate const* goinfo = sObjectMgr->GetGameObjectTemplate(name_id);

    if (!goinfo)
    {
        sLog->outError(LOG_FILTER_SQL, "Gameobject (Entry: %u) not exist and not created at spell (ID: %u) cast", name_id, m_spellInfo->Id);
        return;
    }

    float fx, fy, fz;

    if (m_targets.HasDst())
        destTarget->GetPosition(fx, fy, fz);
    //FIXME: this can be better check for most objects but still hack
    else if (m_spellInfo->Effects[effIndex].HasRadius() && m_spellInfo->Speed == 0)
    {
        float dis = m_spellInfo->Effects[effIndex].CalcRadius(m_originalCaster);
        m_caster->GetClosePoint(fx, fy, fz, DEFAULT_WORLD_OBJECT_SIZE, dis);
    }
    else
    {
        //GO is always friendly to it's creator, get range for friends
        float min_dis = m_spellInfo->GetMinRange(true);
        float max_dis = m_spellInfo->GetMaxRange(true);
        float dis = (float)rand_norm() * (max_dis - min_dis) + min_dis;

        m_caster->GetClosePoint(fx, fy, fz, DEFAULT_WORLD_OBJECT_SIZE, dis);
    }

    Map* cMap = m_caster->GetMap();
    if (goinfo->type == GAMEOBJECT_TYPE_FISHINGNODE || goinfo->type == GAMEOBJECT_TYPE_FISHINGHOLE)
    {
        LiquidData liqData;
        if (!cMap->IsInWater(fx, fy, fz + 1.f/* -0.5f */, &liqData))             // Hack to prevent fishing bobber from failing to land on fishing hole
        { // but this is not proper, we really need to ignore not materialized objects
            SendCastResult(SPELL_FAILED_NOT_HERE);
            SendChannelUpdate(0);
            return;
        }

        // replace by water level in this case
        //fz = cMap->GetWaterLevel(fx, fy);
        fz = liqData.level;
    }
    // if gameobject is summoning object, it should be spawned right on caster's position
    else if (goinfo->type == GAMEOBJECT_TYPE_RITUAL)
    {
        m_caster->GetPosition(fx, fy, fz);
    }

    GameObject* pGameObj = new GameObject;

    if (!pGameObj->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_GAMEOBJECT), name_id, cMap,
        m_caster->GetPhaseMask(), fx, fy, fz, m_caster->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 100, GO_STATE_READY))
    {
        delete pGameObj;
        return;
    }

    int32 duration = m_spellInfo->GetDuration();

    switch (goinfo->type)
    {
    case GAMEOBJECT_TYPE_FISHINGNODE:
    {
        m_caster->SetGuidValue(UNIT_FIELD_CHANNEL_OBJECT, pGameObj->GetGUID());
        m_caster->AddGameObject(pGameObj);              // will removed at spell cancel

        // end time of range when possible catch fish (FISHING_BOBBER_READY_TIME..GetDuration(m_spellInfo))
        // start time == fish-FISHING_BOBBER_READY_TIME (0..GetDuration(m_spellInfo)-FISHING_BOBBER_READY_TIME)
        int32 lastSec = 0;
        switch (urand(0, 3))
        {
        case 0: lastSec = 3; break;
        case 1: lastSec = 7; break;
        case 2: lastSec = 13; break;
        case 3: lastSec = 17; break;
        }

        duration = duration - lastSec*IN_MILLISECONDS + FISHING_BOBBER_READY_TIME*IN_MILLISECONDS;
        break;
    }
    case GAMEOBJECT_TYPE_RITUAL:
    {
        if (m_caster->GetTypeId() == TYPEID_PLAYER)
        {
            pGameObj->AddUniqueUse(m_caster->ToPlayer());
            m_caster->AddGameObject(pGameObj);      // will be removed at spell cancel
        }
        break;
    }
    case GAMEOBJECT_TYPE_DUEL_ARBITER: // 52991
        m_caster->AddGameObject(pGameObj);
        break;
    case GAMEOBJECT_TYPE_FISHINGHOLE:
    case GAMEOBJECT_TYPE_CHEST:
    default:
        break;
    }

    pGameObj->SetRespawnTime(duration > 0 ? duration / IN_MILLISECONDS : 0);

    pGameObj->SetOwnerGUID(m_caster->GetGUID());

    //pGameObj->SetUInt32Value(GAMEOBJECT_LEVEL, m_caster->getLevel());
    pGameObj->SetSpellId(m_spellInfo->Id);

    ExecuteLogEffectSummonObject(effIndex, pGameObj);
    //m_caster->AddGameObject(pGameObj);
    //m_ObjToDel.push_back(pGameObj);

    cMap->AddToMap(pGameObj);

    if (uint32 linkedEntry = pGameObj->GetGOInfo()->GetLinkedGameObjectEntry())
    {
        GameObject* linkedGO = new GameObject;
        if (linkedGO->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_GAMEOBJECT), linkedEntry, cMap,
            m_caster->GetPhaseMask(), fx, fy, fz, m_caster->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 100, GO_STATE_READY))
        {
            linkedGO->SetRespawnTime(duration > 0 ? duration / IN_MILLISECONDS : 0);
            //linkedGO->SetUInt32Value(GAMEOBJECT_LEVEL, m_caster->getLevel());
            linkedGO->SetSpellId(m_spellInfo->Id);
            linkedGO->SetOwnerGUID(m_caster->GetGUID());

            ExecuteLogEffectSummonObject(effIndex, linkedGO);

            linkedGO->GetMap()->AddToMap(linkedGO);
        }
        else
        {
            delete linkedGO;
            linkedGO = NULL;
            return;
        }
    }
}

void Spell::EffectProspecting(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* p_caster = (Player*)m_caster;
    if (!itemTarget || !(itemTarget->GetTemplate()->Flags & ITEM_PROTO_FLAG_PROSPECTABLE))
        return;

    if (itemTarget->GetCount() < 5)
        return;

    if (sWorld->getBoolConfig(CONFIG_SKILL_PROSPECTING))
    {
        uint32 SkillValue = p_caster->GetPureSkillValue(SKILL_JEWELCRAFTING);
        uint32 reqSkillValue = itemTarget->GetTemplate()->RequiredSkillRank;
        p_caster->UpdateGatherSkill(SKILL_JEWELCRAFTING, SkillValue, reqSkillValue);
    }

    m_caster->ToPlayer()->SendLoot(itemTarget->GetGUID(), LOOT_PROSPECTING);
}

void Spell::EffectMilling(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* p_caster = (Player*)m_caster;
    if (!itemTarget || !(itemTarget->GetTemplate()->Flags & ITEM_PROTO_FLAG_MILLABLE))
        return;

    if (itemTarget->GetCount() < 5)
        return;

    if (sWorld->getBoolConfig(CONFIG_SKILL_MILLING))
    {
        uint32 SkillValue = p_caster->GetPureSkillValue(SKILL_INSCRIPTION);
        uint32 reqSkillValue = itemTarget->GetTemplate()->RequiredSkillRank;
        p_caster->UpdateGatherSkill(SKILL_INSCRIPTION, SkillValue, reqSkillValue);
    }

    m_caster->ToPlayer()->SendLoot(itemTarget->GetGUID(), LOOT_MILLING);
}

void Spell::EffectSkill(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;
}

/* There is currently no need for this effect. We handle it in Battleground.cpp
If we would handle the resurrection here, the spiritguide would instantly disappear as the
player revives, and so we wouldn't see the spirit heal visual effect on the npc.
This is why we use a half sec delay between the visual effect and the resurrection itself */
void Spell::EffectSpiritHeal(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    /*
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
    return;
    if (!unitTarget->IsInWorld())
    return;

    //m_spellInfo->Effects[i].BasePoints; == 99 (percent?)
    //unitTarget->ToPlayer()->setResurrect(m_caster->GetGUID(), unitTarget->GetPositionX(), unitTarget->GetPositionY(), unitTarget->GetPositionZ(), unitTarget->GetMaxHealth(), unitTarget->GetMaxPower(POWER_MANA));
    unitTarget->ToPlayer()->ResurrectPlayer(1.0f);
    unitTarget->ToPlayer()->SpawnCorpseBones();
    */
}

// remove insignia spell effect
void Spell::EffectSkinPlayerCorpse(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if ((m_caster->GetTypeId() != TYPEID_PLAYER) || (unitTarget->GetTypeId() != TYPEID_PLAYER) || (unitTarget->isAlive()))
        return;

    unitTarget->ToPlayer()->RemovedInsignia((Player*)m_caster);
}

void Spell::EffectStealBeneficialBuff(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget == m_caster)                 // can't steal from self
        return;

    // HACK FIX !! @TODO: Find how filter not stealable spells for boss
    if (unitTarget->ToCreature() && unitTarget->ToCreature()->IsDungeonBoss())
        return;

    DispelChargesList steal_list;

    // Create dispel mask by dispel type
    uint32 dispelMask = SpellInfo::GetDispelMask(DispelType(m_spellInfo->Effects[effIndex].MiscValue));
    Unit::AuraMap const& auras = unitTarget->GetOwnedAuras();
    for (Unit::AuraMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
    {
        AuraPtr aura = itr->second;
        AuraApplication * aurApp = aura->GetApplicationOfTarget(unitTarget->GetGUID());
        if (!aurApp)
            continue;

        if ((aura->GetSpellInfo()->GetDispelMask()) & dispelMask)
        {
            // Need check for passive? this
            if (!aurApp->IsPositive() || aura->IsPassive() || aura->GetSpellInfo()->AttributesEx4 & SPELL_ATTR4_NOT_STEALABLE)
                continue;

            // The charges / stack amounts don't count towards the total number of auras that can be dispelled.
            // Ie: A dispel on a target with 5 stacks of Winters Chill and a Polymorph has 1 / (1 + 1) -> 50% chance to dispell
            // Polymorph instead of 1 / (5 + 1) -> 16%.
            bool dispel_charges = aura->GetSpellInfo()->AttributesEx7 & SPELL_ATTR7_DISPEL_CHARGES;
            uint8 charges = dispel_charges ? aura->GetCharges() : aura->GetStackAmount();
            if (charges > 0)
                steal_list.push_back(std::make_pair(aura, charges));
        }
    }

    if (steal_list.empty())
        return;

    // Ok if exist some buffs for dispel try dispel it
    DispelList success_list;

    uint64 l_CasterGUID = m_caster->GetGUID();
    uint64 l_VictimGUID = unitTarget->GetGUID();

    uint32 l_DispelSpellID = m_spellInfo->Id;

    std::vector<uint32> l_FailedSpells;

    // dispel N = damage buffs (or while exist buffs for dispel)
    for (int32 count = 0; count < damage && !steal_list.empty();)
    {
        // Random select buff for dispel
        DispelChargesList::iterator l_DispelChargeIT = steal_list.begin();
        std::advance(l_DispelChargeIT, urand(0, steal_list.size() - 1));

        int32 chance = l_DispelChargeIT->first->CalcDispelChance(unitTarget, !unitTarget->IsFriendlyTo(m_caster));
        // 2.4.3 Patch Notes: "Dispel effects will no longer attempt to remove effects that have 100% dispel resistance."
        if (!chance)
        {
            steal_list.erase(l_DispelChargeIT);
            continue;
        }
        else
        {
            if (roll_chance_i(chance))
            {
                success_list.push_back(std::make_pair(l_DispelChargeIT->first->GetId(), l_DispelChargeIT->first->GetCasterGUID()));
                --l_DispelChargeIT->second;
                if (l_DispelChargeIT->second <= 0)
                    steal_list.erase(l_DispelChargeIT);
            }
            else
            {
                /// Append failed spell id
                l_FailedSpells.push_back(l_DispelChargeIT->first->GetId());
            }
            ++count;
        }
    }

    if (!l_FailedSpells.empty())
    {
        WorldPacket l_SpellFailedPacket(SMSG_DISPEL_FAILED, 2 * (16 + 2) + 4 + (4 * l_FailedSpells.size()));
        l_SpellFailedPacket.appendPackGUID(l_CasterGUID);
        l_SpellFailedPacket.appendPackGUID(l_VictimGUID);
        l_SpellFailedPacket << uint32(l_DispelSpellID);
        l_SpellFailedPacket << uint32(l_FailedSpells.size());

        for (uint32 l_I = 0; l_I < l_FailedSpells.size(); ++l_I)
            l_SpellFailedPacket << uint32(l_FailedSpells[l_I]);

        m_caster->SendMessageToSet(&l_SpellFailedPacket, true);
    }

    if (success_list.empty())
        return;

    // @TODO : add the SMSG_SPELLSTEALLOG
    /*WorldPacket dataSuccess(SMSG_SPELL_DISPELL_LOG, 8 + 8 + 4 + 1 + 4 + success_list.size() * 5);
    // Send packet header
    dataSuccess.append(unitTarget->GetPackGUID());         // Victim GUID
    dataSuccess.append(m_caster->GetPackGUID());           // Caster GUID
    dataSuccess << uint32(success_list.size());            // count
    dataSuccess << uint32(m_spellInfo->Id);                // dispel spell id
    dataSuccess << uint32(0);                              // */

    for (DispelList::iterator itr = success_list.begin(); itr != success_list.end(); ++itr)
    {
        // dataSuccess << uint32(itr->first);              // Spell Id
        // dataSuccess << uint8(0);                        // 0 - dispelled !=0 cleansed
        unitTarget->RemoveAurasDueToSpellBySteal(itr->first, itr->second, m_caster);
    }

    // Glyph of SpellSteal
    if (m_caster->HasAura(115713))
        m_caster->HealBySpell(m_caster, m_spellInfo, m_caster->CountPctFromMaxHealth(5));
}

void Spell::EffectKillCreditPersonal(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    unitTarget->ToPlayer()->KilledMonsterCredit(m_spellInfo->Effects[effIndex].MiscValue, 0);
}

void Spell::EffectKillCredit(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    int32 creatureEntry = m_spellInfo->Effects[effIndex].MiscValue;
    if (!creatureEntry)
    {
        if (m_spellInfo->Id == 42793) // Burn Body
            creatureEntry = 24008; // Fallen Combatant
    }

    if (creatureEntry)
        unitTarget->ToPlayer()->RewardPlayerAndGroupAtEvent(creatureEntry, unitTarget);
}

void Spell::EffectQuestFail(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    unitTarget->ToPlayer()->FailQuest(m_spellInfo->Effects[effIndex].MiscValue);
}

void Spell::EffectQuestStart(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = unitTarget->ToPlayer();
    if (Quest const* qInfo = sObjectMgr->GetQuestTemplate(m_spellInfo->Effects[effIndex].MiscValue))
    {
        if (player->CanTakeQuest(qInfo, false) && player->CanAddQuest(qInfo, false))
        {
            player->AddQuest(qInfo, NULL);
        }
    }
}

void Spell::EffectActivateRune(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = m_caster->ToPlayer();

    if (player->getClass() != CLASS_DEATH_KNIGHT)
        return;

    // needed later
    m_runesState = m_caster->ToPlayer()->GetRunesState();

    uint32 count = std::max<uint32>(damage, 1);
    for (uint32 j = 0; j < MAX_RUNES && count > 0; ++j)
    {
        if (player->GetRuneCooldown(j) && player->GetCurrentRune(j) == RuneType(m_spellInfo->Effects[effIndex].MiscValue))
        {
            if (m_spellInfo->Id == 45529)
                if (player->GetBaseRune(j) != RuneType(m_spellInfo->Effects[effIndex].MiscValueB))
                    continue;

            player->SetRuneCooldown(j, 0);
            player->ResyncRunes(MAX_RUNES);
            --count;
        }
    }

    // Blood Tap
    if (m_spellInfo->Id == 45529 && count > 0)
    {
        std::set<uint8> runes;
        for (uint8 i = 0; i < MAX_RUNES; i++)
            if (player->GetRuneCooldown(i) && player->GetCurrentRune(i) != RUNE_DEATH)
                runes.insert(i);

        // Recalculate list if all runes are RUNE_DEATH
        if (runes.empty())
        {
            for (uint8 i = 0; i < MAX_RUNES; i++)
                if (player->GetRuneCooldown(i) && player->GetCurrentRune(i) != RUNE_DEATH)
                    runes.insert(i);
        }

        if (!runes.empty())
        {
            std::set<uint8>::iterator itr = runes.begin();
            std::advance(itr, urand(0, runes.size() - 1));
            player->AddRuneBySpell((*itr), RUNE_DEATH, m_spellInfo->Id);
            player->SetRuneCooldown((*itr), 0);
            player->ResyncRunes(MAX_RUNES);
        }

        for (uint32 j = 0; j < MAX_RUNES && count > 0; ++j)
        {
            if (player->GetRuneConvertSpell(j) != m_spellInfo->Id)
                continue;

            player->SetRuneCooldown(j, 0);
            player->ResyncRunes(MAX_RUNES);
            --count;
        }
    }

    // Empower rune weapon
    if (m_spellInfo->Id == 47568)
    {
        // Need to do this just once
        if (effIndex != 0)
            return;

        for (uint32 i = 0; i < MAX_RUNES; ++i)
        {
            if (player->GetRuneCooldown(i))
                player->SetRuneCooldown(i, 0);
        }
    }
}

void Spell::EffectCreateTamedPet(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER || unitTarget->GetPetGUID() || unitTarget->getClass() != CLASS_HUNTER)
        return;

    uint32 creatureEntry = m_spellInfo->Effects[effIndex].MiscValue;
    Pet* pet = unitTarget->CreateTamedPetFrom(creatureEntry, m_spellInfo->Id);
    if (!pet)
        return;

    // add to world
    pet->GetMap()->AddToMap(pet->ToCreature());

    // unitTarget has pet now
    unitTarget->SetMinion(pet, true, PET_SLOT_ACTUAL_PET_SLOT, pet->m_Stampeded);

    if (unitTarget->GetTypeId() == TYPEID_PLAYER)
    {
        m_caster->ToPlayer()->m_currentPetSlot = m_caster->ToPlayer()->getSlotForNewPet();
        pet->SavePetToDB(m_caster->ToPlayer()->m_currentPetSlot, pet->m_Stampeded);
        unitTarget->ToPlayer()->PetSpellInitialize();
    }
}

void Spell::EffectDiscoverTaxi(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    uint32 nodeid = m_spellInfo->Effects[effIndex].MiscValue;
    if (sTaxiNodesStore.LookupEntry(nodeid))
        unitTarget->ToPlayer()->GetSession()->SendDiscoverNewTaxiNode(nodeid);
}

void Spell::EffectTitanGrip(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
        m_caster->ToPlayer()->SetCanTitanGrip(true);
}

void Spell::EffectRedirectThreat(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (unitTarget)
        m_caster->SetReducedThreatPercent((uint32)damage, unitTarget->GetGUID());
}

void Spell::EffectGameObjectDamage(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!gameObjTarget)
        return;

    Unit* caster = m_originalCaster;
    if (!caster)
        return;

    FactionTemplateEntry const* casterFaction = caster->getFactionTemplateEntry();
    FactionTemplateEntry const* targetFaction = sFactionTemplateStore.LookupEntry(gameObjTarget->GetUInt32Value(GAMEOBJECT_FIELD_FACTION_TEMPLATE));
    // Do not allow to damage GO's of friendly factions (ie: Wintergrasp Walls/Ulduar Storm Beacons)
    if ((casterFaction && targetFaction && !casterFaction->IsFriendlyTo(*targetFaction)) || !targetFaction)
        gameObjTarget->ModifyHealth(-damage, caster, GetSpellInfo()->Id);
}

void Spell::EffectGameObjectRepair(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!gameObjTarget)
        return;

    gameObjTarget->ModifyHealth(damage, m_caster);
}

void Spell::EffectGameObjectSetDestructionState(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!gameObjTarget || !m_originalCaster)
        return;

    Player* player = m_originalCaster->GetCharmerOrOwnerPlayerOrPlayerItself();
    gameObjTarget->SetDestructibleState(GameObjectDestructibleState(m_spellInfo->Effects[effIndex].MiscValue), player, true);
}

void Spell::SummonGuardian(uint32 i, uint32 entry, SummonPropertiesEntry const* properties, uint32 numGuardians)
{
    Unit* caster = m_originalCaster;
    if (!caster)
        return;

    if (caster->isTotem())
        caster = caster->ToTotem()->GetOwner();

    // in another case summon new
    uint8 level = caster->getLevel();

    // level of pet summoned using engineering item based at engineering skill level
    if (m_CastItem && caster->GetTypeId() == TYPEID_PLAYER)
        if (ItemTemplate const* proto = m_CastItem->GetTemplate())
            if (proto->RequiredSkill == SKILL_ENGINEERING)
                if (uint16 skill202 = caster->ToPlayer()->GetSkillValue(SKILL_ENGINEERING))
                    level = skill202 / 5;

    float radius = 5.0f;
    int32 duration = m_spellInfo->GetDuration();

    switch (m_spellInfo->Id)
    {
    case 81283: // Fungal Growth
        numGuardians = 1;
        break;
    case 49028: // Dancing Rune Weapon
        // 20% Parry
        m_originalCaster->CastSpell(m_originalCaster, 81256, true);
        break;
    default:
        break;
    }

    if (Player* modOwner = m_originalCaster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_DURATION, duration);

    //TempSummonType summonType = (duration == 0) ? TEMPSUMMON_DEAD_DESPAWN : TEMPSUMMON_TIMED_DESPAWN;
    Map* map = caster->GetMap();

    for (uint32 count = 0; count < numGuardians; ++count)
    {
        Position pos;
        if (count == 0)
            pos = *destTarget;
        else
            // randomize position for multiple summons
            m_caster->GetRandomPoint(*destTarget, radius, pos);

        TempSummon* summon = map->SummonCreature(entry, pos, properties, duration, caster, m_spellInfo->Id);
        if (!summon)
            return;

        // summon gargoyle shouldn't be initialized twice
        if (summon->GetEntry() == 27829)
        {
            summon->setFaction(caster->getFaction());
            ExecuteLogEffectSummonObject(i, summon);
            return;
        }

        if (summon->HasUnitTypeMask(UNIT_MASK_GUARDIAN))
            ((Guardian*)summon)->InitStatsForLevel(level);

        if (properties && properties->Category == SUMMON_CATEGORY_ALLY)
            summon->setFaction(caster->getFaction());

        if (summon->HasUnitTypeMask(UNIT_MASK_MINION) && m_targets.HasDst())
            ((Minion*)summon)->SetFollowAngle(m_caster->GetAngle(summon));

        /// Dancing rune weapon summon
        if (summon->GetEntry() == 27893)
        {
            if (uint32 weapon = m_caster->GetUInt32Value(PLAYER_FIELD_VISIBLE_ITEMS + (SLOT_MAIN_HAND * 3)))
            {
                summon->SetDisplayId(11686);
                summon->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, weapon);
            }
            else
                summon->SetDisplayId(1126);
        }

        summon->AI()->EnterEvadeMode();

        ExecuteLogEffectSummonObject(i, summon);
    }
}

void Spell::EffectRenamePet(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT ||
        !unitTarget->ToCreature()->isPet() || ((Pet*)unitTarget)->getPetType() != HUNTER_PET)
        return;

    unitTarget->SetByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, 2, UNIT_CAN_BE_RENAMED);
}

void Spell::EffectPlayMusic(SpellEffIndex p_EffectIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    uint32 l_SoundID = m_spellInfo->Effects[p_EffectIndex].MiscValue;

    if (!sSoundEntriesStore.LookupEntry(l_SoundID))
        return;

    WorldPacket l_Data(SMSG_PLAY_MUSIC, 4);
    l_Data << uint32(l_SoundID);

    unitTarget->ToPlayer()->GetSession()->SendPacket(&l_Data);
}

void Spell::EffectSpecCount(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    unitTarget->ToPlayer()->UpdateSpecCount(damage);
}

void Spell::EffectActivateSpec(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    unitTarget->ToPlayer()->ActivateSpec(damage - 1);  // damage is 1 or 2, spec is 0 or 1
}

void Spell::EffectPlaySound(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    switch (m_spellInfo->Id)
    {
    case 58730: // Restricted Flight Area
    case 58600: // Restricted Flight Area
    case 91604: // Restricted Flight Area
        unitTarget->ToPlayer()->GetSession()->SendNotification(LANG_ZONE_NOFLYZONE);
        break;
    default:
        break;
    }

    uint32 soundId = m_spellInfo->Effects[effIndex].MiscValue;

    if (!sSoundEntriesStore.LookupEntry(soundId))
        return;

    unitTarget->SendPlaySound(soundId, true);
}

void Spell::EffectRemoveAura(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;
    // there may be need of specifying casterguid of removed auras
    unitTarget->RemoveAurasDueToSpell(m_spellInfo->Effects[effIndex].TriggerSpell);
}

void Spell::EffectDamageFromMaxHealthPCT(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    m_damage += unitTarget->CountPctFromMaxHealth(damage);
}

void Spell::EffectGiveCurrency(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    unitTarget->ToPlayer()->ModifyCurrency(m_spellInfo->Effects[effIndex].MiscValue, damage);
}

void Spell::EffectDestroyItem(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = unitTarget->ToPlayer();
    uint32 itemId = m_spellInfo->Effects[effIndex].ItemType;

    if (Item* item = player->GetItemByEntry(itemId))
        player->DestroyItem(item->GetBagSlot(), item->GetSlot(), true);
}

void Spell::EffectCastButtons(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* p_caster = m_caster->ToPlayer();
    uint32 button_id = m_spellInfo->Effects[effIndex].MiscValue + 132;
    uint32 n_buttons = m_spellInfo->Effects[effIndex].MiscValueB;

    for (; n_buttons; --n_buttons, ++button_id)
    {
        ActionButton const* ab = p_caster->GetActionButton(button_id);
        if (!ab || ab->GetType() != ACTION_BUTTON_SPELL)
            continue;

        //! Action button data is unverified when it's set so it can be "hacked"
        //! to contain invalid spells, so filter here.
        uint32 spell_id = ab->GetAction();
        if (!spell_id)
            continue;

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spell_id);
        if (!spellInfo)
            continue;

        if (!p_caster->HasSpell(spell_id) || p_caster->HasSpellCooldown(spell_id))
            continue;

        if (!(spellInfo->AttributesEx7 & SPELL_ATTR7_SUMMON_TOTEM))
            continue;

        int32 cost[MAX_POWERS_COST];
        memset(cost, 0, sizeof(uint32)* MAX_POWERS_COST);
        cost[MAX_POWERS_COST - 1] = 0;
        spellInfo->CalcPowerCost(m_caster, spellInfo->GetSchoolMask(), cost);
        if (m_caster->GetPower(POWER_MANA) < cost[POWER_MANA])
            continue;

        TriggerCastFlags triggerFlags = TriggerCastFlags(TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CAST_IN_PROGRESS | TRIGGERED_CAST_DIRECTLY);
        m_caster->CastSpell(m_caster, spell_id, triggerFlags);
    }
}

void Spell::EffectRechargeManaGem(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = m_caster->ToPlayer();

    if (!player)
        return;

    uint32 item_id = m_spellInfo->Effects[EFFECT_0].ItemType;

    ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(item_id);
    if (!pProto)
    {
        player->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
        return;
    }

    if (Item* pItem = player->GetItemByEntry(item_id))
    {
        for (int x = 0; x < MAX_ITEM_PROTO_SPELLS; ++x)
            pItem->SetSpellCharges(x, pProto->Spells[x].SpellCharges);
        pItem->SetState(ITEM_CHANGED, player);
    }
}

void Spell::EffectBind(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = unitTarget->ToPlayer();

    uint32 area_id;
    WorldLocation loc;
    if (m_spellInfo->Effects[effIndex].TargetA.GetTarget() == TARGET_DEST_DB || m_spellInfo->Effects[effIndex].TargetB.GetTarget() == TARGET_DEST_DB)
    {
        SpellTargetPosition const* st = sSpellMgr->GetSpellTargetPosition(m_spellInfo->Id, effIndex);
        if (!st)
        {
            sLog->outError(LOG_FILTER_SPELLS_AURAS, "Spell::EffectBind - unknown teleport coordinates for spell ID %u", m_spellInfo->Id);
            return;
        }

        loc.m_mapId = st->target_mapId;
        loc.m_positionX = st->target_X;
        loc.m_positionY = st->target_Y;
        loc.m_positionZ = st->target_Z;
        loc.SetOrientation(st->target_Orientation);
        area_id = player->GetAreaId();
    }
    else
    {
        player->GetPosition(&loc);
        loc.m_mapId = player->GetMapId();
        area_id = player->GetAreaId();
    }

    player->SetHomebind(loc, area_id);

    // binding
    WorldPacket l_Data(SMSG_BIND_POINT_UPDATE, (4 + 4 + 4 + 4 + 4));
    l_Data << float(loc.m_positionX);
    l_Data << float(loc.m_positionY);
    l_Data << float(loc.m_positionZ);
    l_Data << uint32(loc.m_mapId);
    l_Data << uint32(area_id);
    player->SendDirectMessage(&l_Data);

    // zone update
    l_Data.Initialize(SMSG_PLAYER_BOUND, 8 + 4);
    l_Data.appendPackGUID(player->GetGUID());
    l_Data << uint32(area_id);
    player->SendDirectMessage(&l_Data);
}

void Spell::EffectSummonRaFFriend(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER || !unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    m_caster->CastSpell(unitTarget, m_spellInfo->Effects[effIndex].TriggerSpell, true);
}

void Spell::EffectUnlearnTalent(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* plr = m_caster->ToPlayer();

    for (auto itr : *plr->GetTalentMap(plr->GetActiveSpec()))
    {
        SpellInfo const* spell = sSpellMgr->GetSpellInfo(itr.first);
        if (!spell)
            continue;

        bool l_Continue = false;
        for (uint32 l_TalentID : spell->m_TalentIDs)
        {
            TalentEntry const* l_TalentEntry = sTalentStore.LookupEntry(l_TalentID);
            if (l_TalentEntry && l_TalentID == m_glyphIndex)
                l_Continue = true;
        }

        if (!l_Continue)
            continue;

        plr->removeSpell(itr.first, true);
        // search for spells that the talent teaches and unlearn them
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            if (spell->Effects[i].TriggerSpell > 0 && spell->Effects[i].Effect == SPELL_EFFECT_LEARN_SPELL)
                plr->removeSpell(spell->Effects[i].TriggerSpell, true);

        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            if (spell->Effects[i].ApplyAuraName == SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS || spell->Effects[i].ApplyAuraName == SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS_2)
                plr->RemoveAurasDueToSpell(spell->Effects[i].BasePoints);

        itr.second->state = PLAYERSPELL_REMOVED;

        plr->SetUsedTalentCount(plr->GetUsedTalentCount() - 1);
        plr->SetFreeTalentPoints(plr->GetFreeTalentPoints() + 1);
        break;
    }

    plr->SaveToDB();
    plr->SendTalentsInfoData(false);
}

void Spell::EffectCreateAreatrigger(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    Position l_Source, l_Dest;
    if (!m_targets.HasDst())
        m_caster->GetPosition(&l_Source);
    else
        destTarget->GetPosition(&l_Source);
    l_Dest = l_Source;
    
    // trigger entry/miscvalue relation is currently unknown, for now use MiscValue as trigger entry
    uint32 l_MiscValue = GetSpellInfo()->Effects[effIndex].MiscValue;

    switch (l_MiscValue)
    {
        case 719:  // Anima Ring
            m_caster->GetPosition(&l_Dest);
            break;
        case 1315: // Chi Burst
        case 1316: // Chi Burst
        case 1612: ///< Arcane Orb
            m_caster->MovePosition(l_Dest, m_spellInfo->GetMaxRange(true, m_caster, this), 0.0f);
            break;
        default:
            break;
    }

    AreaTrigger* l_AreaTrigger = new AreaTrigger;
    if (!l_AreaTrigger->CreateAreaTriggerFromSpell(sObjectMgr->GenerateLowGuid(HIGHGUID_AREATRIGGER), m_originalCaster && m_caster != m_originalCaster ? m_originalCaster : m_caster, GetSpellInfo(), effIndex, l_Source, l_Dest))
        delete l_AreaTrigger;

    // Custom MoP Script
    switch (m_spellInfo->Id)
    {
    case 121536:// Angelic Feather
    {
        int32 count = m_caster->CountAreaTrigger(m_spellInfo->Id);

        if (count > 3)
        {
            std::list<AreaTrigger*> angelicFeatherList;
            m_caster->GetAreaTriggerList(angelicFeatherList, m_spellInfo->Id);

            if (!angelicFeatherList.empty())
            {
                angelicFeatherList.sort(JadeCore::AreaTriggerDurationPctOrderPred());

                for (auto itr : angelicFeatherList)
                {
                    AreaTrigger* angelicFeather = itr;
                    angelicFeather->SetDuration(0);
                    break;
                }
            }
        }

        break;
    }
    case 115460:// Healing Sphere
    {
        int32 count = m_caster->CountAreaTrigger(m_spellInfo->Id);

        if (count > 3)
        {
            std::list<AreaTrigger*> healingSphereList;
            m_caster->GetAreaTriggerList(healingSphereList, m_spellInfo->Id);

            if (!healingSphereList.empty())
            {
                healingSphereList.sort(JadeCore::AreaTriggerDurationPctOrderPred());

                for (auto itr : healingSphereList)
                {
                    AreaTrigger* healingSphere = itr;
                    healingSphere->SetDuration(0);
                    break;
                }
            }
        }

        break;
    }
    case 116011:// Rune of Power
    {
        int32 count = m_caster->CountAreaTrigger(m_spellInfo->Id);

        if (count > 2)
        {
            std::list<AreaTrigger*> runeOfPowerList;
            m_caster->GetAreaTriggerList(runeOfPowerList, m_spellInfo->Id);

            if (!runeOfPowerList.empty())
            {
                runeOfPowerList.sort(JadeCore::AreaTriggerDurationPctOrderPred());

                for (auto itr : runeOfPowerList)
                {
                    AreaTrigger* runeOfPower = itr;
                    runeOfPower->SetDuration(0);
                    break;
                }
            }
        }

        break;
    }
    default:
        break;
    }
}

void Spell::EffectUnlockGuildVaultTab(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    // Safety checks done in Spell::CheckCast
    Player* caster = m_caster->ToPlayer();
    if (Guild* guild = caster->GetGuild())
        guild->HandleBuyBankTab(caster->GetSession(), m_spellInfo->Effects[effIndex].BasePoints - 1); // Bank tabs start at zero internally
}

void Spell::EffectResurrectWithAura(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->IsInWorld())
        return;

    Player* target = unitTarget->ToPlayer();
    if (!target)
        return;
    if (target->isAlive())
        return;

    if (target->IsRessurectRequested())       // already have one active request
        return;

    uint32 health = target->CountPctFromMaxHealth(damage);
    uint32 mana = CalculatePct(target->GetMaxPower(POWER_MANA), damage);

    uint32 resurrectAura = 0;
    if (sSpellMgr->GetSpellInfo(GetSpellInfo()->Effects[effIndex].TriggerSpell))
        resurrectAura = GetSpellInfo()->Effects[effIndex].TriggerSpell;

    if (resurrectAura && target->HasAura(resurrectAura))
        return;

    ExecuteLogEffectResurrect(effIndex, target);
    target->SetResurrectRequestData(m_caster, health, mana, resurrectAura);
    SendResurrectRequest(target);
}

void Spell::EffectTeleportToDigsite(SpellEffIndex p_EffectIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->IsInWorld())
        return;

    Player* l_Target = unitTarget->ToPlayer();
    if (!l_Target)
        return;

    if (!l_Target->isAlive())
        return;

    std::vector<uint32> l_Maps;

    AreaGroupEntry const* l_AreaGroupEntry = sAreaGroupStore.LookupEntry(m_spellInfo->AreaGroupId);
    while (l_AreaGroupEntry)
    {
        for (uint8 l_I = 0; l_I < MAX_GROUP_AREA_IDS; ++l_I)
        {
            uint32 l_AreaId = l_AreaGroupEntry->AreaId[l_I];
            AreaTableEntry const* l_AreaTable = sAreaStore.LookupEntry(l_AreaId);
            if (l_AreaTable == nullptr)
                continue;

            l_Maps.push_back(l_AreaTable->ContinentID);

        }
        // Try search in next group
        l_AreaGroupEntry = sAreaGroupStore.LookupEntry(l_AreaGroupEntry->nextGroup);
    }

    if (l_Maps.empty())
        return;

    std::random_shuffle(l_Maps.begin(), l_Maps.end());

    uint16  l_SiteId = l_Target->GetArchaeologyMgr().GetRandomActiveSiteInMap(l_Maps[0]);
    ResearchLootVector const& l_Loot = sObjectMgr->GetResearchLoot();
    if (l_Loot.empty())
        return;

    ResearchLootVector l_LootListTemp;
    ResearchLootVector l_LootList;

    for (ResearchLootVector::const_iterator l_Iterator = l_Loot.begin(); l_Iterator != l_Loot.end(); ++l_Iterator)
    {
        ResearchLootEntry entry = (*l_Iterator);
        if (entry.id == l_SiteId)
            l_LootListTemp.push_back(entry);
    }

    if (l_LootListTemp.empty())
        return;

    l_LootList.push_back(JadeCore::Containers::SelectRandomContainerElement(l_LootListTemp));
    if (l_LootList.empty())
        return;

    l_Target->NearTeleportTo(l_LootList[0].x, l_LootList[0].y, l_LootList[0].z, l_Target->GetOrientation());
}

void Spell::EffectRandomizeArchaeologyDigsites(SpellEffIndex p_EffIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->IsInWorld())
        return;

    Player* l_Target = unitTarget->ToPlayer();
    if (!l_Target)
        return;

    if (!l_Target->isAlive())
        return;

    uint32 l_MapId = m_spellInfo->Effects[p_EffIndex].MiscValue;
    uint32 l_SiteCount = m_spellInfo->Effects[p_EffIndex].BasePoints;

    l_Target->GetArchaeologyMgr().GenerateResearchSitesForMap(l_MapId, l_SiteCount);
}

void Spell::EffectLootBonus(SpellEffIndex p_EffIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->IsInWorld() || p_EffIndex != EFFECT_0)
        return;

    Player* l_Player = unitTarget->ToPlayer();
    if (!l_Player)
        return;

    Unit* l_Caster = NULL;
    Unit::AuraEffectList const& l_AuraList = l_Player->GetAuraEffectsByType(SPELL_AURA_TRIGGER_BONUS_LOOT);
    if (!l_AuraList.empty())
    {
        for (Unit::AuraEffectList::const_iterator l_Itr = l_AuraList.begin(); l_Itr != l_AuraList.end(); ++l_Itr)
        {
            if (AuraPtr l_Aura = (*l_Itr)->GetBase())
                l_Caster = l_Aura->GetCaster();
        }
    }

    bool l_IsBGReward = false;

    if (!l_Caster)
    {
        if (Map* l_Map = GetCaster()->GetMap())
        {
            if (l_Map->IsBattlegroundOrArena() || l_Map->GetId() == 1191)   ///< Ashran
            {
                l_IsBGReward = true;
                l_Caster = GetCaster();
            }
        }
    }
    
    if (!l_IsBGReward && (!l_Caster || !l_Caster->ToCreature()))
        return;
        
    LootStore& l_LootStore = l_IsBGReward ? LootTemplates_Spell : LootTemplates_Creature;
    LootTemplate const* l_LootTemplate = l_LootStore.GetLootFor(l_IsBGReward ? GetSpellInfo()->Id : l_Caster->ToCreature()->GetCreatureTemplate()->lootid);
    if (l_LootTemplate == nullptr)
        return;
        
    std::list<ItemTemplate const*> l_LootTable;
    std::vector<uint32> l_Items;
    l_LootTemplate->FillAutoAssignationLoot(l_LootTable, l_Player, l_IsBGReward);

    float l_DropChance = l_IsBGReward ? 100 : sWorld->getFloatConfig(CONFIG_LFR_DROP_CHANCE) + l_Player->GetBonusRollFails();
    uint32 l_SpecID = l_Player->GetLootSpecId() ? l_Player->GetLootSpecId() : l_Player->GetSpecializationId(l_Player->GetActiveSpec());

    if (l_IsBGReward)
    {
        for (ItemTemplate const* l_Template : l_LootTable)
        {
            if (l_Player->CanUseItem(l_Template) == EQUIP_ERR_OK)
                l_Items.push_back(l_Template->ItemId);
        }
    }
    else for (ItemTemplate const* l_Template : l_LootTable)
    {
        for (SpecIndex l_ItemSpecID : l_Template->specs)
        {
            if (l_ItemSpecID == l_SpecID)
                l_Items.push_back(l_Template->ItemId);
        }
    }

    l_Player->RemoveAurasByType(SPELL_AURA_TRIGGER_BONUS_LOOT);
    
    if (l_Items.empty() && !l_IsBGReward)
    {
        int64 l_GoldAmount = urand(50 * GOLD, 100 * GOLD);
        l_Player->IncreaseBonusRollFails();
        l_Player->ModifyMoney(l_GoldAmount);
        l_Player->SendDisplayToast(0, l_GoldAmount, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_MONEY, true, false);

        WorldPacket l_Data(SMSG_LOOT_MONEY_NOTIFY, 4 + 1);
        l_Data << uint32(l_GoldAmount);
        l_Data.WriteBit(1);   // "You loot..."
        l_Data.FlushBits();
        l_Player->GetSession()->SendPacket(&l_Data);
    }
    else
    {
        std::random_shuffle(l_Items.begin(), l_Items.end());

        if (roll_chance_i(l_DropChance) && !l_Items.empty())
        {
            l_Player->AddItem(l_Items[0], 1);
            l_Player->SendDisplayToast(l_Items[0], 1, l_IsBGReward ? DISPLAY_TOAST_METHOD_PVP_FACTION_LOOT_TOAST : DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false);

            if (!l_IsBGReward)
                l_Player->ResetBonusRollFails();
        }
        else if (!l_IsBGReward)
        {
            int64 l_GoldAmount = urand(50 * GOLD, 100 * GOLD);
            l_Player->IncreaseBonusRollFails();
            l_Player->ModifyMoney(l_GoldAmount);
            l_Player->SendDisplayToast(0, l_GoldAmount, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_MONEY, true, false);

            WorldPacket l_Data(SMSG_LOOT_MONEY_NOTIFY, 4 + 1);
            l_Data << uint32(l_GoldAmount);
            l_Data.WriteBit(1);   // "You loot..."
            l_Data.FlushBits();
            l_Player->GetSession()->SendPacket(&l_Data);
        }
    }

    l_Player->ModifyCurrency(m_spellInfo->CurrencyID, -int32(m_spellInfo->CurrencyCount), false);
}

void Spell::EffectDeathGrip(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH)
        return;

    if (m_caster->isInFlight())
        return;

    if (!m_targets.HasDst())
        return;

    // Init dest coordinates
    float x, y, z;
    destTarget->GetPosition(x, y, z);

    float speedXY, speedZ;
    CalculateJumpSpeeds(effIndex, m_caster->GetExactDist2d(x, y), speedXY, speedZ);

    m_caster->GetMotionMaster()->CustomJump(x, y, z, speedXY, speedZ);
}

void Spell::EffectPlaySceneObject(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (!unitTarget || !unitTarget->IsInWorld())
        return;

    Player* target = unitTarget->ToPlayer();
    if (!target)
        return;

    uint32 sceneId = m_spellInfo->Effects[effIndex].MiscValue;
    target->PlayScene(sceneId, target);
}

void Spell::EffectLearnBluePrint(SpellEffIndex p_EffIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->IsInWorld())
        return;

    Player* l_Player = unitTarget->ToPlayer();

    if (!l_Player)
        return;

    if (!l_Player->GetGarrison())
        return;

    if (l_Player->GetGarrison()->LearnBlueprint(m_spellInfo->Effects[p_EffIndex].MiscValue))
    {
        if (m_CastItem)
        {
            uint64 l_PlayerGUID = m_CastItem->GetOwnerGUID();
            uint64 l_ItemGUID   = m_CastItem->GetGUID();

            l_Player->AddCriticalOperation([l_PlayerGUID, l_ItemGUID]() -> void
            {
                if (Player * l_Player = sObjectAccessor->FindPlayer(l_PlayerGUID))
                {
                    uint32 l_DestroyCount = 1;

                    if (Item * l_Item = l_Player->GetItemByGuid(l_ItemGUID))
                        l_Player->DestroyItemCount(l_Item, l_DestroyCount, true);
                }
            });

            m_CastItem = nullptr;
        }
    }
    else
        SendCastResult(SPELL_FAILED_BLUEPRINT_KNOWN);
}

void Spell::EffectObtainFollower(SpellEffIndex p_EffIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->IsInWorld())
        return;

    Player* l_Player = unitTarget->ToPlayer();

    if (!l_Player)
        return;

    if (!l_Player->GetGarrison())
        return;

    if (l_Player->GetGarrison()->AddFollower(m_spellInfo->Effects[p_EffIndex].MiscValue))
    {
        if (m_CastItem)
        {
            uint64 l_PlayerGUID = m_CastItem->GetOwnerGUID();
            uint64 l_ItemGUID   = m_CastItem->GetGUID();

            l_Player->AddCriticalOperation([l_PlayerGUID, l_ItemGUID]() -> void
            {
                if (Player * l_Player = sObjectAccessor->FindPlayer(l_PlayerGUID))
                {
                    uint32 l_DestroyCount = 1;

                    if (Item * l_Item = l_Player->GetItemByGuid(l_ItemGUID))
                        l_Player->DestroyItemCount(l_Item, l_DestroyCount, true);
                }
            });

            m_CastItem = nullptr;
        }
    }
    else
        SendCastResult(SPELL_FAILED_FOLLOWER_KNOWN);
}

void Spell::EffectUpgradeFolloweriLvl(SpellEffIndex p_EffIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!m_CastItem || !unitTarget || !unitTarget->IsInWorld())
        return;

    Player * l_Player = unitTarget->ToPlayer();

    if (!l_Player || !l_Player->GetGarrison())
        return;

    bool l_IsArmor = !!m_spellInfo->Effects[p_EffIndex].MiscValue;
    uint32 l_FollowerEntry = m_glyphIndex;
    uint32 l_NewValue = 0;

    printf("Follower entry %u \n", m_glyphIndex);
    if (m_spellInfo->Effects[EFFECT_1].Effect == SPELL_EFFECT_DUMMY)
    {
        l_NewValue = m_spellInfo->Effects[EFFECT_1].BasePoints;
    }
    else
    {
        if (l_IsArmor)
            l_NewValue = /*OldArmorValue*/0 + m_spellInfo->Effects[p_EffIndex].BasePoints;
        else
            l_NewValue = /*OldWeponValue*/0 + m_spellInfo->Effects[p_EffIndex].BasePoints;
    }

    uint32 l_Value = m_spellInfo->Effects[p_EffIndex].MiscValueB;

    /// Armor case
    if (l_IsArmor)
    {

    }
    /// Weapon case
    else
    {

    }
}

void Spell::EffectGarrisonFinalize(SpellEffIndex p_EffIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->IsInWorld())
        return;

    Player* l_Player = unitTarget->ToPlayer();

    if (!l_Player)
        return;

    if (!l_Player->GetGarrison())
        return;

    l_Player->ToPlayer()->GetGarrison()->ActivateBuilding();
}

void Spell::EffectResurectPetBattles(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!m_CastItem && m_caster->ToPlayer())
    {
        std::vector<BattlePet::Ptr> l_Pets = m_caster->ToPlayer()->GetBattlePets();

        for (std::vector<BattlePet::Ptr>::iterator l_It = l_Pets.begin(); l_It != l_Pets.end(); ++l_It)
        {
            BattlePet::Ptr l_Pet = (*l_It);

            l_Pet->UpdateStats();
            l_Pet->Health = l_Pet->InfoMaxHealth;
        }

        m_caster->ToPlayer()->GetSession()->SendPetBattleJournal();
    }
}

void Spell::EffectUncagePetBattle(SpellEffIndex effIndex)
{

}

void Spell::EffectCanPetBattle(SpellEffIndex effIndex)
{
    if (!unitTarget)
        return;

    Player* player = unitTarget->ToPlayer();
    if (!player)
        return;

    player->GetSession()->SendPetBattleJournalBattleSlotUpdate();
}

void Spell::EffectThreatAll(SpellEffIndex p_EffIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH)
        return;

    std::list<HostileReference*> l_ThreadList = m_caster->getThreatManager().getThreatList();
    for (HostileReference* l_Itr : l_ThreadList)
    {
        Unit* l_Target = l_Itr->getTarget();

        l_Target->getThreatManager().clearReferences();
        l_Target->getThreatManager().doAddThreat(m_caster, m_spellInfo->Effects[p_EffIndex].BasePoints);
    }
}

void Spell::EffectForcePlayerInteraction(SpellEffIndex p_EffIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH)
        return;

    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(m_spellInfo->Effects[p_EffIndex].TriggerSpell, m_caster->GetMap()->GetDifficulty());
    if (l_SpellInfo == nullptr)
        return;

    Unit* l_Target = unitTarget;
    if (l_Target == nullptr)
        l_Target = m_caster;

    m_caster->CastSpell(l_Target, l_SpellInfo->Id, true);
}

void Spell::EffectBecomeUntargettable(SpellEffIndex p_EffIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    WorldPacket l_Data(SMSG_CLEAR_TARGET, 16 + 2);
    l_Data.appendPackGUID(m_caster->GetGUID());
    m_caster->SendMessageToSet(&l_Data, true);
}

void Spell::EffectDespawnAreaTrigger(SpellEffIndex p_EffIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    std::list<AreaTrigger*> l_AreaTriggers;

    if (m_AreaTrigger != nullptr)
        l_AreaTriggers.push_back(m_AreaTrigger);
    else
    {
        float l_Radius = m_spellInfo->Effects[p_EffIndex].CalcRadius(m_caster, this);

        CellCoord l_Pair(JadeCore::ComputeCellCoord(m_caster->GetPositionX(), m_caster->GetPositionY()));
        Cell l_Cell(l_Pair);
        l_Cell.SetNoCreate();

        JadeCore::AllAreaTriggersInRangeCheck l_Check(m_caster, l_Radius);
        JadeCore::AreaTriggerListSearcher<JadeCore::AllAreaTriggersInRangeCheck> l_Searcher(m_caster, l_AreaTriggers, l_Check);
        TypeContainerVisitor<JadeCore::AreaTriggerListSearcher<JadeCore::AllAreaTriggersInRangeCheck>, GridTypeMapContainer> l_Visitor(l_Searcher);

        l_Cell.Visit(l_Pair, l_Visitor, *m_caster->GetMap(), *m_caster, l_Radius);
    }

    for (AreaTrigger* l_AreaTrigger : l_AreaTriggers)
        l_AreaTrigger->Remove(0);
}

void Spell::EffectStampede(SpellEffIndex p_EffIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* l_Player = m_caster->ToPlayer();
    Unit*   l_Target = unitTarget;

    if (l_Target == nullptr)
        return;

    uint32 l_MalusSpell = m_spellInfo->Effects[p_EffIndex].TriggerSpell;
    bool   l_OnlyCurrentPet = l_Player->HasAuraType(SPELL_AURA_STAMPEDE_ONLY_CURRENT_PET);

    uint32 l_CurrentSlot = l_Player->m_currentPetSlot;
    for (uint32 l_PetSlotIndex = uint32(PET_SLOT_HUNTER_FIRST); l_PetSlotIndex <= uint32(PET_SLOT_HUNTER_LAST); ++l_PetSlotIndex)
    {
        if (l_PetSlotIndex != l_CurrentSlot)
        {
            float l_X, l_Y, l_Z;
            l_Player->GetClosePoint(l_X, l_Y, l_Z, l_Player->GetObjectSize());
            Pet* l_Pet = l_Player->SummonPet(0, l_X, l_Y, l_Z, l_Player->GetOrientation(), SUMMON_PET, l_Player->CalcSpellDuration(GetSpellInfo()), PetSlot(l_OnlyCurrentPet ? l_CurrentSlot : l_PetSlotIndex), true);
            if (!l_Pet)
                return;

            if (!l_Pet->isAlive())
                l_Pet->setDeathState(ALIVE);

            /// Set pet at full health
            l_Pet->SetHealth(l_Pet->GetMaxHealth());
            l_Pet->SetReactState(REACT_HELPER);
            l_Pet->m_Stampeded = true;

            std::list<uint32> l_SpellsToRemove;
            for (auto l_Iter : l_Pet->m_spells)
                l_SpellsToRemove.push_back(l_Iter.first);

            /// Summoned pets with Stamped don't use abilities
            for (uint32 l_ID : l_SpellsToRemove)
            {
                auto l_Iter = l_Pet->m_spells.find(l_ID);
                l_Pet->m_spells.erase(l_Iter);
            }

            l_Pet->m_autospells.clear();
            l_Pet->m_Events.KillAllEvents(true);    ///< Disable automatic cast spells

            l_Pet->SetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL, GetSpellInfo()->Id);
            if (l_MalusSpell != 0 && l_Player->GetBattleground())
                l_Pet->CastSpell(l_Pet, l_MalusSpell, true);
            l_Pet->AI()->AttackStart(l_Target);
        }
    }
}

void Spell::EffectCreateHeirloom(SpellEffIndex p_EffIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    Player* l_Player = GetCaster()->ToPlayer();
    HeirloomEntry const* l_HeirloomEntry = GetHeirloomEntryByItemID(m_glyphIndex);

    if (!l_Player || !l_HeirloomEntry)
        return;

    if (l_HeirloomEntry->ItemID != m_glyphIndex)
        return;

    ItemPosCountVec l_Destination;
    InventoryResult l_Result = l_Player->CanStoreNewItem(NULL_BAG, NULL_SLOT, l_Destination, l_HeirloomEntry->ItemID, 1);

    if (l_Result != EQUIP_ERR_OK)
    {
        l_Player->SendEquipError(l_Result, NULL, NULL, l_HeirloomEntry->ItemID);
        return;
    }

    uint32 l_UpgradeLevel = l_Player->GetHeirloomUpgradeLevel(l_HeirloomEntry);
    uint32 l_UpgradeId = 0;

    if (l_UpgradeLevel)
        l_UpgradeId = l_UpgradeLevel <= MAX_HEIRLOOM_UPGRADE_LEVEL ? l_HeirloomEntry->UpgradeIemBonusID[l_UpgradeLevel - 1] : 0;

    std::vector<uint32> l_Upgrades;

    if (l_UpgradeId)
        l_Upgrades.push_back(l_UpgradeId);

    Item* l_Item = l_Player->StoreNewItem(l_Destination, l_HeirloomEntry->ItemID, true);
    l_Item->AddItemBonus(l_UpgradeId);
    l_Player->SendNewItem(l_Item, l_HeirloomEntry->ItemID, false, false, false, l_Upgrades);
}

void Spell::EffectUpgradeHeirloom(SpellEffIndex p_EffIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    Player* l_Player = GetCaster()->ToPlayer();
    HeirloomEntry const* l_HeirloomEntry = GetHeirloomEntryByItemID(m_glyphIndex);

    if (!l_Player || !l_HeirloomEntry || !m_CastItem)
        return;

    if (l_HeirloomEntry->ItemID != m_glyphIndex)
        return;

    if (!l_Player->HasHeirloom(l_HeirloomEntry))
        return;

    if (!l_Player->CanUpgradeHeirloomWith(l_HeirloomEntry, m_CastItem->GetTemplate()->ItemId))
        return;

    uint32 l_UpgradeFlags = (1 << (l_Player->GetHeirloomUpgradeLevel(l_HeirloomEntry) + 1)) - 1;

    PreparedStatement* l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_UPD_HEILOOM_FLAGS);
    l_Statement->setUInt32(0, l_UpgradeFlags);
    l_Statement->setUInt32(1, l_Player->GetSession()->GetAccountId());
    l_Statement->setUInt32(2, l_HeirloomEntry->ID);
    LoginDatabase.Execute(l_Statement);

    l_Player->RemoveItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), true);

    std::vector<uint32> const& l_Heirlooms = l_Player->GetDynamicValues(PLAYER_DYNAMIC_FIELD_HEIRLOOMS);
    for (uint32 l_I = 0; l_I < l_Heirlooms.size(); ++l_I)
    {
        if (l_Heirlooms[l_I] == l_HeirloomEntry->ItemID)
        {
            l_Player->SetDynamicValue(PLAYER_DYNAMIC_FIELD_HEIRLOOMS_FLAGS, l_I, l_UpgradeFlags);
            break;
        }
    }
}
