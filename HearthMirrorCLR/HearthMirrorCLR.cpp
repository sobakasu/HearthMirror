// This is the main DLL file.

#include "stdafx.h"

#include "HearthMirrorCLR.h"
#include "msclr\marshal_cppstd.h"
#include "codecvt"
#include <windows.h>
#include <tlhelp32.h>

#if _MSC_VER == 1900

// Required due to http://stackoverflow.com/questions/32055357/visual-studio-c-2015-stdcodecvt-with-char16-t-or-char32-t
System::String^ utf16_to_managed_string(std::u16string utf16_string)
{
	std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
	auto p = reinterpret_cast<const int16_t *>(utf16_string.data());
	auto str_bytes = convert.to_bytes(p, p + utf16_string.size());
	return msclr::interop::marshal_as<System::String^>(str_bytes);
}

#else

System::String^ utf16_to_managed_string(std::u16string utf16_string)
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	auto str_bytes = convert.to_bytes(utf16_string);
	return msclr::interop::marshal_as<System::String^>(str_bytes);
}

#endif

DWORD GetProcId(WCHAR* ProcName) {
	PROCESSENTRY32   pe32;
	HANDLE         hSnapshot = NULL;

	pe32.dwSize = sizeof(PROCESSENTRY32);
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Process32First(hSnapshot, &pe32))
	{
		do {
			if (wcscmp(pe32.szExeFile, ProcName) == 0)
				break;
		} while (Process32Next(hSnapshot, &pe32));
	}

	if (hSnapshot != INVALID_HANDLE_VALUE)
		CloseHandle(hSnapshot);

	return pe32.th32ProcessID;
}

namespace HearthMirrorCLR
{
	namespace Objects
	{
		BattleTag::BattleTag(_BattleTag battleTag) {
			Name = utf16_to_managed_string(battleTag.name);
			Number = battleTag.number;
		}
		
		BattleTag::BattleTag() {}

		Card::Card(_Card card) {
			Id = utf16_to_managed_string(card.id);
			Count = card.count;
			Premium = card.premium;
		}

		Card::Card(String^ id, int count, bool premium) {
			Id = id;
			Count = count;
			Premium = premium;
		}

		Card::Card() {}

		Deck::Deck(_Deck deck) {
			Id = deck.id;
			Name = utf16_to_managed_string(deck.name);
			Hero = utf16_to_managed_string(deck.hero);
			IsWild = deck.isWild;
			Type = deck.type;
			SeasonId = deck.seasonId;
			CardBackId = deck.cardBackId;
			HeroPremium = deck.heroPremium;
			Cards = gcnew List<Card^>();

			std::vector<_Card>::iterator itr;
			for (itr = deck.cards.begin(); itr != deck.cards.end(); itr++) {
				Cards->Add(gcnew Card(*itr));
			}
		}

		Deck::Deck() {}

		GameServerInfo::GameServerInfo(_InternalGameServerInfo gameServerInfo) {
			Address = utf16_to_managed_string(gameServerInfo.address);
			AuroraPassword = utf16_to_managed_string(gameServerInfo.auroraPassword);
			ClientHandle = gameServerInfo.clientHandle;
			GameHandle = gameServerInfo.gameHandle;
			Mission = gameServerInfo.mission;
			Port = gameServerInfo.port;
			Resumable = gameServerInfo.resumable;
			SpectatorMode = gameServerInfo.spectatorMode;
			SpectatorPassword = utf16_to_managed_string(gameServerInfo.spectatorPassword);
			Version = utf16_to_managed_string(gameServerInfo.version);
		}

		GameServerInfo::GameServerInfo() {}

		MatchInfo::Player::Player(_InternalPlayer player) {
			Name = utf16_to_managed_string(player.name);
			Id = player.id;
			StandardRank = player.standardRank;
			StandardLegendRank = player.standardLegendRank;
			StandardStars = player.standardStars;
			WildRank = player.wildRank;
			WildLegendRank = player.wildLegendRank;
			WildStars = player.wildStars;
			CardBackId = player.cardBackId;
		}

		MatchInfo::Player::Player() {}

		MatchInfo::MatchInfo(_InternalMatchInfo matchInfo) {
			LocalPlayer = gcnew Player(matchInfo.localPlayer);
			OpposingPlayer = gcnew Player(matchInfo.opposingPlayer);
			BrawlSeasonId = matchInfo.brawlSeasonId;
			MissionId = matchInfo.missionId;
			RankedSeasonId = matchInfo.rankedSeasonId;
		}

		MatchInfo::MatchInfo() {}

