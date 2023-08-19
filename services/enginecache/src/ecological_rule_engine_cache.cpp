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

#include "ecological_rule_engine_cache.h"
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "ecological_rule_engine_constants.h"
#include "ecological_rule_mgr_service_logger.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
#define TAG "ERMS_CACHE"

int32_t EcologicalRuleEngineCache::GetEngineDataRoot(const std::string &rootName, nlohmann::json *&rootJson)
{
    if (rootName == EngineCacheConstants::KEYWORD_RULE_CONTROL_STRATEGY) {
        rootJson = &ruleControlStrategy;
        return 0;
    }
    return -1;
}

void EcologicalRuleEngineCache::GetRuleControlStrategyDataLock()
{
    ruleStrategyLock_.lock();
}
// release all rule_control_strategy_data, just for engine
void EcologicalRuleEngineCache::ReleaseRuleControlStrategyDataLock()
{
    ruleStrategyLock_.unlock();
}

int32_t EcologicalRuleEngineCache::GetRuleStrategyDataVersion(std::string &dataVersion)
{
    std::lock_guard<std::mutex> autoLock(ruleStrategyLock_);
    auto verItem = ruleControlStrategy.find(EngineCacheConstants::APP_ENHANCE_DATA_KEY_DATA_VER);
    if (verItem == ruleControlStrategy.end()) {
        return -1;
    }
    dataVersion = verItem.value();
    LOG_DEBUG("get data version=%{public}s", dataVersion.c_str());
    return 0;
}

// lock by caller when refresh data
int32_t EcologicalRuleEngineCache::RefreshRuleControlStrategy(const std::string &ruleInfo)
{
    LOG_DEBUG("refresh rule strategy, input=%{public}s", ruleInfo.c_str());

    try {
        ruleControlStrategy = nlohmann::json::parse(ruleInfo, nullptr, false);
        if (ruleControlStrategy.is_discarded()) {
            LOG_ERROR("refresh rule strategy, json is invalid");
            return -1;
        }
    } catch (nlohmann::json::parse_error &e) {
        LOG_ERROR("refresh rule strategy, json error=%{public}s", e.what());
        return -1;
    }

    // find data version
    auto verItem = ruleControlStrategy.find(EngineCacheConstants::APP_ENHANCE_DATA_KEY_DATA_VER);
    if (verItem == ruleControlStrategy.end()) {
        LOG_WARN("refresh rule strategy, can't find dataversion item");
    } else {
        std::string dataVersion = verItem.value();
        LOG_INFO("rule strategy  refresh success, dataVer = %{public}s", dataVersion.c_str());
    }
    return 0;
}

// need lock by caller when be called
void EcologicalRuleEngineCache::ClearRuleControlStrategyCache()
{
    try {
        if (!ruleControlStrategy.empty()) {
            ruleControlStrategy.erase(EngineCacheConstants::APP_STRATEGY_DATA_ROOT_KEY);
        } else {
            LOG_INFO("ruleControlStrategy is empty, no need to clear rule strategy cache");
        }
    } catch (nlohmann::json::exception &e) {
        LOG_ERROR("clear rule strategy cache fail, error=%{public}s", e.what());
    }
}

int32_t EcologicalRuleEngineCache::GetMapItem(nlohmann::json *&inputJson, const std::string &key,
    nlohmann::json *&outputJson)
{
    if (inputJson == NULL) {
        LOG_ERROR("input is null, itemKey = %{public}s", key.c_str());
        return -1;
    }
    try {
        auto jo = inputJson->find(key);
        if (jo == inputJson->end()) {
            LOG_DEBUG("not find item, input = %{public}s", key.c_str());
            return -1;
        }
        outputJson = (nlohmann::json *)&(*jo);
    } catch (nlohmann::json::exception &e) {
        LOG_ERROR("itemKey = %{public}s, parser error=%{public}s", key.c_str(), e.what());
        return -1;
    }
    return 0;
}

int32_t EcologicalRuleEngineCache::RefreshAppEnhanceData(const int32_t operType, const std::string &appData)
{
    LOG_INFO("refresh enhance success, operType = %{public}d", operType);
    return 0;
}

