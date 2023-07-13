/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <chrono>
#include <thread>
#include "ecological_rule_mgr_service.h"
#include "system_ability_definition.h"
#include "iservice_registry.h"
#include "if_system_ability_manager.h"
#include "uri.h"
#include "want.h"
#include "ecological_rule_mgr_service_logger.h"
#include "ecological_rule_engine_cache.h"
#include "ecological_rule_calculator_impl.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
#define TAG "ERMS_MAIN"
static inline const std::string ERMS_ENGINE_VERSION = "1";
static inline const std::string ERMS_EXPERIENCE_TARGET = "ecological_experience_target_package_name";
static inline const std::string ERMS_EXPERIENCE_CALLER_UID = "ecological_experience_original_uid";
static inline const std::string ERMS_EXPERIENCE_CALLER_PID = "ecological_experience_original_pid";
static inline const std::string ERMS_SERVICE_VERSION = "1.0.0.B001";

REGISTER_SYSTEM_ABILITY_BY_ID(EcologicalRuleMgrService, 6105, true);

std::mutex EcologicalRuleMgrService::instanceLock_;
sptr<EcologicalRuleMgrService> EcologicalRuleMgrService::instance_;

EcologicalRuleMgrService::EcologicalRuleMgrService() : state_(ServiceRunningState::STATE_NOT_START)
{
    LOG_INFO("EcologicalRuleMgrService constructed without params, version:%{public}s", ERMS_SERVICE_VERSION.c_str());
}

EcologicalRuleMgrService::EcologicalRuleMgrService(int32_t saId, bool runOnCreate)
    : SystemAbility(saId, runOnCreate), state_(ServiceRunningState::STATE_NOT_START)
{
    LOG_INFO("EcologicalRuleMgrService constructed, version:%{public}s", ERMS_SERVICE_VERSION.c_str());
}

EcologicalRuleMgrService::~EcologicalRuleMgrService() {}

sptr<EcologicalRuleMgrService> EcologicalRuleMgrService::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> autoLock(instanceLock_);
        if (instance_ == nullptr) {
            instance_ = new EcologicalRuleMgrService;
        }
    }
    return instance_;
}

int32_t EcologicalRuleMgrService::QueryFreeInstallExperience(const Want &want, const CallerInfo &callerInfo,
    ExperienceRule &rule)
{
    std::string bundleName = want.GetBundle();
    LOG_DEBUG("want name = %{public}s, uri = %{public}s, callerPackageName = %{public}s, callerAppType = %{public}d, "
        "targetAppType = %{public}d",
        bundleName.c_str(), want.GetUriString().c_str(), callerInfo.packageName.c_str(), callerInfo.callerAppType,
        callerInfo.targetAppType);

    int32_t ret = EcologicalRuleCalculatorImpl::GetInstance()->QueryMatchSceneExperience(want, callerInfo,
        EXPERIENCE_TYPE_LOAD, rule);
    if (ret != SUCCESS) {
        LOG_ERROR("QueryMatchSceneExperience failed: %{public}d", ret);
        return ERROR;
    }
    if (rule.replaceWant == nullptr || rule.replaceWant->GetAction().empty()) {
        rule.replaceWant = nullptr;
        LOG_INFO("rule.isAllow = %{public}d, rule.sceneCode = %{public}s, want is null", rule.isAllow,
            rule.sceneCode.c_str());
        return SUCCESS;
    }

    rule.replaceWant->SetParam(ERMS_EXPERIENCE_TARGET, bundleName);
    rule.replaceWant->SetParam(ERMS_EXPERIENCE_CALLER_UID, callerInfo.uid);
    rule.replaceWant->SetParam(ERMS_EXPERIENCE_CALLER_PID, callerInfo.pid);
    LOG_INFO("rule.isAllow = %{public}d, rule.sceneCode = %{public}s, replaceWant = %{public}s", rule.isAllow,
        rule.sceneCode.c_str(), (*(rule.replaceWant)).ToString().c_str());
    return SUCCESS;
}

