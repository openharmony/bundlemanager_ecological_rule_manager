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
#include <thread>
#include "ecological_rule_mgr_service.h"
#include "system_ability_definition.h"
#include "iservice_registry.h"
#include "if_system_ability_manager.h"
#include "uri.h"
#include "want.h"
#include "ecological_rule_mgr_service_logger.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
#define TAG "ERMS_MAIN"

REGISTER_SYSTEM_ABILITY_BY_ID(EcologicalRuleMgrService, 6105, true);

std::mutex EcologicalRuleMgrService::instanceLock_;
sptr<EcologicalRuleMgrService> EcologicalRuleMgrService::instance_;

EcologicalRuleMgrService::EcologicalRuleMgrService() : state_(ServiceRunningState::STATE_NOT_START)
{
}

EcologicalRuleMgrService::EcologicalRuleMgrService(int32_t saId, bool runOnCreate)
    : SystemAbility(saId, runOnCreate), state_(ServiceRunningState::STATE_NOT_START)
{
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
    LOG_DEBUG("QueryFreeInstallExperience want name = %{public}s, caller name = %{public}s",
        want.GetBundle().c_str(), callerInfo.packageName.c_str());
    return SUCCESS;
}

int32_t EcologicalRuleMgrService::EvaluateResolveInfos(const Want &want, const CallerInfo &callerInfo, int32_t type,
    std::vector<AbilityInfo> &abilityInfos)
{
    LOG_DEBUG("EvaluateResolveInfos want name = %{public}s, caller name = %{public}s, type = %{public}d",
        want.GetBundle().c_str(), callerInfo.packageName.c_str(), type);
    return SUCCESS;
}

int32_t EcologicalRuleMgrService::QueryStartExperience(const Want &want, const CallerInfo &callerInfo,
    ExperienceRule &rule)
{
    std::string bundleName = want.GetBundle();
    LOG_INFO("want bundle name = %{public}s, uri = %{public}s", bundleName.c_str(), want.GetUriString().c_str());
    LOG_INFO("callerInfo bundle name = %{public}s, callerAppType = %{public}d, targetAppType = %{public}d",
        callerInfo.packageName.c_str(), callerInfo.callerAppType, callerInfo.targetAppType);

    return SUCCESS;
}

int32_t EcologicalRuleMgrService::IsSupportPublishForm(const std::vector<Want> &wants, const CallerInfo &callerInfo,
    bool &bSupport)
{
    std::string callerName = callerInfo.packageName;
    LOG_DEBUG("IsSupportPublishForm callerName = %s, callerType = %d", callerName.c_str(), callerInfo.callerAppType);
    return SUCCESS;
}

void EcologicalRuleMgrService::OnStart()
{
    LOG_INFO("EcologicalRuleMgrService OnStart");
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        LOG_WARN("EcologicalRuleMgrService is already running");
        return;
    }

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