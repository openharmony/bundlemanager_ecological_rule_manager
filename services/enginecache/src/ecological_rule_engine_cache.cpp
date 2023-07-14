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
        LOG_DEBUG("get data version, can not find the item");
        WriteFile(ruleControlStrategy, EngineCacheConstants::APP_RULE_OUT_FILE);
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
            WriteDebugFile(ruleInfo, EngineCacheConstants::APP_RULE_OUT_FILE);
            return -1;
        }
    } catch (nlohmann::json::parse_error &e) {
        LOG_ERROR("refresh rule strategy, json error=%{public}s", e.what());
        WriteDebugFile(ruleInfo, EngineCacheConstants::APP_RULE_OUT_FILE);
        return -1;
    }

    // write to file
    EcologicalRuleEngineCache::WriteFile(ruleControlStrategy, EngineCacheConstants::APP_RULE_STRATEGY_FILE);

    // find data version
    auto verItem = ruleControlStrategy.find(EngineCacheConstants::APP_ENHANCE_DATA_KEY_DATA_VER);
    if (verItem == ruleControlStrategy.end()) {
        LOG_WARN("refresh rule strategy, can't find dataversion item");
        WriteFile(ruleControlStrategy, EngineCacheConstants::APP_RULE_OUT_FILE);
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
// App Enhance Data
// lock all app_enhance_data, just for engine
void EcologicalRuleEngineCache::GetAppEnhanceDataLock()
{
    appEnhanceDataLock_.lock();
}
// release all app_enhance_data, just for engine
void EcologicalRuleEngineCache::ReleaseAppEnhanceDataLock()
{
    appEnhanceDataLock_.unlock();
}
int32_t EcologicalRuleEngineCache::GetVectorItem(nlohmann::json *&inputJson, const std::string &key,
    const std::string &value, nlohmann::json *&outputJson)
{
    if (inputJson == NULL) {
        LOG_ERROR("input is null, itemKey = %{public}s", key.c_str());
        return -1;
    }
    try {
        if (!inputJson->is_array()) {
            LOG_ERROR("input is not array, itemKey = %{public}s", key.c_str());
            WriteFile(*inputJson, EngineCacheConstants::APP_ENHANCE_OUT_FILE);
            return -1;
        }
        for (auto it1 = inputJson->begin(); it1 != inputJson->end(); it1++) {
            nlohmann::json &arrayItem = *it1;
            auto r2 = arrayItem.find(key);
            if (r2 != arrayItem.end() && r2.value() == value) {
                outputJson = &arrayItem;
                return 0;
            }
        }
    } catch (nlohmann::json::exception &e) {
        LOG_ERROR("itemKey = %{public}s, parser error=%{public}s", key.c_str(), e.what());
        WriteFile(*inputJson, EngineCacheConstants::APP_ENHANCE_OUT_FILE);
        return -1;
    }
    LOG_DEBUG("itemKey = %{public}s, not find", key.c_str());
    return -1;
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
            WriteFile(*inputJson, EngineCacheConstants::APP_ENHANCE_OUT_FILE);
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
    int32_t ret = -1;

    LOG_INFO("begin, operType = %{public}d", operType);

    if (appData.length() == 0) {
        LOG_ERROR("length of input app data is 0");
        return ret;
    }

    try {
        std::lock_guard<std::mutex> autoLock(appEnhanceDataLock_);
        if (operType == EngineCacheConstants::APP_ENHANCED_DATA_ALL_ADD ||
            operType == EngineCacheConstants::APP_ENHANCED_DATA_PART_ADD) {
            ret = AddAppEnhanceData(operType, appData);
        } else if (operType == EngineCacheConstants::APP_ENHANCED_DATA_DEL) {
            ret = DeletePartAppEnhanceData(appData);
        } else {
            LOG_ERROR("operType = %{public}d, invalid", operType);
            return -1;
        }
    } catch (nlohmann::json::exception &e) {
        LOG_ERROR("operType = %{public}d, input json parser error=%{public}s", operType, e.what());
        WriteDebugFile(appData, EngineCacheConstants::APP_ENHANCE_OUT_FILE);
        return -1;
    }
    LOG_INFO("refresh enhance success, operType = %{public}d", operType);
    return ret;
}

int32_t EcologicalRuleEngineCache::GetAppEnhanceItem(const std::string &pkgName, nlohmann::json *&outPutJson)
{
    if (appEnhanceCallerData.pkgName.compare(pkgName) == 0) {
        outPutJson = &(appEnhanceCallerData.appInfo);
        return 0;
    }
    if (appEnhanceWantData.pkgName.compare(pkgName) == 0) {
        outPutJson = &(appEnhanceWantData.appInfo);
        return 0;
    }
    return -1;
}

int32_t EcologicalRuleEngineCache::ParseAppEnhanceItem(const std::string &callerPkgName, const std::string &wantPkgName)
{
    std::map<std::string, std::string>::iterator callerIter;
    std::map<std::string, std::string>::iterator wantIter;
    int32_t ret = 0;

    callerIter = appEnhanceDataCache.find(callerPkgName);
    if (callerIter == appEnhanceDataCache.end()) {
        LOG_INFO("not find caller pkg=%{public}s info", callerPkgName.c_str());
        appEnhanceCallerData.pkgName = "";
        ret = -1;
    } else {
        try {
            appEnhanceCallerData.pkgName = callerPkgName;
            appEnhanceCallerData.appInfo = nlohmann::json::parse(callerIter->second, nullptr, false);
            if (appEnhanceCallerData.appInfo.is_discarded()) {
                LOG_ERROR("parse caller=%{public}s info failed, json is invalid", callerPkgName.c_str());
                ret = -1;
            } else {
                LOG_DEBUG("parse caller=%{public}s info success", callerPkgName.c_str());
            }
        } catch (nlohmann::json::exception &e) {
            LOG_ERROR("parse caller=%{public}s info failed, json err=%{public}s", callerPkgName.c_str(), e.what());
            ret = -1;
        }
    }

    wantIter = appEnhanceDataCache.find(wantPkgName);
    if (wantIter == appEnhanceDataCache.end()) {
        LOG_INFO("not find want pkg=%{public}s info", wantPkgName.c_str());
        appEnhanceWantData.pkgName = "";
        ret = -1;
    } else {
        try {
            appEnhanceWantData.pkgName = wantPkgName;
            appEnhanceWantData.appInfo = nlohmann::json::parse(wantIter->second, nullptr, false);
            if (appEnhanceWantData.appInfo.is_discarded()) {
                LOG_ERROR("parse want=%{public}s info failed, json is invalid", wantPkgName.c_str());
                ret = -1;
            } else {
                LOG_DEBUG("parse want=%{public}s info success", wantPkgName.c_str());
            }
        } catch (nlohmann::json::exception &e) {
            LOG_ERROR("parse want=%{public}s info failed, json err=%{public}s", wantPkgName.c_str(), e.what());
            ret = -1;
        }
    }

    return ret;
}

int32_t EcologicalRuleEngineCache::AddAppEnhanceData(const int32_t operType, const std::string &appData)
{
    // clear when add all
    if (operType == EngineCacheConstants::APP_ENHANCED_DATA_ALL_ADD) {
        appEnhanceDataCache.clear();
    }

    // insert app info to map
    int32_t count = 0;
    std::istringstream iStream(appData);
    std::string appItem;
    while (getline(iStream, appItem)) {
        std::string::size_type pos = appItem.find(EngineCacheConstants::APP_ENHANCE_DATA_SEPARATOR);
        if (pos != appItem.npos) {
            appEnhanceDataCache[appItem.substr(0, pos)] = appItem.substr(pos + 1);
            count++;
        } else {
            LOG_WARN("read app enhance string = %{public}s err, no separator, count = %{public}d", appItem.c_str(),
                count);
        }
    }
    LOG_INFO("after addCount = %{public}d, all_item = %{public}ld", count, appEnhanceDataCache.size());
    // write file
    WriteAppEnhanceFile();
    return 0;
}

void EcologicalRuleEngineCache::DeleteOneAppEnhanceData(const std::string &pkgName)
{
    std::map<std::string, std::string>::iterator mapIter = appEnhanceDataCache.find(pkgName);
    if (mapIter == appEnhanceDataCache.end()) {
        LOG_DEBUG("pkg:%{public}s not exist", pkgName.c_str());
        return;
    }
    appEnhanceDataCache.erase(mapIter);
}

int32_t EcologicalRuleEngineCache::DeletePartAppEnhanceData(const std::string &appData)
{
    // delete item in map
    std::istringstream iStream(appData);
    std::string appItem;
    while (getline(iStream, appItem)) {
        std::string::size_type pos = appItem.find(EngineCacheConstants::APP_ENHANCE_DATA_SEPARATOR);
        if (pos != appItem.npos) {
            std::string packageName = appItem.substr(0, pos);
            DeleteOneAppEnhanceData(packageName);
        } else {
            LOG_WARN("read app enhance string=%{public}s err, no separator", appItem.c_str());
        }
    }

    // write file
    WriteAppEnhanceFile();
    return 0;
}

int32_t EcologicalRuleEngineCache::RefreshSceneConfigData(const std::string &ruleConfig,
    const std::string &sceneExperience)
{
    int32_t ret = 0;
    std::string configVersion;
    LOG_INFO("begin");
    LOG_DEBUG("ruleConfig=%{public}s", ruleConfig.c_str());
    std::lock_guard<std::mutex> autoLock(sceneConfigDataLock_);
    ret = EcologicalRuleEngineCache::ConstructSceneConfigDataJson(ruleConfig, sceneExperience, sceneConfigData,
        configVersion);
    if (ret != 0) {
        return -1;
    }
    LOG_INFO("refresh to cache success, dataVer = %{public}s", configVersion.c_str());

    EcologicalRuleEngineCache::WriteFile(sceneConfigData, EngineCacheConstants::SCENE_CONFIG_DATA_FILE);
    LOG_INFO("write file success, dataVer = %{public}s", configVersion.c_str());

    EcologicalRuleEngineCache::EraseConfigDataAdsInfo(sceneConfigData, configVersion);
    LOG_INFO("erase config AdsInfo end, dataVer = %{public}s", configVersion.c_str());
    return 0;
}

int32_t EcologicalRuleEngineCache::GetSceneConfigDataVersion(std::string &configVersion)
{
    std::lock_guard<std::mutex> autoLock(sceneConfigDataLock_);
    auto configver = sceneConfigData.find(EngineCacheConstants::SCENE_DATA_KEY_CONFIG_VER);
    if (configver == sceneConfigData.end()) {
        LOG_DEBUG("get config data version, not find");
        WriteFile(sceneConfigData, EngineCacheConstants::SCENE_CONFIG_OUT_FILE);
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
            WriteFile(sceneConfigData, EngineCacheConstants::SCENE_CONFIG_OUT_FILE);
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
        WriteFile(sceneConfigData, EngineCacheConstants::SCENE_CONFIG_OUT_FILE);
        return -1;
    }
    return 0;
}

int32_t EcologicalRuleEngineCache::GetPublicKeyByNetworkID(const std::string &networkId, std::string &publicKey,
    int32_t &platformType)
{
    try {
        std::lock_guard<std::mutex> autoLock(sceneConfigDataLock_);
        auto itRuleConfig = sceneConfigData.find(EngineCacheConstants::SCENE_DATA_KEY_RULE_CONFIG);
        if (itRuleConfig == sceneConfigData.end()) {
            LOG_ERROR("not find rule config, networkId=%{public}s", networkId.c_str());
            WriteFile(sceneConfigData, EngineCacheConstants::SCENE_CONFIG_OUT_FILE);
            return -1;
        }

        nlohmann::json &jsonRuleConfig = itRuleConfig.value();
        auto itAdsInfo = jsonRuleConfig.find(EngineCacheConstants::SCENE_DATA_KEY_ADS_INFOS);
        if (itAdsInfo == jsonRuleConfig.end()) {
            LOG_ERROR("not find adsInfos, networkId=%{public}s", networkId.c_str());
            WriteFile(sceneConfigData, EngineCacheConstants::SCENE_CONFIG_OUT_FILE);
            return -1;
        }

        nlohmann::json &jsonAdsInfo = itAdsInfo.value();
        for (auto iter = jsonAdsInfo.begin(); iter != jsonAdsInfo.end(); iter++) {
            nlohmann::json &arrayItem = *iter;

            auto r1 = arrayItem.find(EngineCacheConstants::ADS_INFOS_KEY_AD_NETWORK_ID);
            if (r1 == arrayItem.end() || r1.value() != networkId) {
                continue;
            }
            auto r2 = arrayItem.find(EngineCacheConstants::ADS_INFOS_KEY_PUBLIC_KEY);
            if (r2 == arrayItem.end()) {
                LOG_INFO("not find ads_key, networkId=%{public}s", networkId.c_str());
                return -1;
            }
            publicKey = r2.value();

            auto r3 = arrayItem.find(EngineCacheConstants::ADS_INFOS_KEY_PLATFORM_TYPE);
            if (r3 == arrayItem.end()) {
                LOG_INFO("not find platformType, networkId=%{public}s", networkId.c_str());
                return -1;
            }
            platformType = r3.value();
            break;
        }
    } catch (nlohmann::json::exception &e) {
        LOG_ERROR("exception,networkId=%{public}s,err=%{public}s", networkId.c_str(), e.what());
        WriteFile(sceneConfigData, EngineCacheConstants::SCENE_CONFIG_OUT_FILE);
        return -1;
    }
    LOG_DEBUG("find adsInfos, networkId=%{public}s", networkId.c_str());
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
            WriteFile(sceneConfigData, EngineCacheConstants::SCENE_CONFIG_OUT_FILE);
            return -1;
        }

        nlohmann::json &jsonRuleConfig = itRuleConfig.value();
        auto itCardOwener = jsonRuleConfig.find(EngineCacheConstants::SCENE_DATA_KEY_CARD_OWENERS);
        if (itCardOwener == jsonRuleConfig.end()) {
            LOG_ERROR("not find addCardOwners, pkg=%{public}s", pkgName.c_str());
            WriteFile(sceneConfigData, EngineCacheConstants::SCENE_CONFIG_OUT_FILE);
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
        WriteFile(sceneConfigData, EngineCacheConstants::SCENE_CONFIG_OUT_FILE);
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
            WriteDebugFile(ruleConfig + sceneExperience, EngineCacheConstants::SCENE_CONFIG_OUT_FILE);
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
            WriteDebugFile(ruleConfig + sceneExperience, EngineCacheConstants::SCENE_CONFIG_OUT_FILE);
            return -1;
        }

        // 4 complete json
        completeJson.insert(jsonExperience.begin(), jsonExperience.end());
        outputJson = completeJson;
    } catch (nlohmann::json::exception &e) {
        LOG_ERROR("construct, configVer=%{public}s, json error=%{public}s", configVersion.c_str(), e.what());
        WriteDebugFile(ruleConfig + sceneExperience, EngineCacheConstants::SCENE_CONFIG_OUT_FILE);
        return -1;
    }
    return 0;
}