int32_t EcologicalRuleEngineCache::RefreshSceneConfigData(const std::string &ruleConfig,
    const std::string &sceneExperience)
{
    int32_t ret = 0;
    std::string configVersion;
    LOG_DEBUG("ruleConfig=%{public}s", ruleConfig.c_str());
    std::lock_guard<std::mutex> autoLock(sceneConfigDataLock_);
    ret = EcologicalRuleEngineCache::ConstructSceneConfigDataJson(ruleConfig, sceneExperience, sceneConfigData,
        configVersion);
    if (ret != 0) {
        return -1;
    }
    LOG_INFO("refresh to cache success, dataVer = %{public}s", configVersion.c_str());
    return 0;
}

int32_t EcologicalRuleEngineCache::GetSceneConfigDataVersion(std::string &configVersion)
{
    std::lock_guard<std::mutex> autoLock(sceneConfigDataLock_);
    auto configver = sceneConfigData.find(EngineCacheConstants::SCENE_DATA_KEY_CONFIG_VER);
    if (configver == sceneConfigData.end()) {
        LOG_DEBUG("get config data version, not find");
        return -1;
    }
    configVersion = configver.value();
    LOG_DEBUG("get config data version success, version=%{public}s", configVersion.c_str());
    return 0;
}

int32_t EcologicalRuleEngineCache::GetSceneExperience(const std::string experienceType, const int32_t experienceValue,
    ExperienceResult &result)
{
    try {
        std::lock_guard<std::mutex> autoLock(sceneConfigDataLock_);
        auto itSceneExperience = sceneConfigData.find(EngineCacheConstants::SCENE_DATA_KEY_SCENE_EXP);
        if (itSceneExperience == sceneConfigData.end()) {
            LOG_ERROR("get scene exp, not find scene exp key, type=%{public}s, exp value=%{public}d",
                experienceType.c_str(), experienceValue);
            return -1;
        }

        nlohmann::json &jsonSceneExperience = itSceneExperience.value();
        auto itExperience = jsonSceneExperience.find(experienceType);
        if (itExperience == jsonSceneExperience.end()) {
            LOG_ERROR("get scene exp, not find scene exp type, type=%{public}s, exp value=%{public}d",
                experienceType.c_str(), experienceValue);
            return -1;
        }

        nlohmann::json &jsonExperience = itExperience.value();
        auto itResult = jsonExperience.find(std::to_string(experienceValue));
        if (itResult == jsonExperience.end()) {
            LOG_ERROR("get scene exp, not find scene exp value, type=%{public}s, exp value=%{public}d",
                experienceType.c_str(), experienceValue);
            return -1;
        }

        nlohmann::json &jsonResult = itResult.value();
        auto itAllow = jsonResult.find(EngineCacheConstants::SCENE_EXPERIENCE_KEY_ALLOW);
        if (itAllow == jsonResult.end()) {
            LOG_ERROR("no 'allow',type=%{public}s,exp=%{public}d", experienceType.c_str(), experienceValue);
            return -1;
        }
        result.allow = itAllow.value();
        auto itWant = jsonResult.find(EngineCacheConstants::SCENE_EXPERIENCE_KEY_WANT);
        if (itWant == jsonResult.end()) {
            LOG_INFO("get scene exp, not find 'want', type=%{public}s, exp value=%{public}d", experienceType.c_str(),
                experienceValue);
            result.replaceWant = nullptr;
            return 0;
        }
        nlohmann::json::string_t *wantInfo = itWant.value().get_ptr<nlohmann::json::string_t *>();
        result.replaceWant = Want::FromString(*wantInfo);
        if (result.replaceWant == nullptr) {
            LOG_ERROR("get scene exp, want info is null, type=%{public}s, exp value=%{public}d", experienceType.c_str(),
                experienceValue);
        }
    } catch (nlohmann::json::exception &e) {
        LOG_ERROR("get scene exp, want info is null, type=%{public}s,exp value=%{public}d, json err:%{public}s",
            experienceType.c_str(), experienceValue, e.what());
        return -1;
    }
    return 0;
}

