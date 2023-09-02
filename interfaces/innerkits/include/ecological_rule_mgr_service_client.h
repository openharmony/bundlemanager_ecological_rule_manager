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

#ifndef INTERFACES_INNERKITS_INCLUDE_ECOLOGICALRULEMGRSERVICE_CLIENT_H
#define INTERFACES_INNERKITS_INCLUDE_ECOLOGICALRULEMGRSERVICE_CLIENT_H

#include <mutex>
#include "refbase.h"
#include "iremote_object.h"
#include "ecological_rule_mgr_service_interface.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
using namespace std;
using namespace AppExecFwk;

class EcologicalRuleMgrServiceDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    EcologicalRuleMgrServiceDeathRecipient();
    ~EcologicalRuleMgrServiceDeathRecipient() = default;
    void OnRemoteDied(const wptr<IRemoteObject> &object) override;

private:
    DISALLOW_COPY_AND_MOVE(EcologicalRuleMgrServiceDeathRecipient);
};

class EcologicalRuleMgrServiceClient : public RefBase {
public:
    DISALLOW_COPY_AND_MOVE(EcologicalRuleMgrServiceClient);
    static sptr<EcologicalRuleMgrServiceClient> GetInstance();
    void OnRemoteSaDied(const wptr<IRemoteObject> &object);

    int32_t QueryFreeInstallExperience(const AAFwk::Want &want, const CallerInfo &callerInfo, ExperienceRule &rule);
    int32_t EvaluateResolveInfos(const AAFwk::Want &want, const CallerInfo &callerInfo, int32_t type,
        vector<AbilityInfo> &abInfos, const vector<ExtensionAbilityInfo> &extInfos = vector<ExtensionAbilityInfo>());
    int32_t QueryStartExperience(const AAFwk::Want &want, const CallerInfo &callerInfo, ExperienceRule &rule);
    int32_t IsSupportPublishForm(const vector<AAFwk::Want> &wants, const CallerInfo &callerInfo, bool &bSupport);

private:
    EcologicalRuleMgrServiceClient();
    ~EcologicalRuleMgrServiceClient();
    static sptr<EcologicalRuleMgrServiceInterface> ConnectService();
    static bool CheckConnectService();

    static mutex instanceLock_;
    static sptr<EcologicalRuleMgrServiceClient> instance_;
    static sptr<EcologicalRuleMgrServiceInterface> ecologicalRuleMgrServiceProxy_;
    static sptr<EcologicalRuleMgrServiceDeathRecipient> deathRecipient_;

    static string ERMS_ORIGINAL_TARGET;
};
} // namespace EcologicalRuleMgrService
} // namespace OHOS

#endif // INTERFACES_INNERKITS_INCLUDE_EcologicalRuleMgrService_CLIENT_H