int32_t EcologicalRuleMgrService::EvaluateResolveInfos(const Want &want, const CallerInfo &callerInfo, int32_t type,
    std::vector<AbilityInfo> &abilityInfos)
{
    LOG_DEBUG("want name = %{public}s, caller name = %{public}s, type = %{public}d", want.GetBundle().c_str(),
        callerInfo.packageName.c_str(), type);
    return 0;
}

int32_t EcologicalRuleMgrService::QueryStartExperience(const Want &want, const CallerInfo &callerInfo,
    ExperienceRule &rule)
{
    std::string bundleName = want.GetBundle();
    LOG_INFO("want bundle name = %{public}s, uri = %{public}s", bundleName.c_str(), want.GetUriString().c_str());
    LOG_INFO("callerInfo bundle name = %{public}s, callerAppType = %{public}d, targetAppType = %{public}d",
        callerInfo.packageName.c_str(), callerInfo.callerAppType, callerInfo.targetAppType);

    int32_t ret = EcologicalRuleCalculatorImpl::GetInstance()->QueryMatchSceneExperience(want, callerInfo,
        EXPERIENCE_TYPE_OPEN, rule);
    if (ret != SUCCESS) {
        LOG_ERROR("QueryMatchSceneExperience failed: %{public}d", ret);
        return ERROR;
    }
    if (rule.replaceWant == nullptr || rule.replaceWant->GetAction().empty()) {
        rule.replaceWant = nullptr;
        LOG_INFO("rule isAllow = %{public}d, sceneCode = %{public}s, no action", rule.isAllow, rule.sceneCode.c_str());
        return SUCCESS;
    }

    rule.replaceWant->SetParam(ERMS_EXPERIENCE_TARGET, bundleName);
    rule.replaceWant->SetParam(ERMS_EXPERIENCE_CALLER_UID, callerInfo.uid);
    rule.replaceWant->SetParam(ERMS_EXPERIENCE_CALLER_PID, callerInfo.pid);
    LOG_INFO("rule.isAllow = %{public}d, rule.sceneCode = %{public}s, replaceWant = %{public}s", rule.isAllow,
        rule.sceneCode.c_str(), (*(rule.replaceWant)).ToString().c_str());

    return SUCCESS;
}

int32_t EcologicalRuleMgrService::IsSupportPublishForm(const std::vector<Want> &wants, const CallerInfo &callerInfo,
    bool &bSupport)
{
    LOG_DEBUG("IsSupportPublishForm start, print all want info");
    for (size_t i = 0; i < wants.size(); i++) {
        LOG_DEBUG("bundleName = %{public}s, uri = %{public}s", wants[i].GetBundle().c_str(),
            wants[i].GetUriString().c_str());
    }

    std::string callerName = callerInfo.packageName;
    LOG_DEBUG("caller bundleName = %{public}s, callerType = %{public}d", callerName.c_str(), callerInfo.callerAppType);
    int32_t ret = EcologicalRuleEngineCache::GetInstance().CheckPkgNameInCardOwner(callerName, bSupport);
    if (ret != SUCCESS) {
        bSupport = true;
        LOG_ERROR("CheckPkgNameInCardOwner failed: %{public}d, bSupport = true", ret);
        return ERROR;
    }
    LOG_DEBUG("IsSupportPublishForm end, bSupport = %{public}d", bSupport);
    return SUCCESS;
}

int32_t EcologicalRuleMgrService::SetAppEnhancedData(const int32_t &operType, const std::string &appData)
{
    LOG_DEBUG("SetAppEnhancedData start, operType = %{public}d, appData = %{public}s", operType, appData.c_str());
    int32_t res = EcologicalRuleEngineCache::GetInstance().RefreshAppEnhanceData(operType, appData);
    if (res != SUCCESS) {
        LOG_ERROR("RefreshAppEnhanceData fail: %{public}d", res);
        return ERROR;
    }
    LOG_DEBUG("SetAppEnhancedData end");
    return SUCCESS;
}