// erase adsInfo when platformType not in {1,2}
void EcologicalRuleEngineCache::EraseConfigDataAdsInfo(nlohmann::json &configDataJson, const std::string &configVersion)
{
    try {
        auto itRuleConfig = configDataJson.find(EngineCacheConstants::SCENE_DATA_KEY_RULE_CONFIG);
        if (itRuleConfig == configDataJson.end()) {
            LOG_ERROR("erase AdsInfo, not find rule config, configVer=%{public}s", configVersion.c_str());
            return;
        }

        nlohmann::json &jsonAdsInfo = itRuleConfig.value();
        auto itAdsInfo = jsonAdsInfo.find(EngineCacheConstants::SCENE_DATA_KEY_ADS_INFOS);
        if (itAdsInfo == jsonAdsInfo.end()) {
            LOG_ERROR("erase AdsInfo, not find adsInfos, configVer=%{public}s", configVersion.c_str());
            return;
        }

        nlohmann::json::array_t &adsArray = (*itAdsInfo).get_ref<nlohmann::json::array_t &>();
        bool erased = false;
        do {
            erased = EraseOneItemInAdsInfo(adsArray);
        } while (erased);
    } catch (nlohmann::json::exception &e) {
        LOG_ERROR("erase AdsInfos, configVer=%{public}s, json err=%{public}s", configVersion.c_str(), e.what());
    }
    return;
}

