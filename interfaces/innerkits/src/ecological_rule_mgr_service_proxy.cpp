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

#include "ecological_rule_mgr_service_proxy.h"
#include "iremote_broker.h"
#include "ecological_rule_mgr_service_logger.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
#define TAG "erms_proxy"
static inline const std::u16string ERMS_INTERFACE_TOKEN =
    u"ohos.cloud.ecologicalrulemgrservice.IEcologicalRuleMgrService";

EcologicalRuleMgrServiceProxy::EcologicalRuleMgrServiceProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<EcologicalRuleMgrServiceInterface>(object)
{}

int32_t EcologicalRuleMgrServiceProxy::QueryFreeInstallExperience(const Want &want, const CallerInfo &callerInfo,
    ExperienceRule &rule)
{
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceProxy::EvaluateResolveInfos(const Want &want, const CallerInfo &callerInfo,
    int32_t type, std::vector<AbilityInfo> &abilityInfos)
{
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceProxy::QueryStartExperience(const Want &want, const CallerInfo &callerInfo,
    ExperienceRule &rule)
{
    return ERR_OK;
}

int32_t EcologicalRuleMgrServiceProxy::IsSupportPublishForm(const std::vector<Want> &wants,
    const CallerInfo &callerInfo, bool &bSupport)
{
    return ERR_OK;
}

template <typename T>
bool EcologicalRuleMgrServiceProxy::ReadParcelableVector(std::vector<T> &parcelableVector, MessageParcel &reply)
{
    int32_t infoSize = reply.ReadInt32();
    parcelableVector.clear();
    for (int32_t i = 0; i < infoSize; i++) {
        sptr<T> info = reply.ReadParcelable<T>();
        if (info == nullptr) {
            LOG_ERROR("read Parcelable infos failed");
            return false;
        }
        parcelableVector.emplace_back(*info);
    }
    return true;
}
} // namespace EcologicalRuleMgrService
} // namespace OHOS