		RewardData::RewardData(_RewardData rewardData) {
			Type = static_cast<HearthMirrorCLR::Enums::RewardType>(rewardData.type);
		}

		RewardData::RewardData() {}

		ArcaneDustRewardData::ArcaneDustRewardData(_ArcaneDustRewardData arcaneDustRewardData) : RewardData(arcaneDustRewardData) {
			Amount = arcaneDustRewardData.amount;
		}

		ArcaneDustRewardData::ArcaneDustRewardData() {}

		BoosterPackRewardData::BoosterPackRewardData(_BoosterPackRewardData boosterPackRewardData) : RewardData(boosterPackRewardData) {
			Id = boosterPackRewardData.id;
			Count = boosterPackRewardData.count;
		}

		BoosterPackRewardData::BoosterPackRewardData() {}

		CardRewardData::CardRewardData(_CardRewardData cardRewardData) : RewardData(cardRewardData) {
			Id = utf16_to_managed_string(cardRewardData.id);
			Count = cardRewardData.count;
			Premium = cardRewardData.premium;
		}

		CardRewardData::CardRewardData() {}

		CardBackRewardData::CardBackRewardData(_CardBackRewardData cardBackRewardData) : RewardData(cardBackRewardData) {
			Id = cardBackRewardData.id;
		}

		CardBackRewardData::CardBackRewardData() {}

		ForgeTicketRewardData::ForgeTicketRewardData(_ForgeTicketRewardData forgeTicketRewardData) : RewardData(forgeTicketRewardData) {
			Quantity = forgeTicketRewardData.quantity;
		}

		ForgeTicketRewardData::ForgeTicketRewardData() {}

		GoldRewardData::GoldRewardData(_GoldRewardData goldRewardData) : RewardData(goldRewardData) {
			Amount = goldRewardData.amount;
		}

		GoldRewardData::GoldRewardData(int amount) : RewardData() {
			Type = HearthMirrorCLR::Enums::RewardType::GOLD;
			Amount = amount;
		}

		GoldRewardData::GoldRewardData() {}

		MountRewardData::MountRewardData(_MountRewardData mountRewardData) : RewardData(mountRewardData) {
			MountType = mountRewardData.mountType;
		}

		MountRewardData::MountRewardData() {}

		ArenaInfo::ArenaInfo(_ArenaInfo arenaInfo) {
			Deck = gcnew Objects::Deck(arenaInfo.deck);
			Losses = arenaInfo.losses;
			Wins = arenaInfo.wins;
			CurrentSlot = arenaInfo.currentSlot;
			Rewards = gcnew List<RewardData^>();

			std::vector<_RewardData*>::iterator itr;
			for (itr = arenaInfo.rewards.begin(); itr != arenaInfo.rewards.end(); itr++) {
				Rewards->Add(gcnew RewardData(**itr));
			}
		}

		ArenaInfo::ArenaInfo() {}

		AccountId::AccountId(_AccountId accountId) {
			Hi = accountId.hi;
			Lo = accountId.lo;
		}

		AccountId::AccountId() {}

		BrawlInfo::BrawlInfo(_BrawlInfo brawlInfo) {
			MaxWins = brawlInfo.maxWins;
			MaxLosses = brawlInfo.maxLosses;
			IsSessionBased = brawlInfo.isSessionBased;
			Wins = brawlInfo.wins;
			Losses = brawlInfo.losses;
			GamesPlayed = brawlInfo.gamesPlayed;
			WinStreak = brawlInfo.winStreak;
		}

		BrawlInfo::BrawlInfo() {}

		SetFilterItem::SetFilterItem(_SetFilterItem setFilterItem) {
			IsAllStandard = setFilterItem.isAllStandard;
			IsWild = setFilterItem.isWild;
		}

		SetFilterItem::SetFilterItem() {}
	}