int32_t EcologicalRuleEngineCache::CheckPkgNameInCardOwner(const std::string &pkgName, bool &findPkg)
{
    try {
        findPkg = false;
        std::lock_guard<std::mutex> autoLock(sceneConfigDataLock_);
        auto itRuleConfig = sceneConfigData.find(EngineCacheConstants::SCENE_DATA_KEY_RULE_CONFIG);
        if (itRuleConfig == sceneConfigData.end()) {
            LOG_ERROR("not find rule config, pkg=%{public}s", pkgName.c_str());
            return -1;
        }

        nlohmann::json &jsonRuleConfig = itRuleConfig.value();
        auto itCardOwener = jsonRuleConfig.find(EngineCacheConstants::SCENE_DATA_KEY_CARD_OWENERS);
        if (itCardOwener == jsonRuleConfig.end()) {
            LOG_ERROR("not find addCardOwners, pkg=%{public}s", pkgName.c_str());
            return -1;
        }

        nlohmann::json &jsonCardOwner = itCardOwener.value();
        for (auto iter = jsonCardOwner.begin(); iter != jsonCardOwner.end(); iter++) {
            nlohmann::json &arrayItem = *iter;
            auto r1 = arrayItem.find(EngineCacheConstants::CARD_OWENERS_KEY_PKG_NAME);
            if (r1 != arrayItem.end() && r1.value() == pkgName) {
                LOG_DEBUG("find, pkg=%{public}s", pkgName.c_str());
                findPkg = true;
                break;
            }
        }
    } catch (nlohmann::json::exception &e) {
        LOG_ERROR("pkg=%{public}s, error=%{public}s", pkgName.c_str(), e.what());
        return -1;
    }
    return 0;
}

// inner func, no lock, caller locks self
int32_t EcologicalRuleEngineCache::ConstructSceneConfigDataJson(const std::string &ruleConfig,
    const std::string &sceneExperience, nlohmann::json &outputJson, std::string &configVersion)
{
    try {
        // 1 key  "ruleConfig"
        nlohmann::json completeJson = nlohmann::json::parse(ruleConfig, nullptr, false);
        if (completeJson.is_discarded()) {
            LOG_ERROR("construct, configVer=%{public}s, json is invalid", configVersion.c_str());
            return -1;
        }

        // 2 get ConfigVersion
        auto verItem = completeJson.find(EngineCacheConstants::SCENE_DATA_KEY_CONFIG_VER);
        if (verItem == completeJson.end()) {
            LOG_DEBUG("refresh scene_config_data, construct, can not find version item");
            return -1;
        }
        configVersion = verItem.value();

        // 3 key "sceneExperience"
        nlohmann::json jsonExperience = nlohmann::json::parse(sceneExperience, nullptr, false);
        if (jsonExperience.is_discarded()) {
            LOG_ERROR("construct, sceneExperience=%{public}s, json is invalid", sceneExperience.c_str());
            return -1;
        }

        // 4 complete json
        completeJson.insert(jsonExperience.begin(), jsonExperience.end());
        outputJson = completeJson;
    } catch (nlohmann::json::exception &e) {
        LOG_ERROR("construct, configVer=%{public}s, json error=%{public}s", configVersion.c_str(), e.what());
        return -1;
    }
    return 0;
}

void EcologicalRuleEngineCache::SetExemptExperience(const std::string &targetBundleName, const int32_t rule,
    const int32_t timeout)
{
    struct ExemptExperienceValue exemptValue;
    exemptValue.rule = rule;
    exemptValue.timeout = timeout;
    exemptValue.timestamp = GetMillisecondsOfDay();
    std::lock_guard<std::mutex> autoLock(exemptCacheLock_);
    exemptExperienceCache[targetBundleName] = exemptValue;
    LOG_DEBUG("set exempt experience, pkg=%{public}s, rule=%{public}d, timeout=%{public}d", targetBundleName.c_str(),
        rule, timeout);
}

void EcologicalRuleEngineCache::GetExemptExperience(const std::string &targetBundleName, int32_t &rule, bool &bExempt)
{
    std::map<std::string, struct ExemptExperienceValue>::iterator mapIter;
    uint64_t curTime = GetMillisecondsOfDay();
    std::lock_guard<std::mutex> autoLock(exemptCacheLock_);
    mapIter = exemptExperienceCache.find(targetBundleName);
    if (mapIter != exemptExperienceCache.end() &&
        (mapIter->second.timestamp + static_cast<uint64_t>(mapIter->second.timeout) > curTime)) {
        bExempt = true;
        rule = mapIter->second.rule;
    } else {
        bExempt = false;
    }
    LOG_DEBUG("get exempt experience, pkg=%{public}s, rule=%{public}d, Exempt=%{public}d", targetBundleName.c_str(),
        rule, bExempt);
}