// erase adsInfo one item
bool EcologicalRuleEngineCache::EraseOneItemInAdsInfo(nlohmann::json::array_t &jsonAdsArray)
{
    for (auto itArray = jsonAdsArray.begin(); itArray != jsonAdsArray.end(); itArray++) {
        nlohmann::json &arrayItem = *itArray;
        auto r1 = arrayItem.find(EngineCacheConstants::ADS_INFOS_KEY_PLATFORM_TYPE);
        if (r1 != arrayItem.end() && r1.value() != EngineCacheConstants::ADS_INFOS_PLATFORM_TYPE_AGDS &&
            r1.value() != EngineCacheConstants::ADS_INFOS_PLATFORM_TYPE_PPS) {
            LOG_INFO("erase adsInfo, type=%{public}d", r1.value().get<int32_t>());
            jsonAdsArray.erase(itArray);
            return true;
        }
    }
    return false;
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
                LOG_DEBUG("erase scene_code,bundle:%{public}s,sceneCode:%{public}s,timestamp:%{public}lu",
                    iter->first.c_str(), iterList->sceneCode.c_str(), iterList->timestamp);
                sceneCodeList.erase(iterList++);
            } else {
                iterList++;
            }
        }
    }
}

void SceneRequestCacheClearCallback();
int32_t EcologicalRuleEngineCache::SetExperienceConfig(const std::string &sceneKey, const std::string &sceneCode,
    const std::string &experience)
{
    struct SceneRequestValue requestValue;
    requestValue.sceneCode = sceneCode;
    requestValue.experience = experience;
    requestValue.timestamp = GetMillisecondsOfDay();
    std::lock_guard<std::mutex> autoLock(sceneRequstCacheLock_);
    sceneRequestCache[sceneKey] = requestValue;
    LOG_DEBUG("set exp config, sceneCode:%{public}s, timestamp:%{public}lu", sceneCode.c_str(), requestValue.timestamp);
    return 0;
}

