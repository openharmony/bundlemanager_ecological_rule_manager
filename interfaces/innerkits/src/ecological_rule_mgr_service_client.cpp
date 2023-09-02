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
} // namespace EcologicalRuleMgrService
} // namespace OHOS