	HearthMirrorCLR::Objects::BattleTag^ Reflection::GetBattleTag() {
		_BattleTag unmanaged = Reflection::getMirror()->getBattleTag();
		HearthMirrorCLR::Objects::BattleTag^ managed = gcnew HearthMirrorCLR::Objects::BattleTag(unmanaged);

		return managed;
	}
	List<HearthMirrorCLR::Objects::Card^>^ Reflection::GetCollection() {
		std::vector<Card> unmanaged = Reflection::getMirror()->getCardCollection();
		List<HearthMirrorCLR::Objects::Card^>^ managed = gcnew List<HearthMirrorCLR::Objects::Card^>();

		std::vector<_Card>::iterator itr;
		for (itr = unmanaged.begin(); itr != unmanaged.end(); itr++) {
			managed->Add(gcnew HearthMirrorCLR::Objects::Card(*itr));
		}

		return managed;
	}
	HearthMirrorCLR::Objects::GameServerInfo^ Reflection::GetServerInfo() {
		_InternalGameServerInfo unmanaged = Reflection::getMirror()->getGameServerInfo();
		HearthMirrorCLR::Objects::GameServerInfo^ managed = gcnew HearthMirrorCLR::Objects::GameServerInfo(unmanaged);

		return managed;
	}
	int Reflection::GetGameType() {
		return Reflection::getMirror()->getGameType();
	}
	HearthMirrorCLR::Objects::MatchInfo^ Reflection::GetMatchInfo() {
		_InternalMatchInfo unmanaged = Reflection::getMirror()->getMatchInfo();
		HearthMirrorCLR::Objects::MatchInfo^ managed = gcnew HearthMirrorCLR::Objects::MatchInfo(unmanaged);

		return managed;
	}
	int Reflection::GetFormat() {
		return Reflection::getMirror()->getFormat();
	}
	bool Reflection::IsSpectating() {
		return Reflection::getMirror()->isSpectating();
	}
	HearthMirrorCLR::Objects::AccountId^ Reflection::GetAccountId() {
		_AccountId unmanaged = Reflection::getMirror()->getAccountId();
		HearthMirrorCLR::Objects::AccountId^ managed = gcnew HearthMirrorCLR::Objects::AccountId(unmanaged);

		return managed;
	}
	List<HearthMirrorCLR::Objects::Deck^>^ Reflection::GetDecks() {
		std::vector<_Deck> unmanaged = Reflection::getMirror()->getDecks();
		List<HearthMirrorCLR::Objects::Deck^>^ managed = gcnew List<HearthMirrorCLR::Objects::Deck^>();

		std::vector<_Deck>::iterator itr;
		for (itr = unmanaged.begin(); itr != unmanaged.end(); itr++) {
			managed->Add(gcnew HearthMirrorCLR::Objects::Deck(*itr));
		}

		return managed;
	}
	long Reflection::GetSelectedDeckInMenu() {
		return Reflection::getMirror()->getSelectedDeckInMenu();
	}
	HearthMirrorCLR::Objects::ArenaInfo^ Reflection::GetArenaDeck() {
		_ArenaInfo unmanaged = Reflection::getMirror()->getArenaDeck();
		HearthMirrorCLR::Objects::ArenaInfo^ managed = gcnew HearthMirrorCLR::Objects::ArenaInfo(unmanaged);

		return managed;
	}
	List<HearthMirrorCLR::Objects::Card^>^ Reflection::GetArenaDraftChoices() {
		std::vector<_Card> unmanaged = Reflection::getMirror()->getArenaDraftChoices();
		List<HearthMirrorCLR::Objects::Card^>^ managed = gcnew List<HearthMirrorCLR::Objects::Card^>();

		std::vector<_Card>::iterator itr;
		for (itr = unmanaged.begin(); itr != unmanaged.end(); itr++) {
			managed->Add(gcnew HearthMirrorCLR::Objects::Card(*itr));
		}

		return managed;
	}
	List<HearthMirrorCLR::Objects::Card^>^ Reflection::GetPackCards() {
		std::vector<_Card> unmanaged = Reflection::getMirror()->getPackCards();
		List<HearthMirrorCLR::Objects::Card^>^ managed = gcnew List<HearthMirrorCLR::Objects::Card^>();

		std::vector<_Card>::iterator itr;
		for (itr = unmanaged.begin(); itr != unmanaged.end(); itr++) {
			managed->Add(gcnew HearthMirrorCLR::Objects::Card(*itr));
		}

		return managed;
	}
	HearthMirrorCLR::Objects::BrawlInfo^ Reflection::GetBrawlInfo() {
		_BrawlInfo unmanaged = Reflection::getMirror()->getBrawlInfo();
		HearthMirrorCLR::Objects::BrawlInfo^ managed = gcnew HearthMirrorCLR::Objects::BrawlInfo(unmanaged);

		return managed;
	}
	HearthMirrorCLR::Objects::Deck^ Reflection::GetEditedDeck() {
		_Deck unmanaged = Reflection::getMirror()->getEditedDeck();
		HearthMirrorCLR::Objects::Deck^ managed = gcnew HearthMirrorCLR::Objects::Deck(unmanaged);

		return managed;
	}
	bool Reflection::IsFriendsListVisible() {
		return Reflection::getMirror()->isFriendsListVisible();
	}
	bool Reflection::IsGameMenuVisible() {
		return Reflection::getMirror()->isGameMenuVisible();
	}
	bool Reflection::IsOptionsMenuVisible() {
		return Reflection::getMirror()->isOptionsMenuVisible();
	}
	bool Reflection::IsMulligan() {
		return Reflection::getMirror()->isMulligan();
	}
	int Reflection::GetNumMulliganCards() {
		return Reflection::getMirror()->getNumMulliganCards();
	}
	bool Reflection::IsChoosingCard() {
		return Reflection::getMirror()->isChoosingCard();
	}
	int Reflection::GetNumChoiceCards() {
		return Reflection::getMirror()->getNumChoiceCards();
	}
	bool Reflection::IsPlayerEmotesVisible() {
		return Reflection::getMirror()->isPlayerEmotesVisible();
	}
	bool Reflection::IsEnemyEmotesVisible() {
		return Reflection::getMirror()->isEnemyEmotesVisible();
	}
	bool Reflection::IsInBattlecryEffect() {
		return Reflection::getMirror()->isInBattlecryEffect();
	}
	bool Reflection::IsDragging() {
		return Reflection::getMirror()->isDragging();
	}
	bool Reflection::IsTargetingHeroPower() {
		return Reflection::getMirror()->isTargetingHeroPower();
	}
	int Reflection::GetBattlecrySourceCardZonePosition() {
		return Reflection::getMirror()->getBattlecrySourceCardZonePosition();
	}
	bool Reflection::IsHoldingCard() {
		return Reflection::getMirror()->isHoldingCard();
	}
	bool Reflection::IsTargetReticleActive() {
		return Reflection::getMirror()->isTargetReticleActive();
	}
	bool Reflection::IsEnemyTargeting() {
		return Reflection::getMirror()->isEnemyTargeting();
	}
	bool Reflection::IsGameOver() {
		return Reflection::getMirror()->isGameOver();
	}
	bool Reflection::IsInMainMenu() {
		return Reflection::getMirror()->isInMainMenu();
	}
	HearthMirrorCLR::Enums::UI_WINDOW Reflection::GetShownUiWindowId() {
		return static_cast<HearthMirrorCLR::Enums::UI_WINDOW>(Reflection::getMirror()->getShownUiWindowId());
	}
	HearthMirrorCLR::Enums::SceneMode Reflection::GetCurrentSceneMode() {
		return static_cast<HearthMirrorCLR::Enums::SceneMode>(Reflection::getMirror()->GetCurrentSceneMode());
	}
	bool Reflection::IsPlayerHandZoneUpdatingLayout() {
		return Reflection::getMirror()->isPlayerHandZoneUpdatingLayout();
	}
	bool Reflection::IsPlayerPlayZoneUpdatingLayout() {
		return Reflection::getMirror()->isPlayerPlayZoneUpdatingLayout();
	}
	int Reflection::GetNumCardsPlayerHand() {
		return Reflection::getMirror()->getNumCardsPlayerHand();
	}
	int Reflection::GetNumCardsPlayerBoard() {
		return Reflection::getMirror()->getNumCardsPlayerBoard();
	}
	int Reflection::GetNavigationHistorySize() {
		return Reflection::getMirror()->getNavigationHistorySize();
	}
	int Reflection::GetCurrentManaFilter() {
		return Reflection::getMirror()->getCurrentManaFilter();
	}
	HearthMirrorCLR::Objects::SetFilterItem^ Reflection::GetCurrentSetFilter() {
		_SetFilterItem unmanaged = Reflection::getMirror()->getCurrentSetFilter();
		HearthMirrorCLR::Objects::SetFilterItem^ managed = gcnew HearthMirrorCLR::Objects::SetFilterItem(unmanaged);

		return managed;
	}
	int Reflection::GetLastOpenedBoosterId() {
		return Reflection::getMirror()->getLastOpenedBoosterId();
	}

	hearthmirror::Mirror* Reflection::getMirror() {
		if (Reflection::_mirror == nullptr) {
			DWORD pid = GetProcId(L"Hearthstone.exe");
			Reflection::_mirror = new hearthmirror::Mirror(pid);
		}
		return Reflection::_mirror;
	}

	Status^ Status::GetStatus() {
		// TODO when status is available
		return gcnew Status(HearthMirrorCLR::Enums::MirrorStatus::Ok);
	}

	Status::Status(HearthMirrorCLR::Enums::MirrorStatus status) {
		MirrorStatus = status;
	}

	Status::Status(System::Exception^ ex) {
		MirrorStatus = HearthMirrorCLR::Enums::MirrorStatus::Error;
		Exception = ex;
	}
}