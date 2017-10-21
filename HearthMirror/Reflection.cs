using System;
using System.Collections.Generic;
using System.Linq;
using HearthMirror.Enums;
using HearthMirror.Objects;
using HearthMirror.Util;

namespace HearthMirror
{
	public class Reflection
	{
		private static readonly Lazy<Mirror> LazyMirror = new Lazy<Mirror>(() => new Mirror {ImageName = "Hearthstone"});
		private static Mirror Mirror => LazyMirror.Value;

		private static T TryGetInternal<T>(Func<T> action, bool clearCache = true)
		{
			try
			{
				if(Mirror.Proc.HasExited)
					Mirror.Clean();
				if(clearCache)
					Mirror.View?.ClearCache();
				return action.Invoke();
			}
			catch
			{
				Mirror.Clean();
				try
				{
					return action.Invoke();
				}
				catch
				{
					return default(T);
				}
			}
		}

		public static List<Card> GetCollection() => TryGetInternal(() => GetCollectionInternal().ToList());

		private static IEnumerable<Card> GetCollectionInternal()
		{
			var values = Mirror.Root["NetCache"]["s_instance"]["m_netCache"]["valueSlots"];
			foreach(var val in values)
			{
				if(val == null || val.Class.Name != "NetCacheCollection") continue;
				var stacks = val["<Stacks>k__BackingField"];
				var items = stacks["_items"];
				int size = stacks["_size"];
				for(var i = 0; i < size; i++)
				{
					var stack = items[i];
					int count = stack["<Count>k__BackingField"];
					var def = stack["<Def>k__BackingField"];
					string name = def["<Name>k__BackingField"];
					int premium = def["<Premium>k__BackingField"];
					yield return new Card(name, count, premium > 0);
				}
			}
		}

		public static List<Deck> GetDecks() => TryGetInternal(() => InternalGetDecks().ToList());

		private static IEnumerable<Deck> InternalGetDecks()
		{
			var values = Mirror.Root["CollectionManager"]["s_instance"]?["m_decks"]?["valueSlots"];
			if(values == null)
				yield break; 
			foreach(var val in values)
			{
				if(val == null || val.Class.Name != "CollectionDeck")
					continue;
				var deck = GetDeck(val);
				if(deck != null)
					yield return deck;
			}
		}

		public static GameServerInfo GetServerInfo() => TryGetInternal(InternalGetServerInfo);
		private static GameServerInfo InternalGetServerInfo()
		{
			var serverInfo = Mirror.Root["Network"]["s_instance"]["m_lastGameServerInfo"];
			if(serverInfo == null)
				return null;
			return new GameServerInfo
			{
				Address = serverInfo["<Address>k__BackingField"],
				AuroraPassword = serverInfo["<AuroraPassword>k__BackingField"],
				ClientHandle = serverInfo["<ClientHandle>k__BackingField"],
				GameHandle = serverInfo["<GameHandle>k__BackingField"],
				Mission = serverInfo["<Mission>k__BackingField"],
				Port = serverInfo["<Port>k__BackingField"],
				Resumable = serverInfo["<Resumable>k__BackingField"],
				SpectatorMode = serverInfo["<SpectatorMode>k__BackingField"],
				SpectatorPassword = serverInfo["<SpectatorPassword>k__BackingField"],
				Version = serverInfo["<Version>k__BackingField"],
			};
		}

		public static int GetGameType() => TryGetInternal(InternalGetGameType);
		private static int InternalGetGameType() => (int) Mirror.Root["GameMgr"]["s_instance"]["m_gameType"];

		public static bool IsSpectating() => TryGetInternal(() => Mirror.Root["GameMgr"]?["s_instance"]?["m_spectator"]) ?? false;

		public static long GetSelectedDeckInMenu() => TryGetInternal(() => (long)(Mirror.Root["DeckPickerTrayDisplay"]["s_instance"]?["m_selectedCustomDeckBox"]?["m_deckID"] ?? 0));

