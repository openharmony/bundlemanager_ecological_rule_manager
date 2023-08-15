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

#include <chrono>
#include <string>
#include <cinttypes>
#include "ecological_rule_mgr_service_logger.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "ecological_rule_mgr_service_client.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
using namespace std::chrono;

#define TAG "ERMS_CLIENT"

std::mutex EcologicalRuleMgrServiceClient::instanceLock_;
sptr<EcologicalRuleMgrServiceClient> EcologicalRuleMgrServiceClient::instance_;
sptr<EcologicalRuleMgrServiceInterface> EcologicalRuleMgrServiceClient::ecologicalRuleMgrServiceProxy_;
sptr<EcologicalRuleMgrServiceDeathRecipient> EcologicalRuleMgrServiceClient::deathRecipient_;

std::string EcologicalRuleMgrServiceClient::ERMS_ORIGINAL_TARGET = "ecological_experience_original_target";

inline int64_t GetCurrentTimeMicro()
{
    return duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
}

EcologicalRuleMgrServiceClient::EcologicalRuleMgrServiceClient() {}

EcologicalRuleMgrServiceClient::~EcologicalRuleMgrServiceClient()
{
    if (ecologicalRuleMgrServiceProxy_ != nullptr) {
        auto remoteObj = ecologicalRuleMgrServiceProxy_->AsObject();
        if (remoteObj != nullptr) {
            remoteObj->RemoveDeathRecipient(deathRecipient_);
        }
    }
}

sptr<EcologicalRuleMgrServiceClient> EcologicalRuleMgrServiceClient::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> autoLock(instanceLock_);
        if (instance_ == nullptr) {
            instance_ = new EcologicalRuleMgrServiceClient;
        }
    }
    return instance_;
}

sptr<EcologicalRuleMgrServiceInterface> EcologicalRuleMgrServiceClient::ConnectService()
{
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        LOG_ERROR("GetSystemAbilityManager error");
        return nullptr;
    }

    auto systemAbility = samgr->CheckSystemAbility(6105);
    if (systemAbility == nullptr) {
        LOG_ERROR("CheckSystemAbility error, ECOLOGICALRULEMANAGERSERVICE_ID = 6105");
        return nullptr;
    }

    deathRecipient_ = new EcologicalRuleMgrServiceDeathRecipient();
    systemAbility->AddDeathRecipient(deathRecipient_);

    return iface_cast<EcologicalRuleMgrServiceInterface>(systemAbility);
}

bool EcologicalRuleMgrServiceClient::CheckConnectService()
{
    if (ecologicalRuleMgrServiceProxy_ == nullptr) {
        LOG_WARN("redo ConnectService");
        ecologicalRuleMgrServiceProxy_ = ConnectService();
    }
    if (ecologicalRuleMgrServiceProxy_ == nullptr) {
        LOG_ERROR("Connect SA Failed");
        return false;
    }
    return true;
}

void EcologicalRuleMgrServiceClient::OnRemoteSaDied(const wptr<IRemoteObject> &object)
{
    ecologicalRuleMgrServiceProxy_ = ConnectService();
}

int32_t EcologicalRuleMgrServiceClient::QueryFreeInstallExperience(const OHOS::AAFwk::Want &want,
    const CallerInfo &callerInfo, ExperienceRule &rule)
{
    int64_t start = GetCurrentTimeMicro();
    LOG_DEBUG("QueryFreeInstallExperience want = %{public}s, callerInfo = %{public}s", want.ToString().c_str(),
        callerInfo.ToString().c_str());
    if (callerInfo.packageName.find_first_not_of(' ') == std::string::npos) {
        rule.isAllow = true;
        LOG_DEBUG("callerInfo packageName is empty, allow = true");
        return 0;
    }

    if (!CheckConnectService()) {
        return -1;
    }
    int32_t res = ecologicalRuleMgrServiceProxy_->QueryFreeInstallExperience(want, callerInfo, rule);
    if (rule.replaceWant != nullptr) {
        rule.replaceWant->SetParam(ERMS_ORIGINAL_TARGET, want.ToString());
        LOG_DEBUG("QueryFreeInstallExperience  isAllow = %{public}d, replaceWant = %{public}s", rule.isAllow,
            (*(rule.replaceWant)).ToString().c_str());
    }
    int64_t cost = GetCurrentTimeMicro() - start;
    LOG_DEBUG("[ERMS-DFX] QueryFreeInstallExperience interface cost %{public}" PRId64 " mirco seconds.", cost);
    return res;
}