// sceneCode cache,  clear 60m
void EcologicalRuleEngineCache::SetSceneCode(const std::string &bundleName, const std::string &sceneCode)
{
    struct SceneCodeValue sceneCodeValue;
    sceneCodeValue.sceneCode = sceneCode;
    sceneCodeValue.timestamp = GetMillisecondsOfDay();
    std::map<std::string, std::list<struct SceneCodeValue>>::iterator mapIter;

    std::lock_guard<std::mutex> autoLock(sceneCodeCacheLock_);
    mapIter = sceneCodeCache.find(bundleName);
    if (mapIter != sceneCodeCache.end()) { // find, push to list
        mapIter->second.emplace_back(sceneCodeValue);
    } else { // not find, insert new list
        std::list<struct SceneCodeValue> sceneCodeList;
        sceneCodeList.emplace_back(sceneCodeValue);
        sceneCodeCache.insert(std::pair<std::string, std::list<struct SceneCodeValue>>(bundleName, sceneCodeList));
    }
    ClearSceneCodeCache();
    LOG_DEBUG("set scece_code cache, pkg=%{public}s, sceneCode=%{public}s", bundleName.c_str(), sceneCode.c_str());
}

int32_t EcologicalRuleEngineCache::GetSceneCode(const std::string &bundleName, const int32_t &sceneCodeType,
    std::string &sceneCode)
{
    if (sceneCodeType != EngineCacheConstants::SCENE_CODE_TYPE_COLD &&
        sceneCodeType != EngineCacheConstants::SCENE_CODE_TYPE_HOT) {
        LOG_INFO("get scece_code type=%{public}d error", sceneCodeType);
        return -1;
    }
    std::map<std::string, std::list<struct SceneCodeValue>>::iterator mapIter;
    std::lock_guard<std::mutex> autoLock(sceneCodeCacheLock_);
    mapIter = sceneCodeCache.find(bundleName);
    if (mapIter == sceneCodeCache.end()) { // not find
        LOG_DEBUG("get scece_code, not find, pkg=%{public}s, type=%{public}d", bundleName.c_str(), sceneCodeType);
        return -1;
    }
    std::list<struct SceneCodeValue> &sceneCodeList = mapIter->second;
    // hot boot scene-code
    if (sceneCodeType == EngineCacheConstants::SCENE_CODE_TYPE_HOT) {
        auto iterList = sceneCodeList.rbegin();
        if (iterList == sceneCodeList.rend()) {
            LOG_DEBUG("get scece_code, not find, pkg=%{public}s, type=%{public}d", bundleName.c_str(), sceneCodeType);
            return -1;
        } else {
            sceneCode = iterList->sceneCode;
            LOG_DEBUG("get scece_code success, pkg=%{public}s, type=%{public}d", bundleName.c_str(), sceneCodeType);
            return 0;
        }
    }

    // cold boot scene-code
    std::string tempSceneCode;
    uint64_t curTime = GetMillisecondsOfDay();
    for (auto iter = sceneCodeList.rbegin(); iter != sceneCodeList.rend(); iter++) {
        if (iter->timestamp + EngineCacheConstants::TIMER_INTERVAL_30MIN_MS > curTime) {
            tempSceneCode = iter->sceneCode;
        }
    }
    if (tempSceneCode.empty()) {
        LOG_DEBUG("get cold scece_code, not find, pkg=%{public}s, type=%{public}d", bundleName.c_str(), sceneCodeType);
        return -1;
    }
    sceneCode = tempSceneCode;
    LOG_DEBUG("get scece_code success, pkg=%{public}s, type=%{public}d", bundleName.c_str(), sceneCodeType);
    return 0;
}

// clear when write scene code
void EcologicalRuleEngineCache::ClearSceneCodeCache()
{
    uint64_t curTime = GetMillisecondsOfDay();
    for (auto iter = sceneCodeCache.begin(); iter != sceneCodeCache.end(); iter++) {
        std::list<struct SceneCodeValue> &sceneCodeList = iter->second;
        for (auto iterList = sceneCodeList.begin(); iterList != sceneCodeList.end();) {
            if (iterList->timestamp + EngineCacheConstants::TIMER_INTERVAL_60MIN_MS < curTime) {
                sceneCodeList.erase(iterList++);
            } else {
                iterList++;
            }
        }
    }
}

uint64_t EcologicalRuleEngineCache::GetMillisecondsOfDay()
{
    struct timespec tv = { 0, 0 };
    clock_gettime(CLOCK_MONOTONIC, &tv);
    uint64_t secs = static_cast<uint64_t>(tv.tv_sec);
    uint64_t msec = static_cast<uint64_t>(tv.tv_nsec / 1000 / 1000); // 1000 means unit
    return secs * 1000 + msec;                                       // 1000 means unit
}
} // namespace EcologicalRuleMgrService
} // namespace OHOS