		public static MedalInfo GetMedalInfo() => TryGetInternal(GetMedalInfoInternal);
		private static MedalInfo GetMedalInfoInternal()
		{
			var netCacheValues = Mirror.Root["NetCache"]["s_instance"]?["m_netCache"]?["valueSlots"];
			if(netCacheValues == null)
				return null;
			dynamic netCacheMedalInfo = null;
			foreach(var netCache in netCacheValues)
			{
				if(netCache?.Class.Name != "NetCacheMedalInfo")
					continue;
				netCacheMedalInfo = netCache;
				break;
			}
			if(netCacheMedalInfo == null)
				return null;
			var standard = netCacheMedalInfo["<Standard>k__BackingField"];
			var wild = netCacheMedalInfo["<Wild>k__BackingField"];
			MedalInfoData GetMedalInfoData(dynamic source)
			{
				if(source == null)
					return null;
				return new MedalInfoData
				{
					BestStarLevel = source["_BestStarLevel"],
					CanLoseLevel = source["<CanLoseLevel>k__BackingField"],
					CanLoseStars = source["_CanLoseStars"],
					LegendRank = source["_LegendRank"],
					LevelEnd = source["<LevelEnd>k__BackingField"],
					LevelStart = source["<LevelStart>k__BackingField"],
					SeasonWins = source["<SeasonWins>k__BackingField"],
					StarLevel = source["<StarLevel>k__BackingField"],
					Stars = source["<Stars>k__BackingField"],
					Streak = source["<Streak>k__BackingField"]
				};
			}
			return new MedalInfo
			{
				Standard = GetMedalInfoData(standard),
				Wild = GetMedalInfoData(wild),
			};
		}

		public static MatchInfo GetMatchInfo() => TryGetInternal(GetMatchInfoInternal);
		private static MatchInfo GetMatchInfoInternal()
		{
			var matchInfo = new MatchInfo();
			var gameState = Mirror.Root["GameState"]["s_instance"];
			var netCacheValues = Mirror.Root["NetCache"]["s_instance"]?["m_netCache"]?["valueSlots"];
			if(gameState != null)
			{
				var playerIds = gameState["m_playerMap"]["keySlots"];
				var players = gameState["m_playerMap"]["valueSlots"];
				for(var i = 0; i < playerIds.Length; i++)
				{
					if(players[i]?.Class.Name != "Player")
						continue;
					var medalInfo = players[i]["m_medalInfo"];
					var sMedalInfo = medalInfo?["m_currMedalInfo"];
					var wMedalInfo = medalInfo?["m_currWildMedalInfo"];
					var name = players[i]["m_name"];
					var sRank = sMedalInfo?["rank"] ?? 0;
					var sLegendRank = sMedalInfo?["legendIndex"] ?? 0;
					var wRank = wMedalInfo?["rank"] ?? 0;
					var wLegendRank = wMedalInfo?["legendIndex"] ?? 0;
					var cardBack = players[i]["m_cardBackId"];
					var id = playerIds[i];
					var side = (Side)players[i]["m_side"];
					var account = players[i]["m_gameAccountId"];
					var accountId = new AccountId {Hi = account?["m_hi"] ?? 0, Lo = account?["m_lo"] ?? 0};
					if(side == Side.FRIENDLY)
					{
						dynamic netCacheMedalInfo = null;
						if(netCacheValues != null)
						{
							foreach(var netCache in netCacheValues)
							{
								if(netCache?.Class.Name != "NetCacheMedalInfo")
									continue;
								netCacheMedalInfo = netCache;
								break;
							}
						}
						var sStars = netCacheMedalInfo?["<Standard>k__BackingField"]["<Stars>k__BackingField"];
						var wStars = netCacheMedalInfo?["<Wild>k__BackingField"]["<Stars>k__BackingField"];
						matchInfo.LocalPlayer = new Player(id, name, sRank, sLegendRank, sStars, wRank, wLegendRank, wStars, cardBack, accountId);
					}
					else if (side == Side.OPPOSING)
						matchInfo.OpposingPlayer = new Player(id, name, sRank, sLegendRank, 0, wRank, wLegendRank, 0, cardBack, accountId);
				}
			}
			var gameMgr = Mirror.Root["GameMgr"]["s_instance"];
			if(gameMgr != null)
			{
				matchInfo.MissionId = gameMgr["m_missionId"];
				matchInfo.GameType = gameMgr["m_gameType"];
				matchInfo.FormatType = gameMgr["m_formatType"];

				var brawlGameTypes = new[] {16, 17, 18};
				if(brawlGameTypes.Contains(matchInfo.GameType))
				{
					var mission = GetCurrentBrawlMission();
					matchInfo.BrawlSeasonId = mission?["tavernBrawlSpec"]?["<SeasonId>k__BackingField"];
				}
			}
			if(netCacheValues != null)
			{
				foreach(var netCache in netCacheValues)
				{
					if(netCache?.Class.Name != "NetCacheRewardProgress")
						continue;
					matchInfo.RankedSeasonId = netCache["<Season>k__BackingField"];
					break;
				}
			}
			return matchInfo;
		}