// return 0 means find, other means not find
int32_t EcologicalRuleEngineCache::GetExperienceConfig(const std::string &sceneKey, std::string &sceneCode,
    std::string &experience)
{
    std::map<std::string, struct SceneRequestValue>::iterator mapIter;
    uint64_t curTime = GetMillisecondsOfDay();
    std::lock_guard<std::mutex> autoLock(sceneRequstCacheLock_);
    mapIter = sceneRequestCache.find(sceneKey);
    if (mapIter == sceneRequestCache.end()) {
        return -1;
    }
    if (mapIter->second.timestamp + EngineCacheConstants::TIMER_INTERVAL_5S_MS > curTime) {
        sceneCode = mapIter->second.sceneCode;
        experience = mapIter->second.experience;
        return 0;
    } else {
        LOG_DEBUG("Get exp, erase exp config, sceneCode:%{public}s,timestamp:%{public}lu",
            mapIter->second.sceneCode.c_str(), mapIter->second.timestamp);
        sceneRequestCache.erase(mapIter);
        return -1;
    }
}

// load config-data
int32_t EcologicalRuleEngineCache::LoadAppEnhanceDataFile()
{
    std::ifstream iStream(EngineCacheConstants::APP_ENHANCE_DATA_FILE);
    int32_t count = 0;

    try {
        if (!iStream.is_open()) {
            LOG_ERROR("read file, open file=%{public}s failed", EngineCacheConstants::APP_ENHANCE_DATA_FILE.c_str());
            return -1;
        }
        appEnhanceDataCache.clear();
        std::string appItem;
        while (getline(iStream, appItem)) {
            std::string::size_type pos = appItem.find(EngineCacheConstants::APP_ENHANCE_DATA_SEPARATOR);
            if (pos != appItem.npos) {
                appEnhanceDataCache[appItem.substr(0, pos)] = appItem.substr(pos + 1);
                count++;
            } else {
                LOG_WARN("read file=%{public}s err, no separator,count=%{public}d",
                    EngineCacheConstants::APP_ENHANCE_DATA_FILE.c_str(), count);
            }
        }
    } catch (std::exception &e) {
        LOG_ERROR("read file=%{public}s failed, err=%{public}s", EngineCacheConstants::APP_ENHANCE_DATA_FILE.c_str(),
            e.what());
    }
    iStream.close();
    LOG_INFO("read file=%{public}s success, app count=%{public}d", EngineCacheConstants::APP_ENHANCE_DATA_FILE.c_str(),
        count);
    return 0;
}