int32_t EcologicalRuleMgrService::SetRuleInfo(const std::string &ruleInfo)
{
    LOG_DEBUG("SetRuleInfo ruleInfo = %{public}s", ruleInfo.c_str());
    EcologicalRuleEngineCache::GetInstance().GetRuleControlStrategyDataLock();
    int32_t res = EcologicalRuleEngineCache::GetInstance().RefreshRuleControlStrategy(ruleInfo);
    if (res != SUCCESS) {
        LOG_ERROR("RefreshRuleControlStrategy fail: %{public}d", res);
        EcologicalRuleEngineCache::GetInstance().ReleaseRuleControlStrategyDataLock();
        return ERROR;
    }
    EcologicalRuleCalculatorImpl::GetInstance()->ConstructRuleStrategyObjects();
    EcologicalRuleEngineCache::GetInstance().ClearRuleControlStrategyCache();
    EcologicalRuleEngineCache::GetInstance().ReleaseRuleControlStrategyDataLock();
    LOG_DEBUG("SetRuleInfo end");
    return SUCCESS;
}

int32_t EcologicalRuleMgrService::ExemptExperience(const std::string &targetBundleName, const int32_t &rule,
    const int32_t &timestamp)
{
    LOG_DEBUG("targetBundleName = %{public}s, rule = %{public}d, timestamp = %{public}d", targetBundleName.c_str(),
        rule, timestamp);
    EcologicalRuleEngineCache::GetInstance().SetExemptExperience(targetBundleName, rule, timestamp);
    return SUCCESS;
}

int32_t EcologicalRuleMgrService::GetVersion(std::string &versionJson)
{
    std::string configVersion;
    int32_t res = EcologicalRuleEngineCache::GetInstance().GetSceneConfigDataVersion(configVersion);
    if (res != SUCCESS) {
        LOG_INFO("get configVersion fail");
        configVersion = "1";
    }
    std::string dataVersion;
    int32_t res2 = EcologicalRuleEngineCache::GetInstance().GetRuleStrategyDataVersion(dataVersion);
    if (res2 != SUCCESS) {
        LOG_INFO("get dataVersion fail");
        dataVersion = "1";
    }
    versionJson = "{\"engineVersion\":" + ERMS_ENGINE_VERSION + ",\"dataVersion\":" + dataVersion +
        ",\"configVersion\":" + configVersion + "}";
    LOG_INFO("Service version: %{public}s, versionJson: %{public}s", ERMS_SERVICE_VERSION.c_str(), versionJson.c_str());
    return SUCCESS;
}

int32_t EcologicalRuleMgrService::SetSceneExperience(std::string &ruleConfig, std::string &sceneExperience)
{
    int32_t res = EcologicalRuleEngineCache::GetInstance().RefreshSceneConfigData(ruleConfig, sceneExperience);
    LOG_DEBUG("ruleConfig = %{public}s, sceneExperience = %{public}s", ruleConfig.c_str(), sceneExperience.c_str());
    if (res != SUCCESS) {
        LOG_ERROR("RefreshSceneConfigData fail: %{public}d", res);
        return ERROR;
    }
    return SUCCESS;
}

int32_t EcologicalRuleMgrService::GetSceneCode(const std::string &bundleName, std::string &sceneCode)
{
    int32_t res = EcologicalRuleEngineCache::GetInstance().GetSceneCode(bundleName, 2, sceneCode);
    if (res != SUCCESS) {
        LOG_ERROR("GetSceneCode fail: %{public}d", res);
        return ERROR;
    }
    LOG_INFO("bundleName = %{public}s, sceneCode = %{public}s", bundleName.c_str(), sceneCode.c_str());
    return SUCCESS;
}

int32_t EcologicalRuleMgrService::GetInitialSceneCode(const std::string &bundleName, std::string &originalSceneCode)
{
    int32_t res = EcologicalRuleEngineCache::GetInstance().GetSceneCode(bundleName, 1, originalSceneCode);
    if (res != SUCCESS) {
        LOG_ERROR("GetInitialSceneCode fail: %{public}d", res);
        return ERROR;
    }
    LOG_INFO("bundleName = %{public}s, sceneCode = %{public}s", bundleName.c_str(), originalSceneCode.c_str());
    return SUCCESS;
}