		private static dynamic GetCurrentBrawlMission()
		{
			var missions = Mirror.Root["TavernBrawlManager"]["s_instance"]?["m_missions"];
			if(missions == null) 
				return null;
			foreach(var mission in missions)
			{
				if(mission?.Class.Name != "TavernBrawlMission") 
					continue;
				return mission;
			}
			return null;
		}

		public static ArenaInfo GetArenaDeck() => TryGetInternal(GetArenaDeckInternal);

		private static ArenaInfo GetArenaDeckInternal()
		{
			var draftManager = Mirror.Root["DraftManager"]["s_instance"];
			var deck = GetDeck(draftManager["m_draftDeck"]);
			if(deck == null)
				return null;
			return new ArenaInfo {
				Wins = draftManager["m_wins"],
				Losses = draftManager["m_losses"],
				CurrentSlot = draftManager["m_currentSlot"],
				Deck = deck,
				Rewards = RewardDataParser.Parse(draftManager["m_chest"]?["<Rewards>k__BackingField"]?["_items"])
			};
		}

		public static List<Card> GetArenaDraftChoices() => TryGetInternal(() => GetArenaDraftChoicesInternal().ToList());

		private static IEnumerable<Card> GetArenaDraftChoicesInternal()
		{
			var choicesList =  Mirror.Root["DraftDisplay"]["s_instance"]["m_choices"];
			var choices = choicesList["_items"];
			int size = choicesList["_size"];
			for(var i = 0; i < size; i++)
			{
				if(choices[i] != null)
					yield return new Card(choices[i]["m_actor"]["m_entityDef"]["m_cardId"], 1, false);
			}
		}

		private static Deck GetDeck(dynamic deckObj)
		{
			if(deckObj == null)
				return null;
			var deck = new Deck
			{
				Id = deckObj["ID"],
				Name = deckObj["m_name"],
				Hero = deckObj["HeroCardID"],
				IsWild = deckObj["m_isWild"],
				Type = deckObj["Type"],
				SeasonId = deckObj["SeasonId"],
				CardBackId = deckObj["CardBackID"],
				HeroPremium = deckObj["HeroPremium"],
			};
			var cardList = deckObj["m_slots"];
			var cards = cardList["_items"];
			int size = cardList["_size"];
			for(var i = 0; i < size; i++)
			{
				var card = cards[i];
				string cardId = card["m_cardId"];

				var count = 0;
				var counts = card["m_count"];
				for(var j = 0; j < counts["_size"]; j++)
					count += (int)counts["_items"][j];

				var existingCard = deck.Cards.FirstOrDefault(x => x.Id == cardId);
				if(existingCard != null)
					existingCard.Count++;
				else
					deck.Cards.Add(new Card(cardId, count, false));
			}
			return deck;
		}