void EcologicalRuleEngineCache::WriteAppEnhanceFile()
{
    uint64_t beginTime = EcologicalRuleEngineCache::GetMillisecondsOfDay();
    std::ofstream outStream(EngineCacheConstants::APP_ENHANCE_DATA_FILE);
    try {
        int32_t count = 0;
        if (!outStream.is_open()) {
            LOG_ERROR("write enhance file=%{public}s,open failed", EngineCacheConstants::APP_ENHANCE_DATA_FILE.c_str());
            return;
        }
        for (auto iter = appEnhanceDataCache.begin(); iter != appEnhanceDataCache.end(); iter++) {
            if (iter == appEnhanceDataCache.begin()) {
                outStream << iter->first << EngineCacheConstants::APP_ENHANCE_DATA_SEPARATOR << iter->second;
            } else {
                outStream << std::endl;
                outStream << iter->first << EngineCacheConstants::APP_ENHANCE_DATA_SEPARATOR << iter->second;
            }
            if (outStream.bad()) {
                LOG_ERROR("write enhance file=%{public}s,have err",
                    EngineCacheConstants::APP_ENHANCE_DATA_FILE.c_str());
                break;
            }
            count++;
        }
        LOG_INFO("write enhance file=%{public}s, app count=%{public}d.",
            EngineCacheConstants::APP_ENHANCE_DATA_FILE.c_str(), count);
    } catch (std::exception &e) {
        LOG_ERROR("write enhance file=%{public}s failed, err=%{public}s",
            EngineCacheConstants::APP_ENHANCE_DATA_FILE.c_str(), e.what());
    }
    outStream.close();
    uint64_t endTime = EcologicalRuleEngineCache::GetMillisecondsOfDay();
    LOG_DEBUG("write file =%{public}s, size=%{public}ld, cost=%{public}lu",
        EngineCacheConstants::APP_ENHANCE_DATA_FILE.c_str(), appEnhanceDataCache.size(), endTime - beginTime);
}

