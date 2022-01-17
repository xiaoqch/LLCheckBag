#pragma once

inline class Player* getPlayer(class mce::UUID const& a0) {
    class Player* (Level:: * rv)(class mce::UUID const&);
    *((void**)&rv) = dlsym("?getPlayer@Level@@UEBAPEAVPlayer@@AEBVUUID@mce@@@Z");
    return (Global<Level>->*rv)(std::forward<class mce::UUID const&>(a0));
}

class CheckBagManager
{
    CheckBagManager();
    CheckBagManager(CheckBagManager& manager) = delete;
    struct CheckBagLog {
        mce::UUID mTarget;
        std::unique_ptr<CompoundTag> mBackup;

        CheckBagLog(mce::UUID const& target, std::unique_ptr<CompoundTag> backup)
            :mBackup(std::move(backup)),
            mTarget(target) {
        }
        CheckBagLog(Player* target, std::unique_ptr<CompoundTag> backup) : mBackup(std::move(backup)) {
            auto uuid = target->getUuid();
            mTarget = mce::UUID::fromString(uuid);
        }

        inline Player* getTarget() const {
            return getPlayer(mTarget);
        }

    };
    std::vector<std::string> mUuidList;
    std::unordered_map<std::string, __int64> mRemoveRequsets;
    std::unordered_map<std::string, CheckBagLog> mCheckBagLog;
public:
    enum class Result : int {
        Success,
        Error,
        Request,
        NotStart,
        BackupError,
        BackupNotFound
    };
    enum class DataType :int {
        Snbt,
        Binary,
        Json,
        Unknown,
    };

    static bool mIsFree;
    static CheckBagManager& getManager();

    std::string getSuffix(DataType type);
    DataType fromSuffix(std::string_view suffix);

    inline void updateIsFree() {
        mIsFree = mRemoveRequsets.size() + mCheckBagLog.size();
    }

    void beforePlayerLeave(ServerPlayer* player);
    void afterPlayerLeave(ServerPlayer* player);

    inline bool isCheckingBag(Player* player) {
        auto uuid = player->getUuid();
        return mCheckBagLog.find(uuid) == mCheckBagLog.end();
    }
    inline std::string getBackupPath(Player* player) {
        auto realName = player->getRealName();
        auto path = std::filesystem::path(Config::BackupDirectory);
        path.append(realName + ".nbt");
        return path.string();
    }
    inline std::string getExportPath(std::string const& name, std::string const& suffix) {
        auto path = std::filesystem::path(Config::ExportDirectory);
        path.append(name + "." + suffix);
        return path.string();
    }
    std::vector<std::string> getPlayerList();
    std::unique_ptr<CompoundTag> getBackupBag(Player* player);

    Result removePlayerData(ServerPlayer* player);
    Result removePlayerData(mce::UUID const& uuid);
    Result backupData(Player* player, mce::UUID const& target, CompoundTag& tag);
    Result overwriteBagData(Player* player, CheckBagLog const& log);
    Result restoreBagData(Player* player);
    Result setBagData(Player* player, mce::UUID const& uuid, std::unique_ptr<CompoundTag> targetTag);
    Result stopCheckBag(Player* player);
    Result startCheckBag(Player* player, Player* target);
    Result startCheckBag(Player* player, mce::UUID const& uuid);
    Result overwriteData(Player* player);
    Result exportData(std::string const& name, DataType type);
};