		public static int GetFormat() => TryGetInternal(() => (int)Mirror.Root["GameMgr"]["s_instance"]["m_formatType"]);

		public static Deck GetEditedDeck() => TryGetInternal(GetEditedDeckInternal);
		private static Deck GetEditedDeckInternal()
		{
			var taggedDecks = Mirror.Root["CollectionManager"]["s_instance"]["m_taggedDecks"];
			var tags = taggedDecks["keySlots"];
			var decks = taggedDecks["valueSlots"];
			for (var i = 0; i < tags.Length; i++)
			{
				if(tags[i] == null || decks[i] == null)
					continue;
				if(tags[i]["value__"] == 0)
					return GetDeck(decks[i]);
			}
			return null;
		}

		public static bool IsFriendsListVisible() => TryGetInternal(() => Mirror.Root["ChatMgr"]?["s_instance"]?["m_friendListFrame"]) != null;

		public static bool IsGameMenuVisible() => TryGetInternal(() => Mirror.Root["GameMenu"]?["s_instance"]?["m_isShown"]) ?? false;

		public static bool IsOptionsMenuVisible() => TryGetInternal(() => Mirror.Root["OptionsMenu"]?["s_instance"]?["m_isShown"]) ?? false;

		public static bool IsMulligan() => TryGetInternal(() => Mirror.Root["MulliganManager"]?["s_instance"]?["mulliganChooseBanner"]) != null;

		public static int GetNumMulliganCards() => TryGetInternal(() => Mirror.Root["MulliganManager"]?["s_instance"]?["m_startingCards"]?["_size"]) ?? 0;

		public static bool IsChoosingCard() => (TryGetInternal(() => Mirror.Root["ChoiceCardMgr"]?["s_instance"]?["m_subOptionState"]) != null)
				|| ((int)(TryGetInternal(() => Mirror.Root["ChoiceCardMgr"]?["s_instance"]?["m_choiceStateMap"]?["count"]) ?? 0) > 0);

		public static int GetNumChoiceCards() => TryGetInternal(() => Mirror.Root["ChoiceCardMgr"]?["s_instance"]?["m_lastShownChoices"]?["_size"]) ?? 0;

		public static bool IsPlayerEmotesVisible() => TryGetInternal(() => Mirror.Root["EmoteHandler"]?["s_instance"]?["m_emotesShown"]) ?? false;

		public static bool IsEnemyEmotesVisible() => TryGetInternal(() => Mirror.Root["EnemyEmoteHandler"]?["s_instance"]?["m_emotesShown"]) ?? false;

		public static bool IsInBattlecryEffect() => TryGetInternal(() => Mirror.Root["InputManager"]?["s_instance"]?["m_isInBattleCryEffect"]) ?? false;

		public static bool IsDragging() => TryGetInternal(() => Mirror.Root["InputManager"]?["s_instance"]?["m_dragging"]) ?? false;

		public static bool IsTargetingHeroPower() => TryGetInternal(() => Mirror.Root["InputManager"]?["s_instance"]?["m_targettingHeroPower"]) ?? false;

		public static int GetBattlecrySourceCardZonePosition() => TryGetInternal(() => Mirror.Root["InputManager"]?["s_instance"]?["m_battlecrySourceCard"]?["m_zonePosition"]) ?? 0;

		public static bool IsHoldingCard() => TryGetInternal(() => Mirror.Root["InputManager"]?["s_instance"]?["m_heldCard"]) != null;

		public static bool IsTargetReticleActive() => TryGetInternal(() => Mirror.Root["TargetReticleManager"]?["s_instance"]?["m_isActive"]) ?? false;

		public static bool IsEnemyTargeting() => TryGetInternal(() => Mirror.Root["InputManager"]?["s_instance"]?["m_isEnemyArrow"]) ?? false;

		public static bool IsGameOver() => TryGetInternal(() => Mirror.Root["GameState"]?["s_instance"]?["m_gameOver"]) ?? false;

		public static bool WasRestartRequested() => TryGetInternal(() => Mirror.Root["GameState"]?["s_instance"]?["m_restartRequested"]) ?? false;