// locked by caller
void EcologicalRuleEngineCache::LoadRuleStrategyFile()
{
    int32_t ret = 0;
    ret = ReadFile(EngineCacheConstants::APP_RULE_STRATEGY_FILE, ruleControlStrategy);
    if (ret != 0) {
        LOG_INFO("load file=%{public}s fail", EngineCacheConstants::APP_RULE_STRATEGY_FILE.c_str());
    } else {
        LOG_INFO("load rule strategy file=%{public}s end", EngineCacheConstants::APP_RULE_STRATEGY_FILE.c_str());
    }
}

void EcologicalRuleEngineCache::LoadAppEnhanceFile()
{
    int32_t ret = 0;
    std::lock_guard<std::mutex> autoLock(appEnhanceDataLock_);
    ret = LoadAppEnhanceDataFile();
    if (ret != 0) {
        LOG_INFO("load file=%{public}s fail", EngineCacheConstants::APP_ENHANCE_DATA_FILE.c_str());
    } else {
        LOG_INFO("load app enhance file=%{public}s end", EngineCacheConstants::APP_ENHANCE_DATA_FILE.c_str());
    }
}

void EcologicalRuleEngineCache::LoadSceneConfigFile()
{
    int32_t ret = 0;

    std::lock_guard<std::mutex> autoLock(sceneConfigDataLock_);
    ret = ReadFile(EngineCacheConstants::SCENE_CONFIG_DATA_FILE, sceneConfigData);
    if (ret != 0) {
        LOG_INFO("load file=%{public}s fail", EngineCacheConstants::SCENE_CONFIG_DATA_FILE.c_str());
    } else {
        LOG_INFO("load scene experience file=%{public}s end", EngineCacheConstants::SCENE_CONFIG_DATA_FILE.c_str());
        auto ruleconfig = sceneConfigData.find(EngineCacheConstants::SCENE_DATA_KEY_CONFIG_VER);
        if (ruleconfig == sceneConfigData.end()) {
            LOG_ERROR("load scene experience , config data version, not find");
            return;
        }
        EraseConfigDataAdsInfo(sceneConfigData, ruleconfig.value());
    }
}
// file oper
void EcologicalRuleEngineCache::WriteFile(const nlohmann::json &inputJson, const std::string &fileName)
{
    uint64_t beginTime = EcologicalRuleEngineCache::GetMillisecondsOfDay();
    std::ofstream outStream(fileName);
    try {
        if (!outStream.is_open()) {
            LOG_ERROR("open file=%{public}s failed", fileName.c_str());
            return;
        }
        outStream << inputJson;
        if (outStream.bad()) {
            LOG_ERROR("write file have err, file=%{public}s", fileName.c_str());
        }
    } catch (std::exception &e) {
        LOG_ERROR("write file =%{public}s failed, err=%{public}s", fileName.c_str(), e.what());
    }
    outStream.close();
    uint64_t endTime = EcologicalRuleEngineCache::GetMillisecondsOfDay();
    LOG_DEBUG("write file =%{public}s cost=%{public}lu", fileName.c_str(), endTime - beginTime);
}

