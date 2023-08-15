/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
    int32_t RefreshAppEnhanceData(const int32_t operType, const std::string &appData);
    // caller only can be engine, no lock
    int32_t GetMapItem(nlohmann::json *&inputJson, const std::string &key, nlohmann::json *&outputJson);

    // scene expericence config data
    int32_t RefreshSceneConfigData(const std::string &ruleConfig, const std::string &sceneExperience);
    int32_t GetSceneExperience(const std::string experienceType, const int32_t experienceValue,
        ExperienceResult &result);
    int32_t GetSceneConfigDataVersion(std::string &configVersion);
    int32_t CheckPkgNameInCardOwner(const std::string &pkgName, bool &findPkg);

    // in-process cache
    void SetExemptExperience(const std::string &targetBundleName, const int32_t rule, const int32_t timeout);
    void GetExemptExperience(const std::string &targetBundleName, int32_t &rule, bool &bExempt);

    void SetSceneCode(const std::string &bundleName, const std::string &sceneCode);

    int32_t GetSceneCode(const std::string &bundleName, const int32_t &sceneCodeType, std::string &sceneCode);

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

    nlohmann::json ruleControlStrategy;
    nlohmann::json sceneConfigData;

    std::map<std::string, struct ExemptExperienceValue> exemptExperienceCache;
    std::map<std::string, struct SceneRequestValue> sceneRequestCache;
    std::map<std::string, std::list<struct SceneCodeValue>> sceneCodeCache;

    std::mutex ruleStrategyLock_;
    std::mutex sceneConfigDataLock_;

    std::mutex exemptCacheLock_;
    std::mutex sceneRequstCacheLock_;
    std::mutex sceneCodeCacheLock_;

    // timer
    bool bStarted;

    // config data
    int32_t ConstructSceneConfigDataJson(const std::string &ruleConfig, const std::string &sceneExperience,
        nlohmann::json &outputJson, std::string &configVersion);

    // time
    uint64_t GetMillisecondsOfDay();
    void StartTimer(uint64_t interval, int type, bool repeat, std::function<void()> callBack_);
};
}
}
#endif