		public static bool IsInMainMenu() => (int)(TryGetInternal(() => Mirror.Root["Box"]?["s_instance"]?["m_state"]) ?? -1) == (int)BoxState.HUB_WITH_DRAWER;

		public static UI_WINDOW GetShownUiWindowId() => (UI_WINDOW)(TryGetInternal(() => Mirror.Root["ShownUIMgr"]?["s_instance"]?["m_shownUI"]) ?? UI_WINDOW.NONE);

		public static bool IsPlayerHandZoneUpdatingLayout() => TryGetInternal(() => Mirror.Root["InputManager"]?["s_instance"]?["m_myHandZone"]?["m_updatingLayout"]) ?? false;

		public static bool IsPlayerPlayZoneUpdatingLayout() => TryGetInternal(() => Mirror.Root["InputManager"]?["s_instance"]?["m_myPlayZone"]?["m_updatingLayout"]) ?? false;

		public static SceneMode GetCurrentSceneMode() => (SceneMode)(TryGetInternal(() => Mirror.Root["SceneMgr"]?["s_instance"]?["m_mode"]) ?? SceneMode.INVALID);

		public static int GetNumCardsPlayerHand() => TryGetInternal(() => Mirror.Root["InputManager"]?["s_instance"]?["m_myHandZone"]?["m_cards"]?["_size"]) ?? 0;

		public static int GetNumCardsPlayerBoard() => TryGetInternal(() => Mirror.Root["InputManager"]?["s_instance"]?["m_myPlayZone"]?["m_cards"]?["_size"]) ?? 0;

		public static int GetNavigationHistorySize() => TryGetInternal(() => Mirror.Root["Navigation"]?["history"]?["_size"]) ?? 0;

		public static int GetCurrentManaFilter() => TryGetInternal(() => (int)Mirror.Root["CollectionManagerDisplay"]["s_instance"]["m_manaTabManager"]["m_currentFilterValue"]);

		public static SetFilterItem GetCurrentSetFilter() => TryGetInternal(GetCurrentSetFilterInternal);

		private static SetFilterItem GetCurrentSetFilterInternal()
		{
			var item = Mirror.Root["CollectionManagerDisplay"]["s_instance"]["m_setFilterTray"]["m_selected"];
			return new SetFilterItem()
			{
				IsAllStandard =  (bool)item["m_isAllStandard"],
				IsWild = (bool)item["m_isWild"]
			};
		}

		public static BattleTag GetBattleTag() => TryGetInternal(GetBattleTagInternal);

		private static BattleTag GetBattleTagInternal()
		{
			var bTag = Mirror.Root["BnetPresenceMgr"]["s_instance"]["m_myPlayer"]["m_account"]["m_battleTag"];
			return new BattleTag
			{
				Name = bTag["m_name"],
				Number = bTag["m_number"]
			};
		}

		public static List<Card> GetPackCards() => TryGetInternal(() => GetPackCardsInternal().ToList());

		private static IEnumerable<Card> GetPackCardsInternal()
		{
			var cards = Mirror.Root["PackOpening"]["s_instance"]["m_director"]?["m_hiddenCards"]?["_items"];
			if(cards == null)
				yield break;
			foreach(var card in cards)
			{
				if(card?.Class.Name != "PackOpeningCard")
					continue;
				var def = card["m_boosterCard"]?["<Def>k__BackingField"];
				if(def == null)
					continue;
				yield return new Card((string)def["<Name>k__BackingField"], 1, (int)def["<Premium>k__BackingField"] > 0);
			}
		}

		public static List<RewardData> GetArenaRewards() => TryGetInternal(() => GetArenaRewardsInternal().ToList());

		private static IEnumerable<RewardData> GetArenaRewardsInternal()
		{
			var rewards = Mirror.Root["DraftManager"]["s_instance"]["m_chest"]?["<Rewards>k__BackingField"]?["_items"];
			return RewardDataParser.Parse(rewards);
		}