int32_t EcologicalRuleEngineCache::ReadFile(const std::string &fileName, nlohmann::json &outputJson)
{
    uint64_t beginTime = EcologicalRuleEngineCache::GetMillisecondsOfDay();
    std::ifstream iStream(fileName);
    int32_t ret = 0;
    try {
        if (!iStream.is_open()) {
            LOG_ERROR("read file, open file=%{public}s failed", fileName.c_str());
            return -1;
        }
        iStream >> outputJson;
        if (iStream.bad()) {
            LOG_ERROR("read file have err, file=%{public}s", fileName.c_str());
            ret = -1;
        }
    } catch (std::exception &e) {
        LOG_ERROR("read file=%{public}s failed, err=%{public}s", fileName.c_str(), e.what());
        ret = -1;
    }
    iStream.close();
    uint64_t endTime = EcologicalRuleEngineCache::GetMillisecondsOfDay();
    LOG_DEBUG("read file =%{public}s cost=%{public}lu", fileName.c_str(), endTime - beginTime);
    return ret;
}
void EcologicalRuleEngineCache::WriteDebugFile(const std::string &input, const std::string &fileName)
{
    std::ofstream outStream(fileName);
    if (!outStream.is_open()) {
        LOG_ERROR("write file, open dbg file=%{public}s failed", fileName.c_str());
        return;
    }
    outStream << input;
    if (outStream.bad()) {
        LOG_ERROR("write dbg file have err, file=%{public}s", fileName.c_str());
    }
    outStream.close();
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