int32_t EcologicalRuleMgrServiceClient::EvaluateResolveInfos(const AAFwk::Want &want, const CallerInfo &callerInfo,
    int32_t type, vector<AbilityInfo> &abilityInfos, const vector<ExtensionAbilityInfo> &extInfos)
{
    int64_t start = GetCurrentTimeMicro();
    LOG_DEBUG("want: %{public}s, callerInfo: %{public}s, type: %{public}d", want.ToString().c_str(),
        callerInfo.ToString().c_str(), type);
    if (!CheckConnectService()) {
        return -1;
    }
    int32_t res = ecologicalRuleMgrServiceProxy_->EvaluateResolveInfos(want, callerInfo, type, abilityInfos);
    int64_t cost = GetCurrentTimeMicro() - start;
    LOG_DEBUG("[ERMS-DFX] EvaluateResolveInfos interface cost %{public}" PRId64 " mirco seconds.", cost);
    return res;
}

int32_t EcologicalRuleMgrServiceClient::QueryStartExperience(const OHOS::AAFwk::Want &want,
    const CallerInfo &callerInfo, ExperienceRule &rule)
{
    int64_t start = GetCurrentTimeMicro();
    LOG_DEBUG("callerInfo: %{public}s, want: %{public}s", callerInfo.ToString().c_str(), want.ToString().c_str());
    if (callerInfo.packageName.find_first_not_of(' ') == std::string::npos) {
        rule.isAllow = true;
        LOG_DEBUG("callerInfo packageName is empty, allow = true");
        return 0;
    }

    if (!CheckConnectService()) {
        return -1;
    }
    int32_t res = ecologicalRuleMgrServiceProxy_->QueryStartExperience(want, callerInfo, rule);
    if (rule.replaceWant != nullptr) {
        rule.replaceWant->SetParam(ERMS_ORIGINAL_TARGET, want.ToString());
        LOG_DEBUG("queryStart finish: rule.isAllow = %{public}d, rule.sceneCode = %{public}s, replaceWant = %{public}s",
            rule.isAllow, rule.sceneCode.c_str(), (*(rule.replaceWant)).ToString().c_str());
    }
    int64_t cost = GetCurrentTimeMicro() - start;
    LOG_DEBUG("[ERMS-DFX] QueryStartExperience interface cost %{public}" PRId64 " mirco seconds.", cost);
    return res;
}

int32_t EcologicalRuleMgrServiceClient::IsSupportPublishForm(const std::vector<OHOS::AAFwk::Want> &wants,
    const CallerInfo &callerInfo, bool &bSupport)
{
    int64_t start = GetCurrentTimeMicro();
    LOG_DEBUG("callerInfo = %{public}s", callerInfo.ToString().c_str());
    if (callerInfo.packageName.find_first_not_of(' ') == std::string::npos) {
        bSupport = true;
        LOG_DEBUG("callerInfo packageName is empty, bSupport = true");
        return 0;
    }
    if (!CheckConnectService()) {
        return -1;
    }
    int32_t res = ecologicalRuleMgrServiceProxy_->IsSupportPublishForm(wants, callerInfo, bSupport);
    LOG_DEBUG("IsSupportPublishForm end, bSupport = %{public}d", bSupport);
    int64_t cost = GetCurrentTimeMicro() - start;
    LOG_DEBUG("[ERMS-DFX] IsSupportPublishForm interface cost %{public}" PRId64 " mirco seconds.", cost);
    return res;
}

EcologicalRuleMgrServiceDeathRecipient::EcologicalRuleMgrServiceDeathRecipient() {}

void EcologicalRuleMgrServiceDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    EcologicalRuleMgrServiceClient::GetInstance()->OnRemoteSaDied(object);
}

int32_t EcologicalRuleMgrServiceClient::SetAppEnhancedData(const int32_t &operType, const std::string &appData)
{
    LOG_DEBUG("operType = %{public}d appData = %{public}s", operType, appData.c_str());
    if (!CheckConnectService()) {
        return -1;
    }
    return ecologicalRuleMgrServiceProxy_->SetAppEnhancedData(operType, appData);
}

int32_t EcologicalRuleMgrServiceClient::SetRuleInfo(const std::string &ruleInfo)
{
    LOG_DEBUG("ruleInfo = %{public}s", ruleInfo.c_str());
    if (!CheckConnectService()) {
        return -1;
    }
    return ecologicalRuleMgrServiceProxy_->SetRuleInfo(ruleInfo);
}

int32_t EcologicalRuleMgrServiceClient::ExemptExperience(const std::string &targetBundleName, const int32_t &rule,
    const int32_t &timestamp)
{
    LOG_DEBUG("target: %{public}s rule: %{public}d timestamp: %{public}d", targetBundleName.c_str(), rule, timestamp);
    if (!CheckConnectService()) {
        return -1;
    }
    return ecologicalRuleMgrServiceProxy_->ExemptExperience(targetBundleName, rule, timestamp);
}

int32_t EcologicalRuleMgrServiceClient::GetVersion(std::string &versionJson)
{
    if (!CheckConnectService()) {
        return -1;
    }
    return ecologicalRuleMgrServiceProxy_->GetVersion(versionJson);
}

int32_t EcologicalRuleMgrServiceClient::SetSceneExperience(std::string &ruleConfig, std::string &sceneExperience)
{
    LOG_DEBUG("ruleConfig = %{public}s sceneExperience = %{public}s", ruleConfig.c_str(), sceneExperience.c_str());
    if (!CheckConnectService()) {
        return -1;
    }
    return ecologicalRuleMgrServiceProxy_->SetSceneExperience(ruleConfig, sceneExperience);
}

int32_t EcologicalRuleMgrServiceClient::GetSceneCode(const std::string &bundleName, std::string &sceneCode)
{
    LOG_DEBUG("GetSceneCode bundleName = %{public}s", bundleName.c_str());
    if (!CheckConnectService()) {
        return -1;
    }
    int32_t res = ecologicalRuleMgrServiceProxy_->GetSceneCode(bundleName, sceneCode);
    LOG_DEBUG("GetSceneCode end, sceneCode = %{public}s", sceneCode.c_str());
    return res;
}

int32_t EcologicalRuleMgrServiceClient::GetInitialSceneCode(const std::string &bundleName,
    std::string &originalSceneCode)
{
    LOG_DEBUG("GetInitialSceneCode bundleName = %{public}s", bundleName.c_str());
    if (!CheckConnectService()) {
        return -1;
    }
    int32_t res = ecologicalRuleMgrServiceProxy_->GetInitialSceneCode(bundleName, originalSceneCode);
    LOG_DEBUG("GetInitialSceneCode end, originalSceneCode = %{public}s", originalSceneCode.c_str());
    return res;
}

int32_t EcologicalRuleMgrServiceClient::GetSelfSceneCode(std::string &sceneCode)
{
    if (!CheckConnectService()) {
        return -1;
    }
    int32_t res = ecologicalRuleMgrServiceProxy_->GetSelfSceneCode(sceneCode);
    LOG_DEBUG("GetSelfSceneCode end, sceneCode = %{public}s", sceneCode.c_str());
    return res;
}

int32_t EcologicalRuleMgrServiceClient::GetAdsVerificationVersion(int32_t &version)
{
    if (!CheckConnectService()) {
        return -1;
    }
    int32_t res = ecologicalRuleMgrServiceProxy_->GetAdsVerificationVersion(version);
    LOG_DEBUG("GetAdsVerificationVersion end, version = %{public}d", version);
    return res;
}
} // namespace EcologicalRuleMgrService
} // namespace OHOS