		public static SeasonEndInfo GetSeasonEndInfo() => TryGetInternal(GetSeasonEndInfoInternal);

		private static SeasonEndInfo GetSeasonEndInfoInternal()
		{
			var dialog = Mirror.Root["DialogManager"]["s_instance"]["m_currentDialog"];
			if(dialog?.Class.Name != "SeasonEndDialog" || !dialog["m_shown"])
				return null;
			var info = dialog["m_seasonEndInfo"];
			var rewards = RewardDataParser.Parse(info["m_rankedRewards"]["_items"]);
			return new SeasonEndInfo(
				(int)info["m_bonusStars"],
				(int)info["m_boostedRank"],
				(int)info["m_chestRank"],
				(bool)info["m_isWild"],
				(int)info["m_legendIndex"],
				(int)info["m_rank"],
				(int)info["m_seasonID"],
				rewards);
		}

		public static int GetLastOpenedBoosterId() => (int)(TryGetInternal(() => Mirror.Root["PackOpening"]?["s_instance"]?["m_lastOpenedBoosterId"]) ?? 0);

		public static AccountId GetAccountId() => TryGetInternal(GetAccountIdInternal);

		private static AccountId GetAccountIdInternal()
		{
			var accId = Mirror.Root["BnetPresenceMgr"]?["s_instance"]?["m_myGameAccountId"];
			return accId == null ? null : new AccountId {Hi = accId["m_hi"], Lo = accId["m_lo"]};
		}

		public static BrawlInfo GetBrawlInfo() => TryGetInternal(GetBrawlInfoInternal);

		private static BrawlInfo GetBrawlInfoInternal()
		{
			var mission = GetCurrentBrawlMission();
			if(mission == null)
				return null;

			var brawlInfo = new BrawlInfo
			{
				MaxWins = mission["tavernBrawlSpec"]?["_MaxWins"],
				MaxLosses = mission["tavernBrawlSpec"]?["_MaxLosses"]
			};

			var records = Mirror.Root["TavernBrawlManager"]["s_instance"]?["m_playerRecords"];
			if(records == null)
				return null;

			dynamic record = null;
			foreach(var r in records)
			{
				if(r?.Class.Name != "TavernBrawlPlayerRecord")
					continue;
				record = r;
			}
			if(record == null)
				return null;

			brawlInfo.GamesPlayed = record["_GamesPlayed"];
			brawlInfo.WinStreak = record["_WinStreak"];
			if(brawlInfo.IsSessionBased)
			{
				if(!(bool)record["HasSession"])
					return brawlInfo;
				var session = record["_Session"];
				brawlInfo.Wins = session["<Wins>k__BackingField"];
				brawlInfo.Losses = session["<Losses>k__BackingField"];
			}
			else
			{
				brawlInfo.Wins = record["<GamesWon>k__BackingField"];
				brawlInfo.Losses = brawlInfo.GamesPlayed - brawlInfo.Wins;
			}
			return brawlInfo;
		}

		public static bool IsLogEnabled(string name) => TryGetInternal(() => IsLogEnabledInternal(name));

		private static bool IsLogEnabledInternal(string name)
		{
			var logs = Mirror.Root["Log"]?["s_instance"]?["m_logInfos"]?["valueSlots"];
			if(logs == null)
				return false;
			for(var i = 0; i < logs.Length; i++)
			{
				if(logs[i]?["m_name"] == name)
					return true;
			}
			return false;
		}

#if(DEBUG)
		public static void DebugHelper()
		{
			var data = new[]
			{
				"NetCache", "GameState", "Log", "TavernBrawlManager", "TavernBrawlDisplay", "BnetPresenceMgr", "DraftManager",
				"PackOpening", "CollectionManagerDisplay", "GameMgr", "Network", "DraftManager", "DraftDisplay"
			}.Select(x => Mirror.Root[x]?["s_instance"]).ToList();

			System.Diagnostics.Debugger.Break();
		}
#endif
	}
}