std::string GetBundleNameByUid(const int32_t uid)
{
    std::string bundleName = "";
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> object = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    sptr<AppExecFwk::IBundleMgr> iBundleMgr = iface_cast<AppExecFwk::IBundleMgr>(object);
    if (!iBundleMgr) {
        LOG_ERROR("null bundle manager.");
        return bundleName;
    }

    ErrCode ret = iBundleMgr->GetNameForUid(uid, bundleName);
    if (ret != ERR_OK) {
        LOG_ERROR("get bundle name failed for %{public}d, errCode: = %{public}d.", uid, ret);
    }
    LOG_INFO("bundleName = %{public}s", bundleName.c_str());
    return bundleName;
}

int32_t EcologicalRuleMgrService::GetSelfSceneCode(std::string &sceneCode)
{
    LOG_INFO("GetSelfSceneCode start");
    pid_t uid = GetCallingUid();
    std::string bundleName = GetBundleNameByUid(uid);
    int32_t res = EcologicalRuleEngineCache::GetInstance().GetSceneCode(bundleName, 2, sceneCode);
    if (res != SUCCESS) {
        LOG_ERROR("GetSceneCode fail: %{public}d", res);
        return ERROR;
    }
    LOG_INFO("GetSelfSceneCode end, sceneCode = %{public}s", sceneCode.c_str());
    return SUCCESS;
}

int32_t EcologicalRuleMgrService::GetAdsVerificationVersion(int32_t &version)
{
    version = 1;
    LOG_INFO("GetAdsVerificationVersion version = %{public}d", version);
    return 0;
}

void *EcologicalRuleMgrService::StartLoadRuleEngineData(void *args)
{
    LOG_INFO("start load data file");
    // load rule strategy file
    EcologicalRuleEngineCache::GetInstance().GetRuleControlStrategyDataLock();
    EcologicalRuleEngineCache::GetInstance().LoadRuleStrategyFile();
    EcologicalRuleCalculatorImpl::GetInstance()->ConstructRuleStrategyObjects();
    EcologicalRuleEngineCache::GetInstance().ClearRuleControlStrategyCache();
    EcologicalRuleEngineCache::GetInstance().ReleaseRuleControlStrategyDataLock();
    
    // load app enhance file, locked inner
    EcologicalRuleEngineCache::GetInstance().LoadAppEnhanceFile();
    // load scene config file, locked inner
    EcologicalRuleEngineCache::GetInstance().LoadSceneConfigFile();
    return nullptr;
}

void EcologicalRuleMgrService::LoadRuleConfigData()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int32_t ret = pthread_create(&loadThreadId, &attr, StartLoadRuleEngineData, NULL);
    if (ret != 0) {
        LOG_ERROR("create load rule thread fail");
    }
}

void EcologicalRuleMgrService::OnStart()
{
    LOG_INFO("EcologicalRuleMgrService OnStart");
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        LOG_WARN("EcologicalRuleMgrService is already running");
        return;
    }

    LoadRuleConfigData();

    bool ret = Publish(EcologicalRuleMgrService::GetInstance());
    if (!ret) {
        LOG_ERROR("Publish ecological rule mgr service failed: %{public}d", ret);
        return;
    }
    state_ = ServiceRunningState::STATE_RUNNING;
    LOG_INFO("EcologicalRuleMgrService OnStart finish");
    return;
}

void EcologicalRuleMgrService::OnStop()
{
    if (state_ != ServiceRunningState::STATE_RUNNING) {
        LOG_WARN("EcologicalRuleMgrService is not running");
        return;
    }
    state_ = ServiceRunningState::STATE_NOT_START;
    LOG_INFO("EcologicalRuleMgrService OnStop finish");
}
} // namespace EcologicalRuleMgrService
} // namespace OHOS