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

#ifndef SERVICES_INCLUDE_ECOLOGICALRULEMANAGERSERVICE_PROXY_H
#define SERVICES_INCLUDE_ECOLOGICALRULEMANAGERSERVICE_PROXY_H

#include <mutex>
#include "iremote_proxy.h"
#include "ecological_rule_mgr_service_interface.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
class EcologicalRuleMgrServiceProxy : public OHOS::IRemoteProxy<EcologicalRuleMgrServiceInterface> {
public:
    using Want = OHOS::AAFwk::Want;
    using AbilityInfo = OHOS::AppExecFwk::AbilityInfo;

    explicit EcologicalRuleMgrServiceProxy(const sptr<IRemoteObject> &object);
    ~EcologicalRuleMgrServiceProxy() = default;
    DISALLOW_COPY_AND_MOVE(EcologicalRuleMgrServiceProxy);

    int32_t QueryFreeInstallExperience(const Want &want, const CallerInfo &callerInfo, ExperienceRule &rule) override;
    int32_t EvaluateResolveInfos(const Want &want, const CallerInfo &callerInfo, int32_t type,
        std::vector<AbilityInfo> &abilityInfos) override;
    int32_t QueryStartExperience(const Want &want, const CallerInfo &callerInfo, ExperienceRule &rule) override;
    int32_t IsSupportPublishForm(const std::vector<Want> &wants, const CallerInfo &callerInfo, bool &bSupport) override;

    template <typename T> bool ReadParcelableVector(std::vector<T> &parcelableVector, MessageParcel &reply);

private:
    static inline BrokerDelegator<EcologicalRuleMgrServiceProxy> delegator_;
};
} // namespace EcologicalRuleMgrService
} // namespace OHOS

#endif // SERVICES_INCLUDE_ECOLOGICALRULEMANAGERSERVICE_PROXY_H
