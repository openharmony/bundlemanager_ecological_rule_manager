/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: cache define
 */

#ifndef FOUNDATION_ECOLOGICAL_RULE_MGR_SERVICE_INCLUDE_ENGINE_CACHE_H
#define FOUNDATION_ECOLOGICAL_RULE_MGR_SERVICE_INCLUDE_ENGINE_CACHE_H

#include <string>
#include <list>
#include "nlohmann/json.hpp"
#include "ecological_rule_mgr_service_param.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
struct ExperienceResult {
    bool allow = false;
    sptr<Want> replaceWant;
};

class EcologicalRuleEngineCache {
public:
    static EcologicalRuleEngineCache &GetInstance()
    {
        static EcologicalRuleEngineCache instance_;
        return instance_;
    }
    EcologicalRuleEngineCache(const EcologicalRuleEngineCache &other) = delete;
    EcologicalRuleEngineCache &operator = (const EcologicalRuleEngineCache &other) = delete;

    // enhance_data root, rule_control_strategy root , just for engine call, no lock
    int32_t GetEngineDataRoot(const std::string &rootName, nlohmann::json *&rootJson);

    // rule control strategy data
    // lock by caller when refresh data, caller can only be service
    int32_t RefreshRuleControlStrategy(const std::string &ruleInfo);
    // lock all rule_control_strategy_data, just for service
    void GetRuleControlStrategyDataLock();
    // release all rule_control_strategy_data, just for service
    void ReleaseRuleControlStrategyDataLock();
    // cann't be called by engine, have lock
    int32_t GetRuleStrategyDataVersion(std::string &dataVersion);
    // have lock, called after construct rule strategy object
    void ClearRuleControlStrategyCache();

    // enhance data
    // caller only can be AG, quick-app, have lock
    int32_t RefreshAppEnhanceData(const int32_t operType, const std::string &appData);
    // caller only can be engine, no lock
    int32_t GetVectorItem(nlohmann::json *&inputJson, const std::string &key, const std::string &value,
        nlohmann::json *&outputJson);
    // caller only can be engine, no lock
    int32_t GetMapItem(nlohmann::json *&inputJson, const std::string &key, nlohmann::json *&outputJson);
    // caller only can be engine, no lock
    int32_t GetAppEnhanceItem(const std::string &pkgName, nlohmann::json *&outPutJson);
    int32_t ParseAppEnhanceItem(const std::string &callerPkgName, const std::string &wantPkgName);
    // lock all app_enhance_data, just for engine
    void GetAppEnhanceDataLock();
    // release all app_enhance_data, just for engine
    void ReleaseAppEnhanceDataLock();

    // scene expericence config data
    int32_t RefreshSceneConfigData(const std::string &ruleConfig, const std::string &sceneExperience);
    int32_t GetSceneExperience(const std::string experienceType, const int32_t experienceValue,
        ExperienceResult &result);
    int32_t GetSceneConfigDataVersion(std::string &configVersion);
    int32_t GetPublicKeyByNetworkID(const std::string &networkId, std::string &publicKey, int32_t &platformType);
    int32_t CheckPkgNameInCardOwner(const std::string &pkgName, bool &findPkg);

    // load
    void LoadRuleStrategyFile();
    void LoadAppEnhanceFile();
    void LoadSceneConfigFile();

    // in-process cache
    void SetExemptExperience(const std::string &targetBundleName, const int32_t rule, const int32_t timeout);
    void GetExemptExperience(const std::string &targetBundleName, int32_t &rule, bool &bExempt);

    void SetSceneCode(const std::string &bundleName, const std::string &sceneCode);

    int32_t GetSceneCode(const std::string &bundleName, const int32_t &sceneCodeType, std::string &sceneCode);

    int32_t SetExperienceConfig(const std::string &sceneKey, const std::string &sceneCode,
        const std::string &experience);
    int32_t GetExperienceConfig(const std::string &sceneKey, std::string &sceneCode, std::string &experience);

    void ClearSceneCodeCache();

private:
    EcologicalRuleEngineCache() = default;
    ~EcologicalRuleEngineCache() = default;
    struct ExemptExperienceValue {
        int32_t rule;
        int32_t timeout;
        uint64_t timestamp;
    };
    struct SceneRequestValue {
        std::string sceneCode;
        std::string experience;
        uint64_t timestamp;
    };
    struct SceneCodeValue {
        std::string sceneCode;
        uint64_t timestamp;
    };
    struct AppEnhanceItem {
        std::string pkgName;
        nlohmann::json appInfo;
    };

    AppEnhanceItem appEnhanceWantData;
    AppEnhanceItem appEnhanceCallerData;
    nlohmann::json ruleControlStrategy;
    nlohmann::json sceneConfigData;

    std::map<std::string, std::string> appEnhanceDataCache;

    std::map<std::string, struct ExemptExperienceValue> exemptExperienceCache;
    std::map<std::string, struct SceneRequestValue> sceneRequestCache;
    std::map<std::string, std::list<struct SceneCodeValue>> sceneCodeCache;

    std::mutex appEnhanceDataLock_;
    std::mutex ruleStrategyLock_;
    std::mutex sceneConfigDataLock_;

    std::mutex exemptCacheLock_;
    std::mutex sceneRequstCacheLock_;
    std::mutex sceneCodeCacheLock_;

    // timer
    bool bStarted;

    // App enhance data
    int32_t AddAppEnhanceData(const int32_t operType, const std::string &appData);
    int32_t DeletePartAppEnhanceData(const std::string &appData);
    void DeleteOneAppEnhanceData(const std::string &pkgName);

    // config data
    int32_t ConstructSceneConfigDataJson(const std::string &ruleConfig, const std::string &sceneExperience,
        nlohmann::json &outputJson, std::string &configVersion);
    void EraseConfigDataAdsInfo(nlohmann::json &configDataJson, const std::string &configVersion);
    bool EraseOneItemInAdsInfo(nlohmann::json::array_t &jsonAdsArray);

    // file oper
    void WriteFile(const nlohmann::json &inputJson, const std::string &fileName);
    int32_t ReadFile(const std::string &fileName, nlohmann::json &outputJson);
    void WriteDebugFile(const std::string &input, const std::string &fileName);
    int32_t LoadAppEnhanceDataFile();
    void WriteAppEnhanceFile();
    // time
    uint64_t GetMillisecondsOfDay();
    void StartTimer(uint64_t interval, int type, bool repeat, std::function<void()> callBack_);
};
}
}
#endif
